#!/usr/bin/env python3
"""Vector table and startup regression checks for a firmware ELF.

Standard library plus the toolchain binutils (arm-none-eabi-readelf,
arm-none-eabi-nm, arm-none-eabi-objcopy, arm-none-eabi-objdump).

Usage: check_elf.py <firmware.elf>
"""

import re
import struct
import subprocess
import sys
import tempfile

BIN = "arm-none-eabi-"
FLASH = 0x08000000
TABLE_WORDS = 118
TABLE_SIZE = TABLE_WORDS * 4
# ARMv7-M core exception slots with no handler (table indices 1..15)
RESERVED = (7, 8, 9, 10, 13)
RESERVED_NAMES = {
    7: "Reserved7", 8: "Reserved8", 9: "Reserved9",
    10: "Reserved10", 13: "Reserved13",
}

failures = []


def fail(msg):
    print(f"FAIL: {msg}")
    failures.append(msg)


def ok(msg):
    print(f"PASS: {msg}")


def run(cmd):
    return subprocess.run(cmd, capture_output=True, text=True)


def section_addr_size(elf):
    out = run([BIN + "readelf", "-S", elf]).stdout
    m = re.search(
        r"\.isr_vector\s+\S+\s+([0-9a-f]+)\s+([0-9a-f]+)\s+([0-9a-f]+)", out
    )
    if not m:
        return None
    return int(m.group(1), 16), int(m.group(3), 16)


def symbol_value(elf, name):
    out = run([BIN + "nm", elf]).stdout
    m = re.search(rf"^([0-9a-f]+) [A-Za-z] {re.escape(name)}$", out, re.M)
    if not m:
        return None
    return int(m.group(1), 16)


def table_bytes(elf):
    with tempfile.NamedTemporaryFile(suffix=".bin") as tmp:
        r = run([BIN + "objcopy", "-O", "binary", "-j", ".isr_vector", elf, tmp.name])
        if r.returncode != 0:
            return None
        return open(tmp.name, "rb").read()


def check_layout(elf):
    got = section_addr_size(elf)
    if got is None:
        fail("section .isr_vector not found in ELF")
        return
    addr, size = got
    if addr != FLASH:
        fail(f".isr_vector is at 0x{addr:08x}, expected 0x{FLASH:08x}")
    else:
        ok(f".isr_vector at 0x{FLASH:08x}")
    if size != TABLE_SIZE:
        fail(f".isr_vector is {size} bytes, expected {TABLE_SIZE}")
    else:
        ok(f".isr_vector size {TABLE_SIZE}")


def check_entries(elf, words):
    stack_top = symbol_value(elf, "__stack_top")
    reset = symbol_value(elf, "Reset_Handler")
    if stack_top is None:
        fail("symbol __stack_top not found")
        return
    if reset is None:
        fail("symbol Reset_Handler not found")
        return

    if words[0] != stack_top:
        fail(f"entry 0 is 0x{words[0]:08x}, expected __stack_top = 0x{stack_top:08x}")
    else:
        ok(f"entry 0 is __stack_top (0x{stack_top:08x})")

    expected_reset = reset | 1
    if words[1] != expected_reset:
        fail(
            f"entry 1 is 0x{words[1]:08x}, expected Reset_Handler|Thumb "
            f"= 0x{expected_reset:08x}"
        )
    else:
        ok(f"entry 1 is Reset_Handler with Thumb bit (0x{expected_reset:08x})")

    bad_reserved = [i for i in RESERVED if words[i] != 0]
    for i in bad_reserved:
        fail(f"entry {i} ({RESERVED_NAMES[i]}) is 0x{words[i]:08x}, expected 0")
    if not bad_reserved:
        ok("reserved core slots are zero")

    bad = [
        i for i in range(2, TABLE_WORDS)
        if i not in RESERVED and not (words[i] & 1)
    ]
    for i in bad:
        what = f"IRQn {i - 16}" if i >= 16 else "core slot"
        fail(f"entry {i} ({what}) is 0x{words[i]:08x}, missing Thumb bit or null")
    if not bad:
        ok("all other used entries have the Thumb bit set")


def disasm_reset_handler(elf):
    out = run(
        [BIN + "objdump", "-d", "--disassemble=Reset_Handler", elf]
    ).stdout
    instrs = []
    for line in out.splitlines():
        parts = line.split("\t")
        if len(parts) < 3 or not re.match(r"^\s*[0-9a-f]+:$", parts[0]):
            continue
        addr = int(parts[0].strip().rstrip(":"), 16)
        instrs.append((addr, parts[2].strip()))
    return instrs


def check_fpu_before_enable(elf):
    instrs = disasm_reset_handler(elf)
    if not instrs:
        fail("could not disassemble Reset_Handler")
        return

    # A CPACR write takes effect after a context synchronization event, i.e. the
    # isb. A fresh reset has CPACR = 0, so any FPU instruction before the first
    # isb would raise UsageFault. In binutils' ARM output, VFP/NEON instructions
    # carry a v-prefixed mnemonic and no non-FPU instruction uses the v prefix.
    # A call (bl/blx) before the isb is equally forbidden: the callee may
    # execute FPU instructions that this disassembly of Reset_Handler cannot see.
    isb_idx = next((i for i, (_, m) in enumerate(instrs) if m == "isb"), None)
    limit = isb_idx if isb_idx is not None else len(instrs)
    at = f" before the first isb at 0x{instrs[isb_idx][0]:08x}" if isb_idx is not None else ""

    bad = [(addr, m) for (addr, m) in instrs[:limit] if re.match(r"^v[a-z]", m)]
    for addr, m in bad:
        fail(f"FPU instruction {m!r} at 0x{addr:08x} in Reset_Handler{at}")
    if not bad:
        ok(f"no FPU instruction in Reset_Handler{at} (FPU enable takes effect)")

    bad_calls = [(addr, m) for (addr, m) in instrs[:limit] if m in ("bl", "blx")]
    for addr, m in bad_calls:
        fail(f"call {m!r} at 0x{addr:08x} in Reset_Handler{at}; the callee may use the FPU")
    if not bad_calls:
        ok(f"no call (bl/blx) in Reset_Handler{at}")


def main():
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <firmware.elf>", file=sys.stderr)
        return 2
    elf = sys.argv[1]

    check_layout(elf)

    data = table_bytes(elf)
    if data is None:
        fail("could not extract .isr_vector bytes")
        return 1
    if len(data) != TABLE_SIZE:
        words = struct.unpack(f"<{len(data)//4}I", data[: len(data)//4*4])
    else:
        words = struct.unpack(f"<{TABLE_WORDS}I", data)

    check_entries(elf, words)
    check_fpu_before_enable(elf)

    if failures:
        print(f"\n{len(failures)} check(s) failed")
        return 1
    print("\nall checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())

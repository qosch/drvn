# AGENTS.md

Rules for AI agents working in this repository.

If `.notes/WORKFLOW.md` exists, read it first. It adds workflow rules. All rules in this file still apply.

## Project

Field-oriented control (FOC) firmware for permanent magnet motors. Target MCU: STM32G474VE (Cortex-M4F, 170 MHz, 512 KB flash). Language: C++20. The firmware is the product. `boards/devboard` is the first board. Its hardware is described in `boards/devboard/hardware.md`.

Everything written to files is in English: code, comments, documentation, notes and commit messages.

## Layout

- `mcu/`: STM32G4 support. Linker script, startup code, peripheral building blocks. Knows no application.
- `drivers/`: external components (TLE5012B, WS2812, ...). Know no pins. They get their peripheral instances from the board.
- `boards/<name>/`: pins, instances, power stage data. Selects chip define and linker script.
- `control/`: FOC math. Knows no hardware.
- `app/`: application. Accesses hardware only through `board.hpp`.
- `tests/`: host tests (doctest).
- `tools/`: host application and scripts.
- `external/`: pinned submodules. Never modify.
- `docs/reference/`: manuals and datasheets. Not versioned.
- `docs/decisions/`: decision records.

Dependencies point one way: `app` -> `boards` -> `drivers`, `mcu` -> `external`. Every folder is a CMake target with explicit dependencies. Do not add a dependency against this direction.

`control/` depends on the standard library and Au only. It has no CMSIS include paths and must build and pass its tests on the host.

Every board provides the same names in `board.hpp`. No relative includes across folder boundaries.

## Build and test

Presets: `devboard-debug`, `devboard-release`, `host-tests`.

```
cmake --preset <preset>
cmake --build --preset <preset>
ctest --preset <preset>
```

- Arm GNU Toolchain, CMake, Ninja. Flashing, debugging and RTT through probe-rs.
- Debug builds use `-Og`, never `-O0`. At `-O0` the zero-cost wrappers become real function calls and the current loop misses its deadline.
- A change is done when all presets build without warnings and all tests pass.
- Do not install system-wide software. If a tool is missing, say so.

## Coding standard

Hard constraints:

- No dynamic allocation. No `new`, no `malloc`, no allocating containers. Reason: bounded memory, no fragmentation, no allocation failure at runtime.
- No exceptions, no RTTI. Reason: code size and predictable timing.
- No recursion. Reason: stack usage must be bounded.
- No virtual functions in interrupt context. Use compile-time polymorphism (templates, concepts). Reason: interrupt latency.
- The current loop runs in the task with the highest priority. The ADC interrupt wakes it with a direct-to-task notification and checks that the previous cycle has finished. The task blocks only on this notification.
- Every critical section and every interrupt handler in the FreeRTOS priority range adds to the worst-case latency of the current loop. Keep them short.
- Overcurrent shutdown never depends on a task. It runs in hardware or in an interrupt above the priorities FreeRTOS masks.
- Operator overloading only for math types.
- No ST HAL, no CubeMX, no generated code. Use the CMSIS device headers and the LL drivers behind our own thin C++ layer in `mcu/`.

These rules are enforced by compiler flags and clang-tidy where possible.

Naming and formatting follow the Google C++ Style Guide:

- Types and functions: `PascalCase`. Variables and parameters: `snake_case`. Class data members: `snake_case_` with a trailing underscore. Constants and enumerators: `kPascalCase`. Namespaces: `snake_case`.
- Files: `snake_case.cpp` and `snake_case.hpp` (deviation from Google's `.cc` and `.h`).
- Formatting: clang-format with `.clang-format` in the repository root, based on the Google style.
- Names fixed by others keep their spelling: CMSIS and LL identifiers, interrupt handlers such as `Reset_Handler`, linker script symbols such as `__stack_top`.
- Only the naming and formatting rules of the Google guide apply. The constraints in this file take precedence.

Physical quantities and domain types:

- Physical quantities use Au. No bare `float` for amperes, volts or seconds outside conversion code.
- Own domain types cover what units cannot: electrical vs. mechanical angle, alpha-beta vs. dq frame, raw ADC counts vs. amperes.
- Angles are wrapping `uint32_t`. 2^32 is one revolution.
- Raw values (ADC counts, encoder bits, timer compare values) become physical quantities in `boards/` and `drivers/` only. `control/` never sees raw integers.

## References

- Look things up in `docs/reference/`. Never take register names, bit positions, timings or section numbers from memory.
- Core peripherals (SCB, NVIC, SysTick, MPU, FPU): PM0214. All other peripherals: RM0440. Electrical data, pinout, memory map: the datasheet. Check the errata sheet for every peripheral you touch.
- Register accesses get a bare reference, for example `// RM0440 §N.N.N` or `// PM0214 §N.N.N`.
- If you cannot find a source for a claim about the hardware or a library, say so. Do not fill the gap with a plausible guess.

## Ask instead of guessing

Stop and ask when:

- the task is ambiguous or the manual leaves a question open,
- a change would alter an existing public interface,
- you want to add a dependency or submodule,
- you want to change the linker script, the startup code or the toolchain file,
- a fix would require changes in `external/`,
- you would have to break a rule in this file.

## Git

- Only the maintainer commits and pushes.
- You may stage (`git add`, `git restore --staged`) to prepare a commit. Prepare one commit at a time.
- If only part of a file belongs to a commit, stage only that part. Never edit the working tree to make a commit fit.
- Verify a staged state in a copy outside the repository: clean configure, build and test of every preset that exists in that state. Then report `git diff --cached --stat`, propose the message and wait.
- Create the copy with `git checkout-index --all --prefix=<dir>/`. This writes the staged files to `<dir>` and changes nothing in the repository. It is allowed; it is not `checkout`. Submodules are not included: link them from the working tree into `<dir>`.
- Apart from staging, use git read-only: `status`, `diff`, `log`, `show`, `blame`. No `commit`, `checkout`, `switch`, `stash`, `reset`, `rebase` or `push`.
- Never delete uncommitted files without asking.
- The maintainer may allow more for a specific task.
- Propose commit messages as one line in imperative mood, at most 72 characters, no body.
- Commit messages are not documentation. Reasons belong in code comments or in `docs/decisions/`.


## Hardware

- Flashing, debugging and RTT need the maintainer's OK. He gives it at session start for the whole session, or you ask when you need it.
- Only one session uses the probe at a time. Before using it, check that no other probe-rs process is running.
- Protected code can destroy hardware: dead time, PWM output configuration, break input, power stage driver. Only the maintainer flashes firmware with unverified changes to protected code.
- The tag `power-stage-verified` marks the last state the maintainer verified on hardware. Only he moves it. Before every flash, check the protected paths for changes against this tag, including uncommitted and untracked files. If there are changes, do not flash. Tell the maintainer.
- Protected paths: none yet. They are listed here when the files exist.

## Writing style

This applies to everything you write: code comments, documentation, notes, commit messages and chat replies.

- Short declarative sentences. Simple words.
- No dashes as punctuation (em dash, en dash, spaced hyphen). Use a period, comma, colon or parentheses.
- No filler or promotional words: "robust", "seamless", "leverage", "comprehensive", "crucial", "ensures that", "it's important to note", "in order to".
- No emojis. No bold or italics in running text. No closing summary. No offers of further help.
- State facts, not judgments. "Takes 1.2 µs at 170 MHz", not "is very fast".
- Do not restate the task before doing it.

Not like this:

> This change introduces a robust and efficient mechanism — ensuring that DMA channels are seamlessly allocated at compile time.

Like this:

> DMA channels are now assigned at compile time. Assigning a channel twice fails to compile.

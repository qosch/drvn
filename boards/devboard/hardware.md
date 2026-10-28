# DRVN Dev Base — Firmware Reference (Pin Allocation & Interfaces)

Primary evidence: the **physical PCB netlist** (`production/netlist.ipc`), the
hierarchical global netlist (`drvn_dev_base.net`), `production/bom.csv`,
`production/positions.csv` and the schematic sheets.

> **Source-of-truth rules**
> - Where the schematic sub-sheets, the flattened netlist and the PCB disagree,
>   the **PCB netlist wins** (it is what is actually routed and populated).
> - Several sub-sheets (`enc_ssi.kicad_sch`, `enc_incremental.kicad_sch`, …)
>   contain **outdated reference designators / net names** — do not trust them
>   for pin truth; use this document (see anomaly A7).
> - PCB IPC net names are truncated to a fixed width (e.g. `LLER/MOTORPWMU` =
>   `/microcontroller/MotorPwmU`); full names are restored in this document.
> - `PinPlanning.png` was deliberately **not** used.

---

## 1. MCU

| | |
|---|---|
| Reference | `U10` |
| Part | **STM32G474VETx** (C2970837 @ LCSC) |
| Package | LQFP-100, 14×14 mm, 0.5 mm pitch |
| Core | Cortex-M4F @ up to 170 MHz, 512 kB Flash, 128 kB RAM |
| Datasheet (ST) | **ST DS12288 Rev 6 (Oct 2025), official PDF provided** (`datasheets/DS_stm32g474ve.pdf`); LQFP-100 pinout (Fig. 9) is the basis of the §3 pin table, AF assignments are per the datasheet alternate-function table (Table 13). RM0440 (reference manual) + UM2910 Rev 5 (ST-Link) also in `datasheets/` |

### 1.1 MCU power pins (from PCB netlist)

| Pin | Name | PCB net / connection | Note |
|---|---|---|---|
| 6 | VBAT | `NET-(U10-VBAT)` | Tied to the filtered +3.3V group (no battery). No supercap/battery — VBAT behaves as a plain 3.3 V rail. |
| 24, 49, 64, 75, 100 | VDD | `NET-(U10-VBAT)` (same net as VBAT) | +3.3V through ferrite `FB5` (220 Ω @100 MHz); decoupled by C25 (22 µF) + C26…C35 (1 µF / 100 nF group). |
| 37 | VDDA | `+3.3VA` | From LDO `U5` (TPS7A2033), C13/C14 decoupling. |
| 35 | VSSA | `GNDA` | |
| 36 | VREF+ | `V_REF` | Output of `U8` (REF330, 3.30 V ±0.5 %), filtered by C17/C28/C30 (1 µF/1 µF/100 nF) to GNDA. **This is the ADC reference — do not reassign.** |
| 23, 48, 63, 74, 99 | VSS | `GND` | |

---

## 2. Clock system

- **HSE = 24 MHz crystal `Y1`** (YXC `XL7KI-111-24M`, 4-pin 2016 SMD, CL = 8 pF,
  ±10 ppm). Connected to `PF0` (pin 12, OSC_IN) and `PF1` (pin 13, OSC_OUT).
  See ⚠️ anomaly A1 — **Y1 pin 2 is soldered to +3.3V, not GND**.
- **No LSE** (PC14/PC15 are used as GPIO), **no HSE startup RC network**
  (no 1 MΩ pull-ups on PF0/PF1 in the netlist) — standard, the crystal is the
  oscillator; startup time is governed by RCC defaults.
- The PLL and APB prescaler configuration is a firmware decision — the only
  external clock input on this board is the 24 MHz HSE crystal (USB 48 MHz
  can be taken from the internal HSI48, which the G474 has).
- `PA8` (pin 69) is `RCC_MCO` and has test point `TP6` — convenient for
  measuring the system clock.

### ⚠️ A1 — Crystal Y1 pin 2 to +3.3 V (verify before first power-on)

PCB nets: `Y1.1 → PF1`, `Y1.3 → PF0`, `Y1.4 → GND`, **`Y1.2 → +3.3V`**.
The KiCad symbol used is `Device:Crystal_GND24` = “Four pin crystal, GND on
pins 2 and 4”, and YXC 2016 four-pin crystals carry the ground/shield pad on
pin 2 (datasheet YSX211SL, see §16). Tying pin 2 to +3.3 V may prevent
oscillation or damage the crystal. **Confirmed as a known issue by the
hardware designer; one board has already been reworked, the other will be
reworked before bring-up.** Also note `C12`/`C13` (100 nF caps on the +3.3V
rail) are **not** connected to Y1 — the crystal is unloaded except by its own
internal CL (see A6).

---

## 3. Complete U10 pin table (all 100 pins)

Nets are the PCB-resolved nets (schematic names where available). “—” = no
external connection (floating pad net).

| Pin | Port | PCB net (full name) | Function / notes |
|---|---|---|---|
| 1 | PE2 | `/microcontroller/EncoderSpiClk` | SPI4_SCK (AF5); clocked by timer via PE0 → R17 → PE2 (→ J12.7); see §8.2 |
| 2 | PE3 | `/microcontroller/EncoderSpi~{Cs}` | SPI4_NSS (AF5); asserted by timer via PE4 → R18 → PE3 (→ J12.6); see §8.2 |
| 3 | PE4 | (local) | **TIM3_CH3 (AF2)** — timer CS source: drives R18 → SPI4_NSS (PE3) + J12.6; see §8.2 |
| 4 | PE5 | (local) | SPI4_MISO (AF5) = **data OUT** (SPI slave); input (A) of buffer U9 → J12.10 (enabled by PE1); see §8.2 |
| 5 | PE6 | `/microcontroller/EncoderSpiSdi` | SPI4_MOSI (AF5) = **data IN** (SPI slave); ← J12.3 (encoder MISO); see §8.2 |
| 6 | VBAT | filtered +3.3V | see §1.1 |
| 7 | PC13 | `/microcontroller/UiButton` | SW2 button (100 Ω PU R35, 100 nF C50) |
| 8 | PC14 | `/microcontroller/AuxPowerSupplyEnable` | EN of buck U6 (also LSE pad — unused) |
| 9 | PC15 | `/enc_incremental/SupplyEnable` | EN of load switch U16 (also LSE pad — unused) |
| 10 | PF9 | `/enc_incremental/EncA` | TIM15_CH1 (AF3) / TIM5_CH4 (AF6) / EXTI9; from SP3485 U17 RO |
| 11 | PF10 | `/enc_incremental/EncB` | TIM15_CH2 (AF3) / EXTI10; from SP3485 U18 RO |
| 12 | PF0 | Y1 pin 3 (HSE_IN) | HSE 24 MHz (`RCC_OSC_IN`); also ADC1_IN10 |
| 13 | PF1 | Y1 pin 1 (HSE_OUT) | HSE 24 MHz (`RCC_OSC_OUT`); SPI2_SCK (AF5) also maps here |
| 14 | PG10 | `/microcontroller/~{RESET}` | NRST; SW1 (100 Ω PD R34, 100 nF C49), ST-Link A1.12 |
| 15 | PC0 | `/microcontroller/EncoderDigitalHallU` | GPIO / EXTI0; also ADC1/ADC2 IN6; from J13.5 |
| 16 | PC1 | `/microcontroller/EncoderDigitalHallV` | GPIO / EXTI1; also ADC1/ADC2 IN7; from J13.4 |
| 17 | PC2 | `/microcontroller/EncoderDigitalHallW` | GPIO / EXTI2; also ADC1/ADC2 IN8; from J13.3 |
| 18 | PC3 | `/microcontroller/BusVoltageSensFiltered` | Bus-voltage sense (from J11.14, filtered) → **OPAMP5 (voltage follower) → internal out → ADC5**; direct ADC1/ADC2 IN9 as an alternative |
| 19 | PF2 | `/microcontroller/GPIO` | named “GPIO”, no other member — effectively spare |
| 20 | PA0 | `/microcontroller/InputCurrentSensFiltered` | **ADC1/ADC2 IN1**; input (DC-link) current sense |
| 21 | PA1 | `/microcontroller/CurrentSensFilteredW` | **ADC1/ADC2 IN2**; phase W current sense |
| 22 | PA2 | `/microcontroller/CurrentSensFilteredV` | **ADC1 IN3**; phase V current sense |
| 23 | VSS | GND | |
| 24 | VDD | filtered +3.3V | §1.1 |
| 25 | PA3 | `/microcontroller/BrakingCurrentSensFiltered` | **ADC1 IN4**; braking-resistor current sense |
| 26 | PA4 | `/microcontroller/AuxPowerSupplyAdjust` | **DAC1_OUT1** (dedicated) → R14 (120 k, DNP) → U6 FB |
| 27 | PA5 | `/microcontroller/BrakeResistorCommand` | **DAC1_OUT2** (dedicated); TP5; analog command to power stage (J10.15) |
| 28 | PA6 | — | spare |
| 29 | PA7 | — | spare |
| 30 | PC4 | (local) | test point TP4 only |
| 31 | PC5 | — | spare |
| 32 | PB0 | — | spare |
| 33 | PB1 | `/microcontroller/CurrentSensFilteredU` | **ADC1 IN12** (also ADC3 IN1); phase U current sense |
| 34 | PB2 | — | spare |
| 35 | VSSA | GNDA | |
| 36 | VREF+ | `V_REF` (REF330) | §1.1 |
| 37 | VDDA | +3.3VA | §1.1 |
| 38 | PE7 | — | spare |
| 39 | PE8 | — | spare |
| 40 | PE9 | (local) | test point TP3 only |
| 41 | PE10 | — | spare |
| 42 | PE11 | — | spare |
| 43 | PE12 | `/microcontroller/EncoderAnalogHallFiltered5` | **ADC3/ADC4/ADC5 IN16** (not on ADC1/ADC2); analog Hall 5 |
| 44 | PE13 | `/microcontroller/EncoderAnalogHallFiltered4` | **ADC3 IN3** (not ADC1); analog Hall 4 |
| 45 | PE14 | `/microcontroller/EncoderAnalogHallFiltered3` | **ADC4 IN1** (not ADC1); analog Hall 3 |
| 46 | PE15 | (local) | test point TP2 only |
| 47 | PB10 | (local) | spare GPIO → R19 (0 R, DNP) → JP3.2 (SSI clock source option) |
| 48 | VSS | GND | |
| 49 | VDD | filtered +3.3V | §1.1 |
| 50 | PB11 | `/microcontroller/EncoderAnalogHallFiltered2` | **ADC1/ADC2 IN14**; analog Hall 2 |
| 51 | PB12 | `/microcontroller/EncoderAnalogHallFiltered1` | **ADC1 IN11** (also ADC4 IN3); analog Hall 1 |
| 52 | PB13 | (local, JP3.2) | SPI2_SCK (AF5); SSI clock source via JP3; see §7 |
| 53 | PB14 | `/microcontroller/SpiMiso` | **SPI2_MISO (AF5)** → J9.5 |
| 54 | PB15 | (local, JP4.2) | SPI2_MOSI (AF5); SSI data input via JP4; see §7 |
| 55 | PD8 | `/microcontroller/UartUsbTx` | **USART3_TX (AF7)** → ST-Link A1.13 |
| 56 | PD9 | `/microcontroller/UartUsbRx` | **USART3_RX (AF7)** → ST-Link A1.14 |
| 57 | PD10 | `/microcontroller/UiLedRed` | Red LED D10 via R32 (1 k), active high |
| 58 | PD11 | `/microcontroller/PowerStageStbyReset` | GPIO → J10.17 (power-stage STBY/RESET) |
| 59 | PD12 | `/microcontroller/PowerStageEnable` | GPIO → J10.18 (power-stage EN) |
| 60 | PD13 | `/microcontroller/PowerStageSpare` | GPIO → J10.16 (spare) |
| 61 | PD14 | `/microcontroller/UiLedGreen` | Green LED D9 via R31 (1 k); UI green, see §10.7 |
| 62 | PD15 | `/microcontroller/Spi~{Cs}` | **SPI2_NSS (AF6 — not AF5!)** → J9.6 |
| 63 | VSS | GND | |
| 64 | VDD | filtered +3.3V | §1.1 |
| 65 | PC6 | `/microcontroller/MotorPwmU` | **TIM8_CH1 (AF4)** |
| 66 | PC7 | `/microcontroller/MotorPwmV` | **TIM8_CH2 (AF4)** |
| 67 | PC8 | `/microcontroller/MotorPwmW` | **TIM8_CH3 (AF4)** |
| 68 | PC9 | `/microcontroller/DebugSync` | **TIM8_CH4 (AF4)** — designer intent: short pulse from the center-aligned PWM timer as a scope trigger synced to the motor PWM; also TIM8_BKIN2 / I2C3_SDA; → TP1 |
| 69 | PA8 | (local) | spare; RCC_MCO; TP6 |
| 70 | PA9 | `/microcontroller/UsbPdCfg3` | GPIO → CH224K CFG3 (pin 3) |
| 71 | PA10 | `/microcontroller/SPDIF` | **SAI1_SD_A (AF14)** (also SAI1_D1, AF12) → fiber-optic transmitter U1 (ZGY133/T9, **DNP**); designer intent: debug-data stream via SAI1 |
| 72 | PA11 | `/microcontroller/UsbD-` | **USB_DM (dedicated)** → U3 → J6 (USB 2.0 device port) |
| 73 | PA12 | `/microcontroller/UsbD+` | **USB_DP (dedicated)** → U3 → J6 |
| 74 | VSS | GND | |
| 75 | VDD | filtered +3.3V | §1.1 |
| 76 | PA13 | `/microcontroller/SWDIO` | SWDIO (dedicated) → A1.4 |
| 77 | PA14 | `/microcontroller/SWCLK` | SWCLK (dedicated) → A1.6 |
| 78 | PA15 | `/enc_ssi/SupplyEnable` | EN of load switch U19 (SSI encoder supply) |
| 79 | PC10 | `/microcontroller/~{MotorPwmU}` | **TIM8_CH1N (AF4)** |
| 80 | PC11 | `/microcontroller/~{MotorPwmV}` | **TIM8_CH2N (AF4)** |
| 81 | PC12 | `/microcontroller/~{MotorPwmW}` | **TIM8_CH3N (AF4)** |
| 82 | PD0 | `/can/CANRX` | **FDCAN1_RX (AF9)** → TCAN334G U11 RXD |
| 83 | PD1 | `/can/CANTX` | **FDCAN1_TX (AF9)** → TCAN334G U11 TXD |
| 84 | PD2 | `/can/SHDN` | GPIO → U11 SHDN (standby) |
| 85 | PD3 | `/can/STB` | GPIO → U11 STB (sleep/wake) |
| 86 | PD4 | `/microcontroller/RS-485_De` | GPIO → U12 DE (direct) and U12 ~RE via R39 (1 k) |
| 87 | PD5 | `/microcontroller/RS-485_Tx` | **USART2_TX (AF7)** → U12 DI |
| 88 | PD6 | `/microcontroller/RS-485_Rx` | **USART2_RX (AF7)** ← U12 RO |
| 89 | PD7 | `/microcontroller/UsbPdCfg2` | GPIO → CH224K CFG2 (pin 2) |
| 90 | PB3 | `/microcontroller/SWO` | SWO (dedicated) → A1.8 |
| 91 | PB4 | `/microcontroller/UsbCc2` | GPIO input ← U2 (ESD) ← J6 CC1 pad; UCPD1_CC2 pin |
| 92 | PB5 | `/microcontroller/UiLedsRgb` | GPIO → WS2812B data in (D11.DI); 41-LED chain, §8.7; also **SPI1_MOSI (AF5)** (internal SPI clock, no SCK GPIO needed) |
| 93 | PB6 | `/microcontroller/UsbCc1` | GPIO input ← U2 (ESD) ← J6 CC2 pad; UCPD1_CC1 pin |
| 94 | PB7 | `/microcontroller/I2cSda` | **I2C1_SDA (AF4)** → J15.4 |
| 95 | PB8 | `/microcontroller/I2cScl` | **I2C1_SCL (AF4)** → J15.3 (also BOOT0 pin!) |
| 96 | PB9 | `/microcontroller/UsbPdCfg1` | GPIO → CH224K CFG1 (pin 9); 1 k pull-up R42 to CH224K VDD |
| 97 | PE0 | (local) | **TIM20_CH4N (AF3)** — timer SCK source: drives R17 → SPI4_SCK (PE2) + J12.7; see §8.2 |
| 98 | PE1 | (local) | OE (enable) of buffer U9 (74LVC1G126) — data-out tri-state; **TIM20_CH4 (AF5)**; see §8.2 |
| 99 | VSS | GND | |
| 100 | VDD | filtered +3.3V | §1.1 |

The LQFP-100 pin map above is the official ST **DS12288 Rev 6** pinout
(Fig. 9). All alternate-function assignments used in this document (TIM8 AF4
incl. PC9 = TIM8_CH4, SPI2 AF5 + NSS AF6, SPI4 AF5, SPI1 PB5 MOSI AF5,
SAI1 PA10 AF14/AF12, FDCAN1 AF9, USART2/USART3 AF7, I2C1 AF4, TIM15 AF3,
UCPD1 CC pins, dedicated SWD pins) are per the datasheet alternate-function
table (Table 13).

---

## 4. Motor control (power stage)

Six PWMs, all on **TIM8 (advanced timer)**, **AF4**:

| Signal | Pin | Timer channel |
|---|---|---|
| MotorPwmU | PC6 | TIM8_CH1 |
| MotorPwmV | PC7 | TIM8_CH2 |
| MotorPwmW | PC8 | TIM8_CH3 |
| ~MotorPwmU | PC10 | TIM8_CH1N |
| ~MotorPwmV | PC11 | TIM8_CH2N |
| ~MotorPwmW | PC12 | TIM8_CH3N |

Firmware notes:
- TIM8 is an advanced timer (BRK/BRK2 break hardware present), but the break
  **inputs** are not brought out as dedicated safety pins on this board —
  e.g. TIM8_BKIN2 (PC9) is the DebugSync / TIM8_CH4 pin. The analog **brake
  command** (DAC1_OUT2/PA5, J10.15) is a separate output, not a timer break
  input.
- PWM goes to the external power stage through **J10** (18-pin, 2×9, 2.00 mm):
  11/12 = U, 7/8 = V, 3/4 = W (low/high sides interleaved with GND pins),
  15 = BrakeResistorCommand (analog, from DAC1_OUT2/PA5),
  16 = PowerStageSpare (PD13), 17 = PowerStageStbyReset (PD11), 18 = PowerStageEnable (PD12).
- **No mandatory power-up order** (designer note): `PowerStageStbyReset`
  (PD11), `PowerStageEnable` (PD12), `AuxPowerSupplyEnable` (PC14 → U6 EN)
  and the two encoder supply enables (PC15, PA15) **can all stay off until
  needed**. There is no hardware sequencer and no required order — firmware
  enables each rail/stage when the function is actually used.

## 5. Current / bus-voltage sensing (ADC)

All sense signals arrive from the power stage via **J11** (14-pin, 2×7) —
differential/current-sense inputs, with analog ground `GNDA` and a shared
`V_REF` (REF330) pin (J11.13).

The STM32G474 embeds **five** successive-approximation ADC instances,
**ADC1…ADC5** (DS12288 §3.18): each 12-bit, up to 4 Msps, with its own
sequencer, data register, DMA stream and trigger inputs. ADC1/ADC2 and
ADC3/ADC4 can additionally be coupled in dual mode (ADC1/ADC3 are the masters);
ADC5 is standalone (RM0440 §21).

**How simultaneous sampling works (RM0440):** a single ADC instance converts
its sequencer **sequentially** — every channel gets its own sampling window, so
two channels on the *same* ADC are only ever sampled offset in time (one
conversion cycle apart), never at the same instant. Channels are sampled
**simultaneously only when they are wired to different ADC instances** and those
instances are triggered together, so that each converts its assigned channel in
the same window. (The dual-mode “regular simultaneous” mode is the same idea:
master and slave convert two *different* channels in one shared window.)

Per-pin ADC instance / channel assignment (DS12288 pin table, LQFP-100):

| Signal | J11 pin | MCU pin | Available on (channel) |
|---|---|---|---|
| InputCurrentSensFiltered | 10 | PA0 | ADC1 / ADC2 — IN1 |
| CurrentSensFilteredW | 7 | PA1 | ADC1 / ADC2 — IN2 |
| CurrentSensFilteredV | 6 | PA2 | ADC1 — IN3 (only) |
| BrakingCurrentSensFiltered | 11 | PA3 | ADC1 — IN4 (only) |
| CurrentSensFilteredU | 3 | PB1 | ADC1 — IN12, ADC3 — IN1 |
| BusVoltageSensFiltered | 14 | PC3 | **→ OPAMP5 (voltage follower) → internal OPAMP5 out → ADC5**; direct ADC1 / ADC2 — IN9 as an alternative |

Firmware notes:
- 12-bit, VREF+ = REF330 (3.30 V, ±0.5 %). The ADC clock is taken from AHB
  (independent of the CPU); pick the prescaler per the datasheet tConv for the
  desired sampling time.
- **Phase currents (designer-confirmed assignment):** the three phase
  currents are placed on three *different* instances and started on the same
  trigger, so they are converted in one shared window — a true simultaneous
  U/V/W sample: **V → ADC1 (IN3)** (PA2 is on no other instance), **W →
  ADC2 (IN2)**, **U → ADC3 (IN1)** (PB1 is on ADC1/ADC3).
- **Bus voltage (PC3) → ADC5 via OPAMP5** (designer): the filtered sense on
  PC3 is buffered by an internal **OPAMP5 configured as a voltage follower**
  (the sense divider is high-impedance), and the OPAMP5 internal output is an
  ADC5 channel. Direct PC3 → ADC1/ADC2 (IN9) is a possible alternative but
  not the intended path.
- Input current (PA0) is on ADC1/ADC2 and braking current (PA3) only on ADC1.
- **Shared-channel caveat** (RM0440): a physical pin is connected to more than
  one instance (e.g. PB1 = ADC1_IN12 and ADC3_IN1). While one instance converts
  that pin, the others cannot use the same channel in the same window (except
  dual interleaved mode). Assign each pin to one instance.
- The “Filtered” suffix means the power-stage board provides the LC filter;
  there is no additional on-board RC on these nets.
- **Do not use** PA0/PA1/PA2/PA3/PB1/PC3 for anything else (no alternative on
  this board).

### Analog Hall (position) inputs

From **J16** (12-pin, 2×6), with GNDA and V_REF (J16.12).

| Signal | J16 pin | MCU pin | Available on (channel) |
|---|---|---|---|
| EncoderAnalogHallFiltered1 | 3 | PB12 | ADC1 — IN11, ADC4 — IN3 |
| EncoderAnalogHallFiltered2 | 6 | PB11 | ADC1 / ADC2 — IN14 |
| EncoderAnalogHallFiltered3 | 7 | PE14 | ADC4 — IN1 (only) |
| EncoderAnalogHallFiltered4 | 10 | PE13 | ADC3 — IN3 (only) |
| EncoderAnalogHallFiltered5 | 11 | PE12 | ADC3 / ADC4 / ADC5 — IN16 |

- **Design intent (designer-confirmed):** each of the five hall signals is
  assigned to its own ADC instance so a full hall set is a simultaneous
  multi-instance sample: **Hall 1 (PB12) → ADC1**, **Hall 2 (PB11) → ADC2**,
  **Hall 3 (PE14) → ADC4**, **Hall 4 (PE13) → ADC3**, **Hall 5 (PE12) →
  ADC5**. (ADC1/ADC2/ADC3 are shared with the phase-current channels above.)

### Digital Hall (index/position) inputs

| Signal | Pin | Options |
|---|---|---|
| EncoderDigitalHallU | PC0 | GPIO/EXTI0 (also ADC1/ADC2 IN6) |
| EncoderDigitalHallV | PC1 | GPIO/EXTI1 (also ADC1/ADC2 IN7) |
| EncoderDigitalHallW | PC2 | GPIO/EXTI2 (also ADC1/ADC2 IN8) |

From **J13** (6-pin, 2×3). Plain GPIO (level sampling) — no timer on PC0/1/2
except TIM1 CH1/2/3 (AF2), which is otherwise unused on this board; if
quadrature/index timing is ever needed, TIM1 is available on these pins.

---

## 6. Incremental (quadrature) encoder — differential receive

Transceivers `U17`/`U18` (SP3485EN, 3.3 V, 10 Mbps, ±12 V common-mode) used
**receive-only**:

| SP3485 pin | U17 (phase A) | U18 (phase B) |
|---|---|---|
| 1 RO | → `/enc_incremental/EncA` → **PF9** | → `/enc_incremental/EncB` → **PF10** |
| 2 ~RE | R44 (1 k) to GND — always on | R45 (1 k) to GND — always on |
| 3 DE | R46 (1 k) to GND — always off | R47 (1 k) to GND — always off |
| 4 DI | R48 (1 k) to GND (unconnected use) | R49 (1 k) to GND |
| 5 GND | GND | GND |
| 6 A | `/enc_incremental/EncA+` (J7.6) | `/enc_incremental/EncB+` (J7.4) |
| 7 B | `/enc_incremental/EncA-` (J7.5) | `/enc_incremental/EncB-` (J7.3) |
| 8 VCC | +3.3V | +3.3V |

- **Connector J7** (6-pin IDC 2×3, 2.54 mm): 1=GND, 2=SupplyOut, 3=EncB−,
  4=EncB+, 5=EncA−, 6=EncA+. (Note A/B polarity per pins 3…6 as listed.)
- Encoder supply: `/enc_incremental/SupplyOut` = **+5V rail → U16 (TPS22810)
  EN by PC15 → FB7 (220 Ω) → F2 (200 mA fuse) → J7.2**. Firmware can
  power-cycle the encoder by toggling PC15.
- Firmware options for A/B (PF9/PF10):
  - **TIM15 encoder mode** (CH1 = PF9 AF3, CH2 = PF10 AF3) — 16-bit, 4×
    quadrature.
  - EXTI9/EXTI10 bit-banging; or TIM5_CH4 (AF6) for A only.

## 7. SSI encoder interface (differential, via SP3485 transceivers)

| | U20 (clock **driver**) | U21 (data **receiver**) |
|---|---|---|
| 1 RO | — (not used) | → `/enc_ssi/Data` → JP4.3 |
| 2 ~RE | R54 (1 k) to **+3.3V** — receiver disabled | R51 (1 k) to **GND** — always on |
| 3 DE | R52 (1 k) to **+3.3V** — always driving | R53 (1 k) to GND — always off |
| 4 DI | ← `/enc_ssi/Clk` ← JP3.3 | — |
| 5 GND | GND | GND |
| 6 A | `/enc_ssi/ClkOut` → J8.4 | `/enc_ssi/DataIn` ← J8.6 |
| 7 B | `/enc_ssi/~{ClkOut}` → J8.3 | `/enc_ssi/~{DataIn}` ← J8.5 |
| 8 VCC | +3.3V | +3.3V |

The direction (driver/receiver) of each transceiver is **fixed by the board
bias resistors** (DE/RE tied high/low), not toggled per transfer: U20 always
drives the SSI clock, U21 always receives SSI data.

- **Connector J8** (6-pin): 1=GND, 2=SupplyOut, 3=~ClkOut, 4=ClkOut,
  5=~DataIn, 6=DataIn. ⚠️ J8 is **not in bom.csv/positions.csv** — not
  populated in the current production version.
- Clock source selection (3-pin jumper **JP3** + DNP **R19** 0 R):
  - JP3.1 = SCK of external SPI header J9.3
  - JP3.2 = PB13 (U10.52) and, if R19 fitted, also PB10
  - JP3.3 = U20 DI (SSI clock)
  - Bridge 1–2: expose SCK on J9.3. Bridge 2–3: route **PB13 (SPI2_SCK)** or
    **PB10 (GPIO, needs R19)** to the SSI clock driver.
- Data return (3-pin jumper **JP4**):
  - JP4.1 = J9.4 (MOSI pin of external SPI header)
  - JP4.2 = PB15 (U10.54)
  - JP4.3 = U21 RO (SSI data)
  - Bridge 2–3: SSI data → **PB15 (plain GPIO input)**; bridge 1–2: PB15 on J9.4.
- Encoder supply: `+5V → U19 (TPS22810) EN by PA15 → FB8 → F3 (200 mA) →
  J8.2` (`/enc_ssi/SupplyOut`). Firmware can power-cycle via PA15.
- Firmware SSI modes:
  1. **SPI2-driven** (with R19 DNP): SPI2 on PB13 (SCK, AF5); data in on PB15
     (GPIO, not MOSI in this mode).
  2. **Bit-banged** (R19 fitted, bridge 2–3): PB10 clock out, PB15 data in.

## 8. SPI / I2C / UART / CAN / RS-485 summary

### 8.1 External SPI2 header (J9, 6-pin)

J9: 1=GND, 2=power (3.3 V or 5 V via **JP2**: JP2.1=+3.3V, JP2.2=J9.2,
JP2.3=+5V), 3=SCK, 4=MOSI, 5=MISO (PB14), 6=CS (PD15).

Note the routing through the 3-pin jumpers (see §7 for SSI mode):
J9.3 (SCK) sits on jumper pad JP3.1 and reaches **PB13** (U10.52) when JP3
is bridged 1–2; J9.4 (MOSI) sits on JP4.1 and reaches **PB15** (U10.54) when
JP4 is bridged 1–2. Without the jumpers, SCK/MOSI on J9 are open.

- **SPI2**: SCK = PB13 **AF5**, MISO = PB14 **AF5**, MOSI = PB15 **AF5**,
  NSS = PD15 **AF6** (different AF number than the rest of SPI2!).
- J9 is also **not in bom.csv/positions.csv** (optional in this build).
- JP2/JP3/JP4 are solder jumpers, also not in positions.

### 8.2 Encoder SPI (SPI4, J12, 10-pin) — dedicated serial encoder

The encoder is an SPI **slave** (e.g. Infineon TLE5012B). The MCU (SPI4) is
configured as a **slave** too, and the SCK, CS and data-out enable are
generated in hardware by gated timer channels (TIM20/TIM3) — the
"dual-slave" mode. This is the intended operation mode.

| Signal | Pin | AF | Connection |
|---|---|---|---|
| SCK | PE2 | SPI4_SCK AF5 | timer **PE0** (TIM20_CH4N AF3) → R17 → PE2; also → J12.7 (encoder CLK) |
| CS | PE3 | SPI4_NSS AF5 | timer **PE4** (TIM3_CH3 AF2) → R18 → PE3; also → J12.6 (encoder CS) |
| data in | PE6 | SPI4_MOSI AF5 | J12.3 → PE6 (from encoder MISO) |
| data out | PE5 | SPI4_MISO AF5 | PE5 → U9 → J12.10 (to encoder MOSI); U9 enabled by PE1 |

**Data roles in SPI slave mode** — in a SPI slave, MISO is the *output* and
MOSI is the *input*, so the roles are reversed relative to a master:
- **PE5 (SPI4_MISO) = data OUT** to the encoder (through buffer U9).
- **PE6 (SPI4_MOSI) = data IN** from the encoder.

The STM32G4 SPI has **no MISO/MOSI swap**: RM0440 `SPIx_CR1` shows only the
`MSTR` bit selects master/slave, and the MISO/MOSI roles follow that function
(master: MOSI out / MISO in; slave: MOSI in / MISO out). The "reversed" data
roles here (out on MISO/PE5, in on MOSI/PE6) are therefore only possible with
the MCU as a **slave** — which is why this uses the dual-slave + timer mode
rather than a master mode.

**SCK/CS generation (timers):** the SCK, CS and buffer-enable pins are timer
channel outputs: PE0 = **TIM20_CH4N** (AF3), PE4 = **TIM3_CH3** (AF2), PE1
(buffer enable) = **TIM20_CH4** (AF5). So TIM20 (CH4/CH4N) generates the SCK
and the data-out enable, and TIM3 (CH3) generates the CS. The timer-driven
SCK/CS pass through R17/R18 to both the SPI4 slave (PE2/PE3) and the encoder
(J12.7/J12.6). The result is encoder readout with no CPU intervention,
synchronous with the PWM.

**U9 (74LVC1G126, SOT-353) buffer on the data-out path**
(pinout: 1=OE, 2=A (input), 3=GND, 4=Y (output), 5=VCC):
- U9.1 (OE) = **PE1** (TIM20_CH4 AF5 — buffer enable; also a plain GPIO)
- U9.2 (A, input) = **PE5** (SPI4_MISO = data out)
- U9.3 (GND) = GND
- U9.4 (Y, output) = **J12.10** (`/microcontroller/EncoderSpiSdo`)
- U9.5 (VCC) = +3.3V

The buffer makes the data-out (PE5) tri-state: when `PE1` is low the drive onto
`J12.10` is released (high-Z) so the encoder can drive the line.

**J12 data wiring (at the encoder):** board **MISO/PE5** (data out, via U9 →
J12.10) connects to the encoder's **MOSI** (slave data-in); board **MOSI/PE6**
(data in, J12.3) connects to the encoder's **MISO** (slave data-out). The two
data lines (J12.10 / J12.3) are separate on the board and can be **merged
externally** into a single bidirectional line (encoders with one bidirectional
data pin, e.g. TLE5012B) or kept as **two separate unidirectional lines**; in
either case SPI4 operates as a slave and the buffer releases the data-out line
for the encoder to drive.

**J12 pinout:** 1=GND, 2=+3.3V, 3=Sdi (data in, PE6), 4=GND, 5=GND, 6=Cs,
7=Clk, 8=GND, 9=GND, 10=Sdo (data out, PE5 via U9).

### 8.3 RS-485 port (U12, SP3485EN) — USART2

| SP3485 pin | Connection |
|---|---|
| 1 RO | → **PD6 = USART2_RX (AF7)** (`/RS-485_Rx`) |
| 2 ~RE | ← R39 (1 k) ← **PD4** (`/RS-485_De`) |
| 3 DE | ← **PD4** (direct) |
| 4 DI | ← **PD5 = USART2_TX (AF7)** (`/RS-485_Tx`) |
| 6 A / 7 B | `/RS-485/RS485_A` / `/RS-485/RS485_B` (to J4) |
| 8 VCC | +3.3V |

- One GPIO (PD4) does the half-duplex direction control: HIGH = transmit
  (DE=1, ~RE=0), LOW = receive (DE=0, ~RE=1 via 1 k).
- **USART2** on PD5/PD6 (AF7). Set the direction bit on TX-complete / before
  each transmit.
- Wire to connector **J4**: pin 6 = A (non-inverting), pin 4 = B (inverting).

### 8.4 CAN (FDCAN1, TCAN334G `U11`, J3)

TCAN334G pinout per TI SLLSEQ7F (SOT-23-8):

| TCAN334G pin | Signal | MCU |
|---|---|---|
| 1 TXD | `/can/CANTX` | **PD1 = FDCAN1_TX (AF9)** |
| 2 GND | GND | — |
| 3 VCC | +3.3V | — |
| 4 RXD | `/can/CANRX` | **PD0 = FDCAN1_RX (AF9)** |
| 5 SHDN | `/can/SHDN` | PD2 (GPIO; HIGH = shutdown, internal pull-down) |
| 6 CANL | `/can/CANLOW` | J3.6 |
| 7 CANH | `/can/CANHIGH` | J3.4 |
| 8 STB | `/can/STB` | PD3 (GPIO; HIGH = low-power standby, internal pull-down) |

- **J3** (6-pin IDC 2×3): 1=GNDPWR, 2=(anode of D4, see A4), 3=GNDPWR,
  4=CANH, 5=GNDPWR, 6=CANL.
- Optional **120 Ω termination**: `R38` (120 Ω 1 %) between CANH and CANL,
  enabled by jumper **JP5** (JP5.1=CANH, JP5.2→R38→CANL).
- TCAN334G (TI SLLSEQ7F): dominant-timeout protection, wake-up, ±12 V
  common-mode, up to 5 Mbps; STB = standby, SHDN = shutdown.
- J3.4/J3.6 CANH/CANL naming matches TCAN334G pins 7/6.

### 8.5 I2C (J15, 4-pin)

| J15 pin | Net | MCU |
|---|---|---|
| 1 | GND | — |
| 2 | +3.3V | — |
| 3 | I2cScl | **PB8 = I2C1_SCL (AF4)** (⚠️ PB8 is also **BOOT0** — keep it low at boot unless booting from RAM; no board pull-up!) |
| 4 | I2cSda | **PB7 = I2C1_SDA (AF4)** |

**No on-board (external) pull-ups** on SCL/SDA. I2C1 (not I2C4) is the
instance that covers both pins (see A5). **Decision (designer):** use the **MCU
internal pull-ups** for basic low-speed operation — external pull-ups are not
required.

### 8.6 USB 2.0 device port (J6, data port)

- MCU FS device: **PA11 = USB_DM, PA12 = USB_DP** (dedicated pins, no AF).
- Path: PA11/PA12 → `U3` (USBLC6-2P6 ESD) → J6 D+/D− (J6.A6/B6 = D+, A7/B7 = D−).
- CC lines: J6 CC1/CC2 → `U2` (USBLC6-2P6) → **PB6 (UsbCc1)** / **PB4
  (UsbCc2)** (GPIO inputs; these are also the UCPD1_CC1/CC2 pins of the
  G474, so the UCPD peripheral could be used if desired).
- **No CC Rd pulldowns on J6** → this port is a **data-only** USB device
  port (no PD). VBUS is not routed to the MCU; the CC pins (PB4/PB6) are the
  only connector-related signals available to the MCU.
- J6 VBUS (A4/B4/B9) → D3 (SS54) → `/PowerIn` (diode-OR into the input bus).
- USBLC6-2P6 pinout (ST DS4260, SOT-666): 1/6 = I/O1, 2 = GND, 3/4 = I/O2,
  5 = VBUS (VDD). On J6 both U2/U3 VDD = +3.3V.

### 8.7 WS2812B RGB LED chain (41 LEDs, `D11`…`D51`)

- Data: **PB5** (GPIO) → D11.DI. Single daisy chain, all VDD on
  `/SUPPLYRGBLEDS` (+3.5V via FB6 220 Ω), GND on pin 3, data out pin 1,
  data in pin 4. Per-LED 100 nF caps C51…C92.
- **SPI + DMA option (designer intent):** PB5 is also `SPI1_MOSI` (AF5). The
  chain can be driven from an SPI data output with DMA — SPI master mode
  generates the internal clock, so **no SCK/CS GPIO is needed**; the SCK rate
  is then chosen so the encoded bit width matches the WS2812B timing.
- Exact chain order (from PCB DO→DI net tracing):
  `D11 → D17 → D23 → D27 → D31 → D35 → D39 → D43 → D46 → D49 → D12 → D18 →
  D13 → D19 → D24 → D28 → D32 → D36 → D40 → D44 → D47 → D50 → D14 → D20 →
  D25 → D29 → D33 → D37 → D41 → D15 → D21 → D26 → D30 → D34 → D38 → D42 →
  D45 → D48 → D51 → D16 → D22`
- Chain tail: D22.DI-out → **J17.2** (J17 = 3-pin: 1=GND, 2=data,
  3=`/SUPPLYRGBLEDS`) — for extending the ring. J17 not in BOM/positions.
- R36/R37 (100 Ω) are series resistors at chain junctions (D13, D15 inputs).
- 41 pixels; the chain order above defines the pixel index for spatial mapping.
- **Layout note (designer):** the LEDs physically run in a **straight line**
  across the board (data passes through all of them, right-to-left in the
  layout photo); the designators are *not* spatially meaningful — only the net
  chain order above defines pixel index.

## 9. USB-PD power input (J5, CH224K `U15`)

`U15` = **CH224K** (WCH) USB-PD 3.0 / BC1.2 **sink** controller,
SSOP-10-1EP (C970725 @ LCSC). It negotiates *incoming* power from J5.
**CH224K has an internal CC pulldown (~4.7 kΩ to GND, measured in the field —
not the nominal 5.1 kΩ)**; the board adds no external Rd, consistent with
that. (Field note: fake CH224K parts exist and lack the pulldown — verify
part authenticity if J5 power negotiation misbehaves.)

Board wiring (KiCad symbol pin numbers):

| U15 pin | Function | Board net |
|---|---|---|
| 1 | VDD | `/usb_pd/VDD` (VBUS → R41 1 k → VDD, C95 10 µF/50 V) |
| 2 | CFG2 | `/microcontroller/UsbPdCfg2` → **MCU PD7** |
| 3 | CFG3 | `/microcontroller/UsbPdCfg3` → **MCU PA9** |
| 4 | DP | J5 D+ via U14 (USBLC6) |
| 5 | DM | J5 D− via U14 |
| 6 | CC2 | J5 CC1 via U13 (USBLC6) |
| 7 | CC1 | J5 CC2 via U13 |
| 8 | VBUS | VBUS → **R40 (10 k)** → U15.8 |
| 9 | CFG1 | `/microcontroller/UsbPdCfg1` → **MCU PB9** (1 k pull-up R42 to VDD) |
| 10 | PG | open-drain power-good, **not connected to MCU** (by design — see A3) |
| 11/EP | GND | GND |

**Power path (matches the WCH reference circuit, DS Fig. 6.2):** VBUS feeds
VDD through R41 (1 k) + C95 (10 µF) and the VBUS sense pin through R40
(10 k) — exactly the topology WCH shows for 5/9/12/15/20 V operation. The
chip’s internal regulator handles the VBUS-to-VDD derivation, so this is the
manufacturer-recommended design for high-voltage PD negotiation. The VBUS
sense pin (8) is high-voltage tolerant (VIOHV, −0.5…13.5 V).

### Voltage request — WCH truth table (CH224DS1 v1F, §5.2.2)

| CFG1 | CFG2 | CFG3 | Requested VBUS |
|---|---|---|---|
| 1 | — | — | 5 V |
| 0 | 0 | 0 | 9 V |
| 0 | 0 | 1 | 12 V |
| 0 | 1 | 1 | 15 V |
| 0 | 1 | 0 | 20 V |

Firmware mapping (board net → chip function):

| Chip input | U15 pin | Driven by MCU | Board net |
|---|---|---|---|
| CFG1 (analog) | 9 | **PB9** (pin 96) | `/UsbPdCfg1` (1 k pull-up R42 to CH224K VDD) |
| CFG2 (digital, max 3.7 V) | 2 | **PD7** (pin 89) | `/UsbPdCfg2` |
| CFG3 (digital, max 3.7 V) | 3 | **PA9** (pin 70) | `/UsbPdCfg3` |

Firmware notes:
- **Default before MCU boot = 5 V.** CFG1 is pulled **high** by R42
  (1 k → CH224K VDD, which rises as soon as VBUS is connected), so CFG1 = 1
  before the MCU starts and the chip requests 5 V regardless of the floating
  CFG2/CFG3 (ignored while CFG1 = 1). The datasheet's “20 V if CFG1 floats”
  warning applies to an *unbiased* CFG1 — it does **not** apply to this board:
  R42 is exactly the “configuration resistor on CFG1” the datasheet
  recommends. The three CFG GPIOs can be configured/driven at any point during
  boot; no 20 V transient has to be tolerated.
- CFG2/CFG3 inputs are max 3.7 V on the CH224K — the 3.3 V MCU GPIOs are fine.
- `/usb_pd/VDD` (CH224K VDD) rises as soon as VBUS is connected (R41 1 k);
  it is **not** a firmware-controlled rail.
- J5 VBUS (A4/A9/B4/B9) → D2 (SS54) → `/PowerIn` — this is one of the
  diode-OR inputs of the main power bus (5 V/9 V/12 V/15 V/20 V depending on
  negotiation).
- J5 also carries USB data to the CH224K (BC1.2/fast-charge protocols);
  **USB device traffic does not work on J5** (the MCU USB is on J6).

## 10. Power architecture

### 10.1 Input

- `/PowerIn` is a **diode-OR bus** of ~5 V sources (SS54 SMA Schottky, 40 V):
  - D1: anode = J1 (banana pad) / JP1.1, cathode = `/PowerIn`
  - D2: anode = J5 VBUS (USB-PD), cathode = `/PowerIn`
  - D3: anode = J6 VBUS (USB-C data port), cathode = `/PowerIn`
  - D4: anode = J3.2 (CAN connector pin 2), cathode = `/PowerIn` (⚠️ A4)
  - D5: anode = J4.2 (RS-485 connector pin 2), cathode = `/PowerIn` (⚠️ A4)
  - J1 / J3.2 / J4.2 are otherwise unconnected reserved external-input pads;
    J2 is a plain GND banana pad.
- **F1** = 2920 SMD fuse, 33 V **2 A** (BSMD2920-200-33V), in series:
  `/PowerIn` → F1 → `/PowerInFused` → **J14** (power stage connector pins 5/6).
- **J14** (6-pin, 2×3, 2.00 mm) — power stage supply selection (schematic
  note): the attached power-stage board may either (a) be powered *from*
  this board (bridge J14.5–J14.6 with pin 6 driven from the attached board
  side) or (b) power this board itself.
  J14: 1=GNDPWR, 2=GNDPWR, 3=PowerOut (→ attached board), 4=GNDPWR,
  5=`/PowerInFused`, 6=`/PowerInFused`.
- `/PowerInLogic` = {J14.3, FB1.2, FB2.2, R1.1} — i.e. directly from J14.3
  into ferrites FB1 (→ U6 VIN) and FB2 (→ U7 VIN). **R1 (22 k) is only the
  D6 indicator divider** (`/PowerInLogic` → R1 → D6 anode), not part of the
  supply path.
- **D6** (green, 0603) = “input present” LED: `/PowerInLogic` → R1 (22 k) →
  D6 → GND (dim, ~0.15 mA).
- **JP1** (2-pin jumper): pin 1 = D1 anode / J1, pin 2 = `/PowerIn` —
  service jumper (bridging it bypasses D1); not populated.

### 10.2 Buck 1: U6 (TPS62932, 2 A, 3.8–30 V in) → `+5V` net

- VIN = `/PowerInLogic` via FB1 (220 Ω); C2/C4 (10 µF/50 V) input caps.
- EN = **PC14 (`AuxPowerSupplyEnable`)** — MCU-controlled.
- FB divider: **R8 (120 k) from VOUT, R9 (12 k) to GND** → Vref 0.8 V
  (±1 %) → **VOUT = 0.8 × (1 + 120/12) = 8.8 V** ⚠️ **anomaly A2 — the net
  is named `+5V`**. R12 (120 k)/R13 (12 k) (both DNP) tap the same FB node:
  **intended use (designer): DAC trim (R14 → PA4) and a spare divider
  footprint (R12/R13)** in case the 5 V output is insufficient for an
  external peripheral. **No fixed load is currently planned for this rail and
  its purpose is undecided (designer note)** — it currently feeds U16/U19
  (encoder-switch VIN), J9 (via JP2) and the D7 LED, all conditional on the
  firmware enabling the rail (PC14). **Measure the actual voltage in hardware
  before relying on it.**
- SW → L1 (47 µH 1 A) → `+5V` (bulk C19/C21 10 µF/50 V, C18 100 nF).
- RT = R6 (0 R, **DNP**) → default switching frequency (RT open = 500 kHz per
  TPS62932 datasheet).
- SS = C10 (100 nF/50 V) (soft-start cap).
- SOT-583 pinout (TI SLUSEA4D, Fig. 7-1): 1=RT, 2=EN, 3=VIN, 4=GND, 5=SW,
  6=BST, 7=SS, 8=FB — matches the board (R6→U6.1 RT, PC14→U6.2 EN,
  FB1→U6.3 VIN, L1→U6.5 SW, C10→U6.7 SS, R8/R9→U6.8 FB).
- `+5V` feeds: U16/U19 (encoder supply switches, VIN), JP2.3 (J9 power
  option), R15 → D7 LED, R8/R12 (FB), C18/C19/C21/C23.

### 10.3 Buck 2: U7 (TPS62932) → `+3.5V` net

- VIN = `/PowerInLogic` via FB2; C3 (10 µF/50 V) + C9 (100 nF) input caps.
- EN = **R4 (22 k) to EN + R5 (10 k) to GND** → EN is pulled to ~0.77 × VIN
  → **U7 is always enabled when VIN > ~1.3 V** (not MCU-controlled).
- FB divider: R10 (33 k)/R11 (10 k) → **VOUT = 0.8 × 4.3 = 3.44 V ≈ 3.5 V** ✓
  (matches the `+3.5V` name).
- SW → L2 (47 µH) → `+3.5V` (C20 100 nF, C22/C24 22 µF/6.3 V).
- RT = R7 (0 R, **DNP**) → default 500 kHz.
- `+3.5V` feeds: U4/U5 LDO inputs (via FB3/FB4), FB6 → `/SUPPLYRGBLEDS`
  (WS2812 ring), R16 → D8 LED, R10 (FB).

### 10.4 LDOs: U4/U5 (TPS7A2033, 3.3 V, 300 mA, X2SON-4)

| | U4 | U5 |
|---|---|---|
| IN | `+3.5V` via FB3 | `+3.5V` via FB4 |
| EN | R2 (10 k) to IN (always on) | R3 (10 k) to IN (always on) |
| OUT | **`+3.3V`** | **`+3.3VA`** |
| Caps | C1 100 nF (+3.3V bulk) | C13/C14 100 nF |

Both are **always-on** (EN tied to IN); only PC14 gates U6.

### 10.5 Voltage reference

`U8` = **REF330** (SOT-23): IN = +3.3VA, OUT = **`V_REF`** (3.30 V ±0.5 %),
GND = GNDA. `V_REF` → U10 VREF+ (pin 36), J11.13, J16.12 (shared reference to
the power-stage sense amplifiers). Decoupled by C17/C28 (1 µF) + C30 (100 nF)
to GNDA.

### 10.6 Encoder supply switches (TPS22810, WSON-6-1EP)

| | U16 (incremental encoder) | U19 (SSI encoder) |
|---|---|---|
| 1 VOUT | C99 (100 nF), FB7 → F2 (200 mA) → J7.2 | C103 (100 nF), FB8 → F3 (200 mA) → J8.2 |
| 2 QOD | ← R43 (1 k) from VOUT (quick discharge) | ← R50 (1 k) from VOUT |
| 3 CT | C96 (100 nF) → inrush limiting | C100 (100 nF) |
| 4 GND | GND | GND |
| 5 EN | **PC15** | **PA15** |
| 6 VIN | +5V | +5V |
| EP | GND | GND |

### 10.7 Indicator LEDs

| LED | Color | Drive | Net function |
|---|---|---|---|
| D6 | green | `/PowerInLogic` → R1 22 k | input present (dim) |
| D7 | green | `+5V` → R15 2.7 k | +5V rail on |
| D8 | green | `+3.5V` → R16 1 k | +3.5V rail indicator |
| D9 | green | PD14 (`/UiLedGreen`) → R31 1 k | UI green LED |
| D10 | red | PD10 (`/UiLedRed`) → R32 1 k, active high | UI red LED |

### 10.8 Rail summary

| Net | Source | Typical | MCU-controlled? |
|---|---|---|---|
| `/PowerIn` | diode OR (J1/J5/J6/J3.2/J4.2) | 5…20 V | no |
| `/PowerInFused` | F1 (2 A, 33 V) | = /PowerIn | no |
| `/PowerInLogic` | J14.3 | = /PowerIn | no |
| `+5V` | U6 (TPS62932) | **8.8 V per divider (A2)** | yes (PC14) |
| `+3.5V` | U7 (TPS62932) | 3.44 V | no |
| `+3.3V` | U4 (TPS7A2033) | 3.3 V | no |
| `+3.3VA` | U5 (TPS7A2033) | 3.3 V | no |
| `V_REF` | U8 (REF330) | 3.30 V | no |
| `/usb_pd/VDD` | VBUS → R41 1 k | VBUS−Δ | no |
| `/SUPPLYRGBLEDS` | +3.5V → FB6 | ~3.5 V | no |
| encoder supplies | U16/U19 from +5V | = +5V | yes (PC15/PA15) |

**No mandatory power-up order** (designer note): with `/PowerInLogic`
present, U7 (+3.5V) and the LDOs (+3.3V/+3.3VA) come up by themselves (U7 EN
divider R4/R5; LDO ENs tied to IN); U6 (+5V) stays gated on PC14; the power
stage and the encoder supplies (PD11/PD12, PC15/PA15) can **all stay off
until the firmware enables them**.

Also: **`GND` and `GNDPWR` are net-tied** (`NT1`) — a single ground domain.

---

## 11. UI

- **Button SW2** → PC13 (`/UiButton`): 100 Ω pull-up R35 to +3.3V, 100 nF
  debounce C50 to GND. Active-low. (PC13 is also RTC_TAMP1 — fine.)
- **Reset SW1** → PG10 (NRST): 100 Ω pull-down R34 to GND, 100 nF C49 on NRST.
  Push → NRST low. (R34 = 100 Ω to GND means NRST is pulled low through 100 Ω
  when the button is pressed; pull-up is inside the MCU NRST.)
- **Red LED** D10: PD10, active high, 1 k.
- **Green LED** D9: PD14 via R31 (`/UiLedGreen`); D8 is the +3.5V rail LED (R16).
- **WS2812 ring**: PB5, 41 pixels, chain order in §8.7.
- **Fiber-optic link** `U1` (ZGY133/T9, Chau-Light, 660 nm AlGaInP, 3–5 V,
  TTL input, ~16 Mbps NRZ capable, DNP): pin 1 = GND, pin 2 = +3.3V,
  pin 3 = **PA10** (`SAI1_SD_A`, AF14; net "SPDIF"). Designer intent:
  debug-data stream to another board via SAI1.
- **Debug sync**: PC9 (`TIM8_CH4`, AF4; TP1). Designer intent: a short pulse
  from the center-aligned PWM timer used as a scope trigger, synced to the
  motor PWM. (PC9 also carries TIM8_BKIN2 / I2C3_SDA.)

---

## 12. Debug / programming

- **SWD**: PA13 (SWDIO, A1.4), PA14 (SWCLK, A1.6), **SWO** on PB3 (A1.8).
- **UART to ST-Link**: USART3, PD8 TX (A1.13), PD9 RX (A1.14). The ST-Link
  V3-MiniE VCP (USB virtual COM port) runs **736 bps – 16 Mbps** (UM2910
  §5.5.3).
- **A1** = ST-Link V3-MiniE connector (**STDC14, 2×7, 1.27 mm**, SAMTEC
  FTSH-107-01-L-DV-K-A per UM2910 Rev 5), **DNP in this build** (not in
  positions.csv). Signal pins per official STDC14 (UM2910 Table 4):
  4 = SWDIO, 6 = SWCLK, 8 = SWO, 12 = NRST, 13 = T_VCP_RX (receives MCU TX),
  14 = T_VCP_TX (drives MCU RX), 5/11 = GND. **Deviations from the official
  pinout:** pin 3 (`T_VCC`, target-voltage sense input) is wired to GND,
  pin 7 (GND) is unconnected, pin 11 (`GNDDETECT`) is wired to GND.
  Effect: the ST-Link's target-voltage detection is disabled (fine — this
  target is 3.3 V); SWD/SWO/VCP work normally.
- **Test points** (all populated): TP1 = PC9, TP2 = PE15, TP3 = PE9,
  TP4 = PC4, TP5 = PA5 (BrakeResistorCommand), TP6 = PA8 (MCO).
- **Net ties (no pads to place)**: NT1 bridges **GND ↔ GNDPWR** (single
  ground domain); NT2 bridges **GND ↔ AGND** (intentional ground bond).
- **Spare GPIOs**: PA6, PA7, PC5, PB0, PB2, PE7, PE8, PE10, PE11, PF2 (net
  “GPIO”, unconnected); PB10 (SSI clock option, needs R19). PE0/PE4/PE1 are
  **not spare** — they are the encoder-SPI timer SCK/CS sources and the buffer
  enable (see §8.2).

---

## 13. DNP / not-in-BOM / not-in-positions summary

**Not in positions.csv (not placed in this production build):**

| Ref | Part | Consequence |
|---|---|---|
| A1 | ST-Link V3-MiniE | no on-board debug header (program via external SWD) |
| U1 | ZGY133/T9 | no fiber-optic link |
| J8 | SSI encoder connector | SSI encoder not connectable in this build |
| J17 | WS2812 ring extension header | no ring extension |
| JP1–JP5 | jumpers (power bypass, rail select, SSI clk/data, CAN term) | fixed default routing; CAN termination off; SSI clock = PB13 only |
| R6, R7 | 0 R (TPS62932 RT) | both bucks run at default 500 kHz |
| R12, R13 | 120 k/12 k (U6 FB backup divider) | U6 set by R8/R9 only |
| R14 | 120 k (DAC trim to U6 FB) | AuxPowerSupplyAdjust (PA4/DAC) has no effect |
| R19 | 0 R (PB10 → JP3.2) | SSI clock source = PB13 only (no PB10 bit-bang option) |
| H1–H6 | mounting holes | (mechanical) |
| NT1, NT2 | net ties (NT1: GND↔GNDPWR, NT2: GND↔AGND) | (not in BOM at all) |

**In BOM but not in positions:** only A1 and U1 (everything else above is
simply absent from bom.csv — the BOM appears to be a filtered/partial
export; see A8).

---

## 14. Design anomalies & open questions (ask hardware)

| # | Item | Detail |
|---|---|---|
| **A1** | **Y1 pin 2 → +3.3V** (crystal) | KiCad symbol `Crystal_GND24`/YXC datasheet expect GND on pin 2; PCB ties it to +3.3V. **Known to the hardware designer; one board already reworked, the other to be reworked before bring-up.** |
| **A2** | **`+5V` rail computes to 8.8 V** | U6 FB divider R8/R9 = 120 k/12 k with 0.8 V ref → 8.8 V, but the net is named +5V and feeds 5 V-rated encoder supplies (via U16/U19) and the J9 power option (3.3 V/5 V). R12/R13 + R14 form a **spare/DAC-trim divider option** (designer: for the case the 5 V output is insufficient for an external peripheral; the rail is a general-purpose supply). **Measure the actual voltage in hardware before using +5V-fed peripherals.** |
| **A3** | CH224K `PG` unconnected | **OK by design (designer):** if J5 has no VBUS, `/usb_pd/VDD` (the CH224K supply) is 0 anyway — nothing downstream runs without VBUS, so a PG status line adds no information for firmware. |
| **A4** | Reserved diode-OR inputs on J3.2/J4.2/J1 | CAN connector pin 2 → D4, RS-485 connector pin 2 → D5, banana pad J1 → D1: diode-OR’d into `/PowerIn`, anode sides otherwise unconnected. **No hazard while unconnected** (diodes simply off); only a hazard if a signal were wrongly routed to a reserved pin. |
| **A5** | No external I2C pull-ups on J15; PB8 = BOOT0 | Pull-ups come from the attached device **or the MCU internal pull-ups** (designer: internal pull-ups for basic low-speed operation). Boot-time risk: an external I2C device **holding SCL low at power-on/reset** could be misread on PB8/BOOT0 and change the boot source (plain floating SCL is safe). BOOT0 handling is **not a priority for now** — can be dealt with in software. |
| **A6** | No HSE load caps | Crystal runs unloaded except its internal caps. **Designer: expected to work now; to be fixed in a later revision.** Part of the A1 cluster. |
| **A7** | Stale sub-sheets | The sub-sheet files `enc_ssi.kicad_sch` / `enc_incremental.kicad_sch` contain **older reference designators/net names**, and the note in `microcontroller.kicad_sch` (“CLK/CS can be driven by timers and SDO can be switched to high impedance”) predates the current pin plan. The PCB netlist is authoritative (this document). |
| **A8** | BOM incompleteness | J8, J9, J17, JP1–JP5, NT1/NT2, R6/R7, R12–R14, R19 are absent from bom.csv though present in schematic/PCB — production files need regeneration if these become populated. |

---

## 15. Connector quick reference

| Ref | Type | Pins (1-based) |
|---|---|---|
| J1 | banana pad | 1 = D1 anode (external input 1) |
| J2 | banana pad | 1 = GNDPWR |
| J3 | IDC 2×3 | 1 GND, 2 ext-in (D4 anode), 3 GND, 4 CANH, 5 GND, 6 CANL |
| J4 | IDC 2×3 | 1 GND, 2 ext-in (D5 anode), 3 GND, 4 RS485_B, 5 GND, 6 RS485_A — the physical RS-485 connector |
| J5 | USB-C (GT-USB-7010ASV) | PD/power port: A4/A9/B4/B9 VBUS, A5 CC1, B5 CC2, A6/B6 D+, A7/B7 D−, A1/A12/B1/B12 GNDPWR, S1 GND |
| J6 | USB-C (same footprint) | data port: same pin map; VBUS → D3, CC1/CC2 → U2 → PB4/PB6, D+/D− → U3 → PA12/PA11 |
| J7 | IDC 2×3 | incremental encoder: 1 GND, 2 VCC, 3 B−, 4 B+, 5 A−, 6 A+ |
| J8 | IDC 2×3 | SSI encoder: 1 GND, 2 VCC, 3 ~CLK, 4 CLK, 5 ~DATA, 6 DATA (not in BOM) |
| J9 | header 6 | SPI2: 1 GND, 2 VCC (JP2-selectable 3.3/5 V), 3 SCK, 4 MOSI, 5 MISO, 6 CS (not in BOM) |
| J10 | 2×9 2.0 mm | power stage PWM/ctrl: 1 GND, 2 +3.3V, 3 W, 4 ~W, 5 GND, 6 GND, 7 V, 8 ~V, 9 GND, 10 GND, 11 U, 12 ~U, 13 GND, 14 GND, 15 BrakeCmd (DAC), 16 spare, 17 STBY/RESET, 18 EN |
| J11 | 2×7 2.0 mm | analog sense: 1 GNDA, 2 +3.3VA, 3 ISensU, 4 GNDA, 5 GNDA, 6 ISensV, 7 ISensW, 8 GNDA, 9 GNDA, 10 ISensIn, 11 IBrake, 12 GNDA, 13 V_REF, 14 BusV |
| J12 | 2×5 2.0 mm | encoder SPI: 1 GND, 2 +3.3V, 3 Sdi, 4 GND, 5 GND, 6 Cs, 7 Clk, 8 GND, 9 GND, 10 Sdo |
| J13 | 2×3 2.0 mm | digital Hall: 1 GND, 2 +3.3V, 3 W, 4 V, 5 U, 6 GND |
| J14 | 2×3 2.0 mm | power stage supply: 1 GNDPWR, 2 GNDPWR, 3 PowerOut, 4 GNDPWR, 5/6 PowerIn (bridge per note) |
| J15 | 2×2 2.0 mm | I2C: 1 GND, 2 +3.3V, 3 SCL, 4 SDA |
| J16 | 2×6 2.0 mm | analog Hall: 1 GNDA, 2 +3.3VA, 3 Hall1, 4 GNDA, 5 GNDA, 6 Hall2, 7 Hall3, 8 GNDA, 9 GNDA, 10 Hall4, 11 Hall5, 12 V_REF |
| J17 | 3-pin | WS2812 out: 1 GND, 2 data (tail of D22), 3 +3.5V (not in BOM) |
| A1 | ST-Link V3-MiniE, STDC14 2×7 | 4 SWDIO, 6 SWCLK, 8 SWO, 12 NRST, 13 T_VCP_RX (MCU TX), 14 T_VCP_TX (MCU RX), 5/11 GND; pin 3 T_VCC→GND, pin 7 unconnected (DNP) |
| JP1 | 2-pin | 1 = D1 anode/J1, 2 = /PowerIn (service jumper) |
| JP2 | 3-pin | 1 +3.3V, 2 J9.2, 3 +5V |
| JP3 | 3-pin | 1 SCK(J9.3), 2 PB13(+PB10 if R19), 3 SSI CLK |
| JP4 | 3-pin | 1 MOSI(J9.4), 2 PB15, 3 SSI DATA |
| JP5 | 2-pin | 1 CANH, 2 → R38 120 Ω → CANL (termination enable) |

---

## 16. Datasheets & sources

### Manufacturer datasheets (retrieved)

| Part | Source | Location |
|---|---|---|
| TPS62932 (TI) | ti.com, rev D | `/tmp/pi-web-pdf/tps6293x-38-v-to-30-v-2-a-3-a-synchronous-buck-converters-in-a-sot583-package-datasheet-rev-d.md` |
| TPS22810 (TI) | ti.com, rev C | `/tmp/pi-web-pdf/tps22810-27-18-v-79-m-on-resistance-load-switch-with-thermal-protection-datasheet-rev-c.md` |
| TCAN334G (TI) | ti.com SLLSEQ7F (rev May 2025) | `_tmp_schematic/datasheets/tcan334g.pdf` |
| SP3485EN (MaxLinear) | maxlinear.com | `_tmp_schematic/datasheets/sp3485.pdf` (+ `.txt`) |
| YXC YSX211SL / XL7KI crystal | YXC (xonstorage mirror) | `_tmp_schematic/datasheets/ysx211sl.pdf` |
| STM32G474VE (ST) | official: DS12288 Rev 6 (datasheet), RM0440 (reference manual) | `_tmp_schematic/datasheets/DS_stm32g474ve.pdf`, `_tmp_schematic/datasheets/RM0440*/` |
| ST-Link V3-MiniE (ST) | official: UM2910 Rev 5 (user manual), DB Stlink-V3MINIE (design doc) | `_tmp_schematic/datasheets/um2910-*.pdf`, `_tmp_schematic/datasheets/DB_stlink-v3minie-1.pdf` |

### Non-manufacturer / mirrored sources (use with the stated caveat)

| Part | Source | Caveat |
|---|---|---|
| **CH224K (WCH)** | `CH224DS1.PDF` v1F from `strawberry-linux.com/pub/` and `components101.com/.../WCH_CH224K_ENG.pdf` (`_tmp_schematic/datasheets/ch224_ds1.pdf`/`.txt`, `/tmp/pi-web-pdf/wch-ch224k-eng.md`) | **Not the official WCH download** (wch.cn blocked/JS-gated from this network). Truth table, pin functions and the Fig. 6.2 reference circuit read from the mirrored PDF. **Pin functions (CFG1 = pin 9, CFG2 = pin 2, CFG3 = pin 3; CC1/CC2 per the symbol) confirmed against the physical part.** |
| ZGY133/T9 (Chau-Light) | LCSC page C277781 + Chau-Light product PDF (`xonstorage.../3509816_ZGY133T9PLT_link.pdf`) | LCSC PDF extraction failed (invalid structure); identity confirmed via product data sheet: 660 nm LED + driver, 3–5 V, TTL, 1 mm plastic fiber, 16 Mbps NRZ. |
| USBLC6-2 (ST) | ST DS4260 (unreachable directly; pin table from reference copies) | Pin 1/6 I/O1, 2 GND, 3/4 I/O2, 5 VBUS — matches board usage. |
| STM32G474 AF/ADC | covered by the official DS12288 Rev 6 (Table 13 AF, pin-definition ADC channels) + RM0440; CubeMX-derived cross-checks (STM32duino `PeripheralPins.c`, GAW pinout page) kept as secondary sources | All mappings in this document: TIM8 PC6/7/8/10/11/12 AF4 (PC9 = TIM8_CH4), SPI2 PB13/14/15 AF5 + PD15 AF6, SPI4 PE2/3/5/6 AF5, SPI1 PB5 MOSI AF5, SAI1 PA10 (SAI1_SD_A AF14 / SAI1_D1 AF12), FDCAN1 PD0/1 AF9, USART2 PD5/6 AF7, USART3 PD8/9 AF7, I2C1 PB7/8 AF4, TIM15 PF9/10 AF3 (incremental encoder), TIM20 PE0 (CH4N) / PE1 (CH4) + TIM3 PE4 (CH3) for encoder SPI SCK/CS/enable, UCPD1 CC on PB4/PB6, SWD on PA13/PA14/PB3, ADC channels per §5. |

### Field notes

- CH224K internal CC pulldown ≈ 4.7 kΩ (measured on a working part; fake
  parts lack it). The drivn board relies on this internal pulldown (no
  external Rd) — consistent.

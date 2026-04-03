# esphome-bspool-chlorinator

ESPHome external component for **BSPool VA Salt (Smart)** salt-water chlorinator. Communicates over UART (RS232-TTL) using a proprietary 3-byte request/response protocol.

## Quick Start

```bash
# Build test (ESP8266, Arduino framework)
# Requires ESPHome CLI installed
esphome compile tests/components/bs_pool/test.esp8266-ard.yaml

# Usage: add as external component in your ESPHome YAML
# See example.yaml for full configuration
```

## Architecture

```
components/bs_pool/                # All source code lives here
├── __init__.py + bs_pool.{h,cpp}  # Core: BSPool hub, BSPoolListener, DataPacket, FunctionCode
├── sensor/                        # 8 measurement sensors (incl. hours of operation)
├── binary_sensor/                 # 21 status/alarm/warning binary sensors
├── switch/                        # 9 switches (8 user settings + stop/start)
├── select/                        # 3 selects (control mode, language, relay status)
├── number/                        # 10 writable numbers (power, volume, pH/ORP targets, relay, programs)
└── text_sensor/                   # 3 text sensors (time, version, chlorinator type)

tests/components/bs_pool/          # Build tests (ESP8266)
PCB/                               # Hardware: schematics, BOM, wiring (read-only reference)
```

### Core Pattern

**Listener-based polling**: `BSPool` (PollingComponent + UARTDevice) polls the chlorinator every 60s. Each sub-component registers as a `BSPoolListener` and declares which `FunctionCode`s it needs via `codes_to_poll()`. Responses are dispatched to all listeners via `handle_message(DataPacket&)`.

### Sub-component Structure (repeated 6×)

Every entity type (sensor, binary_sensor, switch, select, number, text_sensor) follows the same pattern:

| File | Purpose |
|------|---------|
| `__init__.py` | ESPHome config schema + `to_code()` codegen |
| `bs_pool_X.h` | C++ class: BSPoolListener + Component, SUB_* macros |
| `bs_pool_X.cpp` | `codes_to_poll()` + `handle_message()` with switch on function_code |

Switch, select, and number additionally have child classes for write-back to the UART bus:
- `UserSettingsSwitch`, `StopStartSwitch` — via `Parented<BSPoolSwitch>`
- `ControlModeSelect`, `LanguageSelect`, `RelayStatusSelect` — via `Parented<BSPoolSelect>`
- `WritableNumber` — generic configurable number via `Parented<BSPoolNumberComponent>` with function_code, scale, 16-bit flag, and program-time mode

## Protocol

- **Baud**: 19200
- **Request**: `{'?', function_code, '\4'}` (3 bytes)
- **Response**: `{function_code, data_b2, data_b3}` (3 bytes)
- **DataPacket**: union of `{function_code, data_b2, data_b3}` and raw `uint8_t data[3]`

### Function Codes

| Code | Name | Usage |
|------|------|-------|
| `'r'` | USER | User settings bitfield (R/W) |
| `'S'` | STOP_START | Stop/Start chlorinator (R/W) |
| `'m'` | CONTROL_MODE | Manual/Auto/Semi-auto (R/W) |
| `'L'` | LANGUAGE | Language selection (R/W) |
| `'R'` | RELAY_STATUS | Relay mode (R/W) |
| `'A'` | ALARMS | Alarm bitmask (R) |
| `'w'` | WARNINGS | Warning bitmask (R) |
| `'s'` | STATUS | Status bitmask (R) |
| `'c'`/`'C'`/`'V'`/`'p'`/`'N'`/`'o'`/`'W'` | Measurements | Various sensor values (R) |
| `'F'`/`'G'` | HOURS_LOW/HIGH | Hours of operation (R) |
| `'H'` | TIME | Clock (R) |
| `'y'` | VERSION | Firmware version (R) |
| `'Z'` | CHLORINATOR_TYPE | Device type/size (R) |
| `'T'` | POWER | Power 0–100% (R/W) |
| `'v'` | POOL_VOLUME | Pool volume m³ (R/W) |
| `'b'` | CLEANING_CYCLE | Cleaning cycle delay (R/W) |
| `'P'` | PH_TARGET | pH target ×100 (R/W) |
| `'O'` | ORP_TARGET | ORP target mV (R/W) |
| `'D'` | RELAY_DELAY | Relay delay minutes (R/W) |
| 201–204 | PROGRAM_1/2_START/STOP | Relay program times (R/W) |

## Conventions

- **Language**: C++ (Arduino/ESP8266), Python (ESPHome codegen)
- **Commits**: Conventional Commits in English (`feat:`, `fix:`, `chore:`)
- **Branch**: `main`
- **No CI/CD, no linter, no formatter** — early-stage project
- **Hardware target**: ESP8266 NodeMCU + RS232-TTL converter + DC-DC 5V step-down
- **Test**: Build-only test via `esphome compile` (no unit tests)

## Key Gotchas

- **Sensor scaling**: cell_current/voltage → `/10.0f`, pH/salt → `/100.0f`, radox → raw mV
- **Invalid values**: pH and salt report `0xFFFF` when unavailable → publish `NAN`
- **Temperature sign**: `data_b3` flag controls sign of temperature
- **Inverted switch bits**: `is_outdoor` (bit0), `ph_alarm` (bit4), `ph_control` (bit6) use inverted logic
- **Stop/Start polarity**: protocol 0=Start, 1=Stop; switch ON=running, OFF=stopped
- **Hours of operation**: requires two polls (`'F'` + `'G'`), combined in sensor handler
- **WritableNumber**: generic class configured via `set_function_code()`, `set_is_16bit()`, `set_scale()`, `set_is_program_time()` from Python codegen
- **Program times**: stored as minutes since midnight (0–1439), converted to hours+minutes on write
- **FunctionCode enum**: uses `uint8_t` base (not `char`) to support numeric codes 201–204
- **TODO**: `bs_pool_switch.cpp:34` — unverified bit for `cover_switch_off`

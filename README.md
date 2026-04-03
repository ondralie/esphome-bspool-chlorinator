# ESPHome BSPool Chlorinator

ESPHome external component for the BSPool VA Salt (Smart) salt-water chlorinator.

## What is this

An ESPHome external component that connects to the BSPool VA Salt (Smart) chlorinator over RS232 (UART) using its proprietary 3-byte request/response protocol. It exposes sensors, binary sensors, switches, selects, numbers, and text sensors directly to Home Assistant without any custom firmware flashing or cloud dependency.

## Supported entities

**8 sensors**
- Cell current, cell intensity, cell voltage, pH, salt concentration, ORP (redox), temperature, hours of operation

**21 binary sensors**
- 7 alarms: ORP probe saturated, overtemperature, open circuit, water flow, short circuit, P954 unit stopped, communication error to P954
- 6 warnings: low salt, high salt, pH pump overrun, acid, clock set, probe disconnected
- 8 status flags: output voltage polarity, filter pump running, chlorination in process, stopped by remote, ORP sensor stable, salt sensor connected, electrode cleaning, super-chlorination

**9 switches** (user settings + stop/start)
- Outdoor, cover switch off, flow switch installed, ORP displayed, pH alarm, pH corrector alkaline, pH control, cover installed
- Chlorinator start/stop

**3 selects**
- Control mode: Manual / Automatic / Semi-automatic
- Language: English, Spanish, Catalan, French, Italian, Dutch, Portuguese, Turkish, Hebrew, German, Czech
- Relay status: Off / On / Timed / 1 cycle per 24h / 2 cycles per 24h

**10 numbers** (writable settings)
- Power (0–100%), pool volume (m³), cleaning cycle (minutes), pH target, ORP target (mV)
- Relay delay (minutes), relay program 1/2 start/stop times

**3 text sensors**
- Device time, firmware version, chlorinator type

## Installation

Add the component as an external source in your ESPHome YAML:

```yaml
external_components:
  - source: github://ondralie/esphome-bspool-chlorinator
    components: [ bs_pool ]
```

## Configuration

Minimal working configuration for ESP8266:

```yaml
external_components:
  - source: github://ondralie/esphome-bspool-chlorinator
    components: [ bs_pool ]

uart:
  tx_pin: GPIO1
  rx_pin: GPIO3
  baud_rate: 19200

bs_pool:
  update_interval: 60s

sensor:
  - platform: bs_pool
    temperature:
      name: "Pool Temperature"
```

See [example.yaml](example.yaml) for the full configuration with all available entities.

## Hardware

- ESP8266 NodeMCU V3
- RS232-to-TTL converter module
- DC-DC step-down converter (set to 5V)

You can wire the components together on a breadboard or any other way you prefer. A simple custom PCB that connects all the parts is also available in the [`PCB/`](PCB/) directory.

```
                          ┌──────────────┐
  Chlorinator             │  RS232-TTL   │          ┌──────────────┐
  ┌─────────┐   RS232     │  Converter   │   UART   │   ESP8266    │
  │         ├────────────►│              │          │   NodeMCU    │
  │  BSPool │  Left ───── │ RX       TX ─┼─────────►│ GPIO3 (RX)   │
  │ VA Salt │  Middle ─── │ TX       RX ─┼◄─────────│ GPIO1 (TX)   │
  │         │  Right ──── │ GND     GND ─┼──────────│ GND          │
  └────┬────┘             │         VCC ─┼──┐       └──────┬───────┘
       │                  └──────────────┘  │              │
       │ 12V                                │ 5V           │ 5V
       │              ┌──────────────┐      │              │
       └─────────────►│   DC-DC      ├──────┴──────────────┘
                      │  Step-Down   │
                      │  (set 5V!)   │
                      └──────────────┘
```

The full schematic is available in [`PCB/`](PCB/).

## Protocol documentation

See [Serial Protocol Documentation](docs/serial-protocol.md) for the full communication protocol reference.

## Disclaimer

This project is provided as-is, without any warranty. It has been tested on a single device only. Use at your own risk. The author assumes no responsibility for any damage to your equipment, pool, or anything else resulting from using this component.

## License

GPL-3.0 -- see [LICENSE](LICENSE)

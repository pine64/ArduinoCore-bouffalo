# Arduino Core for Bouffalo's RISC-V MCUs
[Discord](https://discord.gg/89VWQVH) | [Matrix](https://matrix.to/#/#pine64-nutcracker:matrix.org) | [Telegram](https://t.me/joinchat/Kmi2S0nOsT240emHk-aO6g)

This is community made Arduino Core for Bouffalo Labs RISC-V MCUs. The target is to make this core compatible with AVR and ESP32/ESP8266 Arduino Core for easy migrating of existing projects.
# Supported MCUs
- [X] BL602
- [ ] BL604

# How To Install
In Arduino IDE, go to `File > Preferences`, and add to  `Additional Boards Manager URLs` following:
`https://nutcracker.pine64.org/arduino/package_bouffalo_index.json`. Then open `Tools > Board > Boards Manager`, search for `Arduino for Bouffalo Labs RISC-V MCUs` and install it. (Also, then don't forget to pick your BLXXX based board)

# Disclaimer
Some parts of this core are based or inspired by work of Arduino, Espressif and Bouffalo Lab teams and their communities.

# FAQ
## Is this part of Nutcracker challenge?
Partly. This effort is for me ([gamiee](https://github.com/gamelaster)) a way how to explore Bouffalo SDK and the MCU itself. So it will help a lot later when reversing the blobs.

## Can I use this for testing BL IOT SDK?
At the moment, yes. Most of the components are precompiled to static libraries and all headers are available. (Although, the WiFi component isn't compiled (yet))

## Supported operating systems
Windows and Linux are tested and working fine. MacOS should work fine too, but it's not tested.

## Supported peripherals
You can find supported peripherals in [this project table](https://github.com/pine64/ArduinoCore-bouffalo/projects/1).

## Supported libraries
You can find supported libraries in [this project table](https://github.com/pine64/ArduinoCore-bouffalo/projects/2).

## Other to-do's
You can find them in [this project table](https://github.com/pine64/ArduinoCore-bouffalo/projects/3).


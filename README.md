# AirGradient MQTT for Arduino

This is an Arduino sketch for the AirGradient DIY Air Quality Sensor Pro, PCB version 4.2. 

The base configuration for an AirGradient product is for it to log data to AirGradient's servers. This sketch replaces that functionality with MQTT capabilities to log data to a server of your choice.

## Getting Started and uploading this sketch

1. Download and Install the [Arduino IDE](https://www.arduino.cc/en/software), [Arduino CLI](https://arduino.github.io/arduino-cli/0.19/installation/), or [VS Code](https://code.visualstudio.com/) with the [Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) to your system.
1. Clone this repo or download the latest release (a zip of the source code).
1. Open `airgradient_mqtt.ino` with your chosen Arduino tools.
1. Install all required Arduino reference libraries.
1. Plug in the microcontroller for your AirGradient (such as an ESP8266 Wemos LOLIN D1 mini) and upload the sketch to the controller.

## Running tests

This project leverages [AUnit](https://github.com/bxparks/AUnit) to facilitate building automated tests, and [EpoxyDuino](https://github.com/bxparks/EpoxyDuino) to mock an Arduino on a PC and run continuous integration.

I ran tests using Windows Subsystem for Linux (WSL). Follow these steps to get started - you may be able to skip several steps on a standard Linux distro or macOS.

1. Install the AUnit library within the Arduino IDE.
1. [Setup and configure WSL](https://learn.microsoft.com/en-us/windows/wsl/install).
    1. At the time of this running, the provided Ubuntu distro requires an `apt update` before proceeding.
    1. EpoxyDuino requires a C++ compiler and make. Add them to your install with `apt install g++` and `apt install make`.
1. Clone the [EpoxyDuino](https://github.com/bxparks/EpoxyDuino) repo and place it within the `user/Documents/Arduino/libraries` folder (this is not a required location, but this project is setup assuming EpoxyDuino is located here).
1. Add a `.env` file to the root of this project with the following format:

```bash
arduino_ide_dir=LOCATION_OF_ARDUINO_IDE
```

You are now ready to build and compile and run the tests. Follow these instructions to run all tests from the root of this project. I prefer to use the WSL terminal within VS Code for this purpose.

```bash
$ make -C tests clean

$ make -C tests tests
$ make -C tests runtests | grep failed
```

Alternatively, you can browse to the subfolder containing a specific subset of tests and run those directly.

```bash
$ make clean

$ make
$ make run
```

## Impementation Details - RTTI

For this sketch I tried to implement and apply Object Oriented Programming techniques were it made sense. I had thought I might reuse the State Machine concept, and built `MachineBase` and `StateBase` to be extendable.

The different `ConfigStateMachine` states like `ClearState` implement `StateBase` and its references to `MachineBase`, but require awareness of the `ConfigStateMachine` in order to write to the OLED display. To do this, `ClearState` requires Run-time type information (RTTI) so that it can `dynamic_cast<>` a `MachineBase` to `ConfigStateMachine`.

Many embedded systems do not support RTTI, and for this reason the Arduino compiler disables it by default. An ESP8266 **can** support RTTI, but to provide Arduino support, the ESP Arduino core implementation avoids its use and implements `TypeConversionFunctions` combined with Enums. Applied to our case, `MachineBase` gets a `MachineType` Enum. Other objects and classes can read the Enum and determine what type the `MachineBase` implementation it is.

I prefer use of `dynamic_cast<>` because I feel it helps satisfy SOLID. TODO - finish this statement.

This sketch includes both an implementation that avoids use of RTTI and one that leverages it. TODO - get the `#if` directives working properly to easily switch.

A file called `platform.txt` contains the compiler flags for a particular board. On Windows I found mine at this path: `C:\Users\USERNAME\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.1.2\platform.txt`.

I replaced the original line:

```
compiler.cpp.flags=-c "{compiler.warning_flags}-cppflags" {build.stacksmash_flags} -Os -g -free -fipa-pta -Werror=return-type -mlongcalls -mtext-section-literals -fno-rtti -falign-functions=4 {build.stdcpp_level} -MMD -ffunction-sections -fdata-sections {build.exception_flags} {build.sslflags} {build.mmuflags} {build.non32xferflags}
```

With this alternative that swapps the `fno-rtti` flag for `-frtti`:

```
compiler.cpp.flags=-c "{compiler.warning_flags}-cppflags" {build.stacksmash_flags} -Os -g -free -fipa-pta -Werror=return-type -mlongcalls -mtext-section-literals -frtti -falign-functions=4 {build.stdcpp_level} -MMD -ffunction-sections -fdata-sections {build.exception_flags} {build.sslflags} {build.mmuflags} {build.non32xferflags}
```

This enables RTTI with the ESP8266 and facilitates using `dynamic_cast<>`.

## Integrating with Home Assistant

My goal behind adding MQTT to the AirGradient was to integrate with Home Assistant and use HA as an MQTT Broker. [Follow these instructions to configure Home Assistant in this way](https://www.home-assistant.io/integrations/mqtt).

### General HA setup (todo)

1. create a user for mqtt
1. setup the broker. configure it for the user
1. point the publisher or subscriber to the broker. has to be able to call mqtt:// instead of http:// and access the port
1. add sensors to the yaml in home assistant to receive the data based on the topic
1. update the UI to show those sensors

### Creating sensors within Home Assistant

I use Home Assistant to track and display the sensor status. Define the sensors in your `configuration.yaml` as follows:

```yaml
# Air Gradient sensors
mqtt:
  - sensor:
      name: "Room-name Temperature"
      state_topic: "home/roomname/temperature"
      unit_of_measurement: "°F"
  - sensor:
      name: "Room-name Humidity"
      state_topic: "home/roomname/humidity"
      unit_of_measurement: "%"
  - sensor:
      name: "Room-name NOX Index"
      state_topic: "home/roomname/nox_index"
  - sensor:
      name: "Room-name TVOC Index"
      state_topic: "home/roomname/tvoc_index"
  - sensor:
      name: "Room-name PM 1"
      state_topic: "home/roomname/pm01"
      unit_of_measurement: "ug/m3"
  - sensor:
      name: "Room-name PM 2.5"
      state_topic: "home/roomname/pm25"
      unit_of_measurement: "ug/m3"
  - sensor:
      name: "Room-name AQI"
      state_topic: "home/roomname/pm25AQI"
  - sensor:
      name: "Room-name PM 0.3"
      state_topic: "home/roomname/pm03"
      unit_of_measurement: "ug/m3"
  - sensor:
      name: "Room-name PM 10"
      state_topic: "home/roomname/pm10"
      unit_of_measurement: "ug/m3"
  - sensor:
      name: "Room-name CO2"
      state_topic: "home/roomname/co2"
      unit_of_measurement: "ppm"
```

## References

AirGradient provides several resources for DIY support and customization.

- [DIY Pro 4.2 build instructions](https://www.airgradient.com/open-airgradient/instructions/diy-pro-v42/)
- [DIY Pro Presoldered 4.2 build instructions](https://www.airgradient.com/open-airgradient/instructions/diy-pro-presoldered-v42/)
- [AirGradient PCB and 3D Printer files](https://www.airgradient.com/open-airgradient/instructions/overview/)
- [AirGradient Example 4.2 sketch](https://github.com/airgradienthq/arduino/blob/master/examples/DIY_PRO_V4_2/DIY_PRO_V4_2.ino)

There are several forks of the original [AirGradient arduino repo](https://github.com/airgradienthq/arduino/) that add alternative capabilities that I used for reference.

- [JHadley1406 / aigradient_arduino](https://github.com/JHadley1406/airgradient_arduino)
- [hjburke / airgradient-mqtt](https://github.com/hjburke/airgradient-mqtt)
- [nsbk / airgradient_mqtt](https://github.com/nsbk/airgradient_mqtt)
- [agileek / AirGradientHomeAssistant](https://github.com/agileek/AirGradientHomeAssistant)
- [geerlingguy / airgradient-prometheus](https://github.com/geerlingguy/airgradient-prometheus)
- [Jeff Geerling - Monitoring my home's air quaity (CO2, PM2.5, Temp/Humidity) with AirGradient's DIY sensor](https://www.jeffgeerling.com/blog/2021/airgradient-diy-air-quality-monitor-co2-pm25)

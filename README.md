# AirGradient MQTT

Simple script that reads AirGradient air quality sensors and publishes data to an MQTT server.

I use it along with my DIY AirGradient boards. Information on how to get the components and build the boards is available [here](https://www.airgradient.com/open-airgradient/instructions/diy/) 

This script allows you to host your own solution instead of connecting the DIY Sensor to the AirGradient Dashboard. That's a very convenient option if you don't want to host yourself, you can find all the information you need [here](https://www.airgradient.com/open-airgradient/instructions/diy-connect-to-dashboard/)

Instead of running an HTTP server and polling from Prometheus as in Jeff Geerling’s solution, I like better the approach of the boards reporting the values to the MQTT server. This code is based on the solution that Jeff describes in [his blog](https://www.jeffgeerling.com/blog/2021/airgradient-diy-air-quality-monitor-co2-pm25)

I'm not using the OLED display to show the values, but you can find the relevant code in the blog post above.

## Configuration

Update the `arduino_config.h` file with your details. I use one config file per device.

`SECRET_SSID`: WiFi ssid
`SECRET_PASS`: WiFi password
`MQTT_SERVER_ADDRESS`: The MQTT server address, you can use `test.mosquitto.org` if you're not running your own MQTT server yet. I use a dockerized Mosquitto MQTT server https://mosquitto.org/
`IP_LAST_CHUNK`: 
`MQTT_TOPIC`: The MQTT topic that the values will be published to. I define one topic per device.
`DEVICE_ID`: A friendly name for your device

## Home Assistant

I use Home Assistant to track and display the sensor status. Define the sensors in your `configuration.yaml` file as described in [this repo](https://github.com/smrtnt/Open-Home-Automation/tree/master/ha_mqtt_sensor_dht22)


## License

MIT License

Copyright (c) 2022 @nsbk

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

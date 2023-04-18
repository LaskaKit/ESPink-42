# How to make display work

1. Add text from configuration.yaml to your configuration.yaml in Home Assistant (you do not have to change anything for testing, otherwise edit it to your needs).
   
2. Place fonts folder into esphome/config.

3. Add display to esphome and Home Assistant.
   
4. Copy text from espink.yaml to your device config yaml and edit necessary parameters. **Name of the device has to match your added device name.**

5. Upload code to your device. From now on, you are able to upload code wirelessly, just locally, because device is most of the time in sleep mode.

# What does it do?
It prints data from [TMEP](https://tmep.cz/) on the display after getting them from Home Assistant and goes to sleep.

![esphome](../../img/esphome.jpg)

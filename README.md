# webinker-firmware

A E-paper display that renders webpages at interval over wifi. It uses [Waveshare e-Paper](https://www.waveshare.com/4.3inch-e-paper.htm) which is controlled by an ESP32, which fetches images from a webservice, which renders webpages to grayscale images. This repository contains:

* [`webinker-epaper`](webinker-epaper): custom firmware for the [Waveshare e-Paper](https://www.waveshare.com/4.3inch-e-paper.htm) with custom firmware
* [`webinker-controller`](webinker-controller): the firmware for the ESP32 that fetches images over wifi from the webservice and sends it through to the e-Paper.

The webservice is available at `https://webinker.herokuapp.com/`. Its code is available at https://github.com/bobvanderlinden/webinker.

## Requirements

Software:

* [PlatformIO](https://platformio.org/)

Hardware:

* [Waveshare e-Paper 4.3inch](https://www.waveshare.com/4.3inch-e-paper.htm)
* [Wemos LOLIN32 v1.0.0 with ESP32](https://wiki.wemos.cc/products:lolin32:lolin32)
* [ST-Link v2](https://www.st.com/en/development-tools/st-link-v2.html)
* 4-pin header
* Micro-USB cable
* Jumper cables
* Soldering iron

## Installation

### webinker-epaper

* Solder header to e-Paper board
* Move resistor to invert reset pin (TODO: upload image)
* Connect ST-Link v2 to e-Paper (TODO: upload image)
* Plug in ST-Link v2

```
cd webinker-epaper
platformio run --target upload
```

### webinker-controller

* Plug in LOLIN32 using micro USB cable

```
cd webinker-controller
platformio run --target upload
```

## Usage

Connect e-Paper with LOLIN32:

<table>
<tr><th>e-Paper</th><th>LOLIN32</th></tr>
<tr><td>DIN</td><td>17</td></tr>
<tr><td>DOUT</td><td>16</td></tr>
<tr><td>GND</td><td>GND</td></tr>
<tr><td>VCC</td><td>5V</td></tr>
</table>

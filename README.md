# Mortar
Mortar is a software to drive an altazimuth mounted (eg. dobsonian) telescope using input from Stellarium. Mortar runs on a Raspberry Pi and it is connected with Stellarium running on the same Pi, or on a remote computer. 

## Background

I wanted to make a simple to use interface to drive a dobsonian telescope. It should use original mount, common electronics and 3D-printed parts. It should not need extensive calibration (eg. searching Polaris). It should not need doing conversion between equatorial and altazimuth coordinates.  

[![Dobsonian telescope driven by Mortar](https://img.youtube.com/vi/ajDmgW0s_2s/0.jpg)](https://www.youtube.com/watch?v=ajDmgW0s_2s)

## Prerequisites

### Software Dependencies

* Stellarium
* pigpio library
* Qt and qmake
* Qt serialport module
* Qt gamepad module
* xdo3 library


### Hardware

* Altzimuth telescope
* 3D-printed parts and bearings - see mechanical/teleskop.FCStd Parts are made with [FreeCAD Assembly3 branch.](https://github.com/realthunder/FreeCAD_assembly3/releases)
* Raspberry Pi
* Two bipolar stepper motors
* Two stepper motor drivers DRV8814 (L298 is deprecated in master, see wiringpi-legacy branch)
* Gamepad
* (Optional) GPS module
* (Optional) touch screen

An integrated shield board for Raspberry Pi is available in a separate repository [MortarShield.](https://github.com/twizzter/MortarShield/)

![Mortar Shield](https://github.com/twizzter/MortarShield/blob/main/ms11.png?raw=true)

Pins for DRV8814 drivers are described in MotorWorker.cpp

Both drivers:

    Decay: GPIO 22, physical pin 15
    xI0: GPIO17, physical pin 11, current regulation
    xI1: GPIO27, physical pin 13
    
Altitude driver:

    AENBL: GPIO 4, physical pin 7
    APHASE: GPIO 5, physical pin 29
    BENBL: GPIO 18, physical pin 12
    BPHASE: GPIO 6, physical pin 31
    
Azimuth driver:

    AENBL: GPIO 20, physical pin 38
    PHASE: GPIO 12, physical pin 32
    BENBL: GPIO 21, physical pin 21
    BPHASE: GPIO 26, physical pin 37


## Installation and Usage

If you want use GPS, enable UART in the rpi-config utility.

![UART setup][uart]

[uart]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/rpi-config.png "Raspberry UART setup" 

Download and compile the [pigpio](http://abyz.me.uk/rpi/pigpio/index.html) library.

```
git clone  https://github.com/joan2937/pigpio.git
cd pigpio
make
sudo make install
```

Download and compile and run the software.

```
sudo apt install qt5-default libqt5gamepad5-dev libqt5serialport5-dev libxdo-dev
git clone https://github.com/kwahoo2/Mortar 
cd Mortar
qmake -makefile -o Makefile Mortar.pro
make
./Mortar
```
Run Stellarium. Set remote access and configure telescope.

![Remote access setup][stellar-remote]

[stellar-remote]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/stellar-conf1.png "Setting remote access in the Stellarium" 

![Telescope setup][telescope]

[telescope]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/stellar-conf2.png "Setting a telescope in the Stellarium" 

In the Mortar open the preferences and set the telescope name from Stellarium. If you run Stellarium from remote host replace "localhost" with its IP. Set motor driver type and gear ratios.

![Mortar preferences][prefs]

[prefs]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/remote.png "Mortar preferences" 

You may use a gamepad to move your telescope. Analog stick works as coarse adjustment, dpad moves a stepper motor by a small (size is adjustable in options) step. R2 button emulates F11, and it is useful for Stellarium fullscreen toggle.

## Telescope calibration and observation

1. If Mortar is connected to Stellarium "Azimuth" and "Altitude" values should be same as in the Stellarium. 

2. If you have a GPS module connected, you should see GPS time, date and location values in Mortar. Click "Sync Stellarium with GPS" to correct your time and position. This step is optional. You may set these values manually in Stellarium. 

3. Point the telescope on an object on the sky. You may use gamepad input for this. Point the virtual telescope in the Stellarium on the same object. 

4. Click "Sync with Stellarium". Both programs are synced now. Real telescope will follow the virtual one.

5. While Mortar supports fine microstepping, constant movement may introduce vibrations. To prevent this a "Start Stop mode" is availabe. When enabled Mortat will move the telescope in predefined intervals.


### WiringPi legacy brach

wiringpi-legacy branch contains a deprecated implementation using WiringPi instead of pigpio. That brach supports full-step operation only. 


## License

Check [LICENSE](LICENSE) for details.

Mortar uses parts of [pigpio-cpp](https://github.com/skyformat99/pigpio-cpp) licensed on [Unlicense License.](pigpio-cpp-LICENSE) 

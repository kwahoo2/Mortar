# Mortar
Mortar is a software to drive an altazimuth mounted (eg. dobsonian) telescope using input from Stellarium. Mortar runs on a Raspberry Pi and it is connected with Stellarium running on the same Pi, or on a remote computer. 

## Background


## Prerequisites

### Software Dependencies

* Stellarium
* Qt and qmake
* Qt serialport module
* Qt gamepad module
* xdo3 library


### Hardware

* Rasperry Pi
* Two stepper motors
* Stepper motor drivers (DRV8814 or L298)
* Gamepad
* (Optional) GPS module
* (Optional) touch screen

## Installation and Usage

If you want use GPS, enable UART in the rpi-config utility.

![UART setup][uart]

[uart]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/rpi-config.png "Raspberry UART setup" 

Download and compile the software.

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

You may use a gamepad to move your telescope. Analog stick works as coarse adjustment, dpad moves a stepper motor by a single step. R2 button emulates F11, and it is useful for Stellarium fullscreen toggle.

## Telescope calibration and observation

1. If Mortar is connected to Stellarium "Azimuth" and "Altitude" values should be same as in the Stellarium. 

2. If you have a GPS module connected, you should see GPS time, date and location values in Mortar. Click "Sync Stellarium with GPS" to correct your time and position. This step is optional. You may set these values manually in Stellarium. 

3. Point the telescope on an object on the sky. You may use gamepad input for this. Point the virtual telescope in the Stellarium on the same object. 

4. Click "Sync with Stellarium". Both programs are synced now. Real telescope will follow the virtual one.


### Raspberry Pi4 and WiringPi

If you use Raspberry Pi4 you may encounter issues. Check for working version of WiringPi libraries:
http://wiringpi.com/wiringpi-updated-to-2-52-for-the-raspberry-pi-4b/



## License

Check [LICENSE](LICENSE) for details.

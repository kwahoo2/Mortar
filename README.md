# Mortar
Mortar is a software to drive an altazimuth mounted (eg. dobsonian) telescope using input from Stellarium. Mortar runs on a Raspberry Pi and it is connected with Stellarium running on the same Pi, or on a remote computer.
It can use popular DRV8825 or less common DRV8814 drivers.

## Background

I wanted to make a simple to use interface to drive a dobsonian telescope. It should use original mount, common electronics and 3D-printed parts. It should not need extensive calibration (eg. searching Polaris). It should not need doing conversion between equatorial and altazimuth coordinates.  

[![Dobsonian telescope driven by Mortar](https://img.youtube.com/vi/ajDmgW0s_2s/0.jpg)](https://www.youtube.com/watch?v=ajDmgW0s_2s)

## Prerequisites

### Software Dependencies

* Stellarium
* pigpio library
* cmake
* Qt 5 or 6
* Qt serialport module
* Qt charts module
* SDL3 library
* xdo3 library (optional, non-functional on Wayland)


### Hardware

* Altzimuth telescope
* 3D-printed parts and bearings - see mechanical/telescope.FCStd. Parts are compatible with FreeCAD 1.1rc1.
* Raspberry Pi (1-4, 5 is not supported by pigpio)
* Two bipolar stepper motors
* Two stepper motor drivers DRV8814 OR two DRV8825 drivers (L298 is deprecated in master, see wiringpi-legacy branch)
* (Optional) Gamepad
* (Optional) GPS module
* (Optional) touch screen

## DRV8825 driver boards
Mortar can use DRV8825 drivers, one per motor. User chooses between DRV8814 and DRV8825 selecting the required one in the preferences. Pinout is also shown here.
![Driver select][driver-select]

[driver-select]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/driver-select.png "Selecting stepper driver"

"DRV8825 Hat" has pinout adjusted for single board [WaveShare Stepper Motor HAT](https://www.waveshare.com/wiki/Stepper_Motor_HAT)

## Custom DRV8814 board
An integrated shield board for Raspberry Pi is available in a separate repository [MortarShield.](https://github.com/twizzter/MortarShield/)

![Mortar Shield](https://github.com/twizzter/MortarShield/blob/main/ms11.png?raw=true)
It can be cloned as submodule for this repository.

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

### pigpio library

pigpio is available only for Raspberry Pi 1 to 4, it cannot be used with Pi 5.
Since Debian Trixie pigpio package is not longer available. User has to compile it manually.

```
wget https://github.com/joan2937/pigpio/archive/refs/tags/v79.tar.gz
tar zxf v79.tar.gz
cd pigpio-79
make
sudo make install
```

Download, compile and run the software.

```
sudo apt install qtbase5-dev libqt5serialport5-dev libqt5charts5-dev libxdo-dev libsdl3-dev
git clone --recurse-submodules https://github.com/kwahoo2/Mortar 
cd Mortar
mkdir -p build
cd build
cmake ..
make
sudo ./Mortar
```
Run Stellarium. Set remote access and configure the telescope.

![Remote access setup][stellar-remote]

[stellar-remote]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/stellar-conf1.png "Setting remote access in the Stellarium" 

![Telescope setup][telescope]

[telescope]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/stellar-conf2.png "Setting a telescope in the Stellarium" 

In the Mortar open the preferences and set the telescope name from Stellarium. If you run Stellarium from remote host replace "localhost" with its IP. Set motor driver type and gear ratios.

![Mortar preferences][prefs]

[prefs]: https://raw.githubusercontent.com/kwahoo2/Mortar/master/.github/images/remote.png "Mortar preferences" 

You may use a gamepad to move your telescope. Analog stick works as coarse adjustment, dpad moves a stepper motor by a small (size is adjustable in options) step. R2 button emulates F11, and it is useful for Stellarium fullscreen toggle (only on X11, with libxdo).
Alternatively, if you do not have a gamepad, you can move the telescope with arrows in the main window.

## Telescope calibration and observation

1. If Mortar is connected to Stellarium "Azimuth" and "Altitude" values should be same as in the Stellarium. 

2. If you have a GPS module connected, you should see GPS time, date and location values in Mortar. Click "Sync Stellarium with GPS" to correct your time and position. This step is optional. You may set these values manually in Stellarium. 

3. Point the telescope on an object on the sky. You may use gamepad input for this or click arrows in the Mortar window. Point the virtual telescope in the Stellarium on the same object.

4. Click "Sync with Stellarium". Both programs are synced now. Real telescope will follow the virtual one.

5. While Mortar supports fine microstepping, constant movement may introduce vibrations. To prevent this a "Start Stop mode" is available. When enabled Mortar will move the telescope in predefined intervals.

## Running Mortar with root privileges but without password

If you see something like this in the console:

```
2022-12-21 20:00:32 gpioPWM: pigpio uninitialised, call gpioInitialise()
2022-12-21 20:00:32 gpioPWM: pigpio uninitialised, call gpioInitialise()
```

that means you started Mortar as a normal user. pigpio requires root privileges for PWM. Writing password for sudo can be tedious, especially when there is no keyboard connected. There is a method that allows running sudo without password:

Edit /etc/sudoers
Add following line (replacing yourname and MortarLocation):

```
yourname ALL = NOPASSWD:/MortarLocation/Mortar
```

Create a script called eg Mortar.sh that contains (replace MortarLocation):

```
#!/bin/sh
sudo /MortarLocation/Mortar
```

Make it executable:

```
chmod +x Mortar.sh
```

Now you can run Mortar by double clicking on the Mortar.sh script.


## Disabling steppers at Pi boot
Raspberry starts GPIO in an unknown state, stepper motors may consume some energy, before Mortar is even started. To avoid this, you can set ENBL pins to low (or high in the case of DRV 8825 Hat) at the RPi OS boot.

Edit:

    /boot/firmware/config.txt

and add following lines:

If DRV8814 board is used,

```
gpio=4=op,dl
gpio=18=op,dl
gpio=20=op,dl
gpio=21=op,dl
```

If DRV8825 is used (pinout as in the preferences tab),

```
gpio=4=op,dh
gpio=20=op,dh
```

If DRV8825 Hat (Waveshare) is used,

```
gpio=4=op,dl
gpio=12=op,dl
```

## Using on PC with remote Pi driver

Mortar can be compiled without GPIO support and ran on any PC. In that case you should run cli driver, on a remote Pi and connect with it from a PC.

Compile the remote driver (on the Pi):

```
cd Mortar/pi\ remote\ driver/
mkdir -p build
cd build
cmake ..
make
```

Run the driver:

```
sudo ./Mortar-Remote-Driver
```

You may also like to limit allowed client IP:

```
sudo ./Mortar-Remote-Driver xxx.xxx.xxx.xxx
```

Then run the Mortar on the PC, fill remote driver IP in the preferences and click on "Connect to remote driver".

The Pi driver should show something like:

```
sudo ./Mortar-Remote-Driver 
No IP restriction. All IPs are allowed.
Server listening on port 2137...
Connection accepted from: 192.168.1.28
Azi speed: 100
Alt speed: 100
Hold PWM: 20
Run PWM: 40
Driver id: 1
Fast decay: 0
Disable steppers: 0
Shutter mode: 0
```

## Board testing, alternative usages for the DRV8814 board
There is a very basic Python script ([mortartest.py](https://github.com/kwahoo2/Mortar/blob/master/basic-test-py/mortartest.py)) that allows using the MortarShield as basic stepper driver. It can be used as an example for building other scripts too.

Before running it you have to have the pigpio daemon running:

    sudo pigpiod

For testing the board invoke the script:

    python mortartest.py

It will move both motors 50 steps forward and backward.

To move by requester number of steps, import the script in the Python intepreter:

```
python
import mortartest
mortartest.move_stepper_to(50, 0) # move first (0) stepper 50 steps forward, ALT stepper is 0, AZI is 1
```

## License

Check [LICENSE](LICENSE) for details.

Mortar uses parts of [pigpio-cpp](https://github.com/skyformat99/pigpio-cpp) licensed on [Unlicense License.](pigpio-cpp-LICENSE) 

/* Copyright (c) 2020, Adrian Przekwas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "MotorWorker.h"
#include <QDebug>
#include <QElapsedTimer>
#include <cmath>
#include <iostream>
#include <ostream>
#include <memory>

MotorWorker::MotorWorker(QObject *parent)
                        : QThread(parent)
{
#ifdef RASPBERRYPI
    board = pigpio_wcpp::Board();
    board.initialise();
    qDebug() << "Board version: " << board.version();
#endif
}

void MotorWorker::run()
{
    worker_stopped = false;
    std::cout <<"Using driver id: " << driverId << std::endl;
#ifdef RASPBERRYPI

    std::unique_ptr<pigpio_wcpp::DigitalOutput> shutterPin, decPin, xI0Pin, xI1Pin, aPhPinAlt, bPhPinAlt, aPhPinAzi, bPhPinAzi;
    std::unique_ptr<pigpio_wcpp::DigitalOutput> enablPinAlt, dirPinAlt, stepPinAlt, enablPinAzi, dirPinAzi, stepPinAzi;
    std::unique_ptr<pigpio_wcpp::PWM> aEnblPinPWMAlt, bEnblPinPWMAlt, aEnblPinPWMAzi, bEnblPinPWMAzi;


    if (driverId == 0) //DRV8814
    {
        decPin = std::make_unique<pigpio_wcpp::DigitalOutput>(22);  //GPIO 22, physical pin 15
        xI0Pin = std::make_unique<pigpio_wcpp::DigitalOutput>(17); //GPIO17, physical pin 11, current regulation
        xI1Pin = std::make_unique<pigpio_wcpp::DigitalOutput>(27); //GPIO27, physical pin 13

        aEnblPinPWMAlt = std::make_unique<pigpio_wcpp::PWM>(4); //GPIO 4, physical pin 7
        aPhPinAlt = std::make_unique<pigpio_wcpp::DigitalOutput>(5);  //GPIO 5, physical pin 29
        bEnblPinPWMAlt = std::make_unique<pigpio_wcpp::PWM>(18); //GPIO 18, physical pin 12
        bPhPinAlt = std::make_unique<pigpio_wcpp::DigitalOutput>(6);  //GPIO 6, physical pin 31

        aEnblPinPWMAzi = std::make_unique<pigpio_wcpp::PWM>(20); //GPIO 20, physical pin 38
        aPhPinAzi = std::make_unique<pigpio_wcpp::DigitalOutput>(12);  //GPIO 12, physical pin 32
        bEnblPinPWMAzi = std::make_unique<pigpio_wcpp::PWM>(21); //GPIO 21, physical pin 21
        bPhPinAzi = std::make_unique<pigpio_wcpp::DigitalOutput>(26);  //GPIO 26, physical pin 37

        shutterPin = std::make_unique<pigpio_wcpp::DigitalOutput>(25);  //GPIO 25, physical pin 22

        xI0Pin->low();
        xI1Pin->low();
        decPin->low();
        aPhPinAlt->low();
        bPhPinAlt->low();
        aPhPinAzi->low();
        bPhPinAzi->low();
        shutterPin->low();

        aEnblPinPWMAlt->setFrequency(40000);
        aEnblPinPWMAlt->setRange(pigpio_wcpp::PWMRange(pwmRange));
        bEnblPinPWMAlt->setFrequency(40000);
        bEnblPinPWMAlt->setRange(pigpio_wcpp::PWMRange(pwmRange));

        aEnblPinPWMAzi->setFrequency(40000);
        aEnblPinPWMAzi->setRange(pigpio_wcpp::PWMRange(pwmRange));
        bEnblPinPWMAzi->setFrequency(40000);
        bEnblPinPWMAzi->setRange(pigpio_wcpp::PWMRange(pwmRange));

        /*xI1 xI0 Current
          1    1    0%
          1    0    38%
          0    1    71%
          0    0    100%*/
    }
    if (driverId == 1) //DRV8825
    {
        decPin = std::make_unique<pigpio_wcpp::DigitalOutput>(22);  //GPIO 22, physical pin 15

        enablPinAlt = std::make_unique<pigpio_wcpp::DigitalOutput>(4); //GPIO 4, physical pin 7
        dirPinAlt = std::make_unique<pigpio_wcpp::DigitalOutput>(5);  //GPIO 5, physical pin 29
        stepPinAlt = std::make_unique<pigpio_wcpp::DigitalOutput>(6);  //GPIO 6, physical pin 31

        enablPinAzi = std::make_unique<pigpio_wcpp::DigitalOutput>(20); //GPIO 20, physical pin 38
        dirPinAzi = std::make_unique<pigpio_wcpp::DigitalOutput>(12);  //GPIO 12, physical pin 32
        stepPinAzi = std::make_unique<pigpio_wcpp::DigitalOutput>(26);  //GPIO 26, physical pin 37

        decPin->low();
        enablPinAlt->high(); //DRV8825 is enabled when low state on nENBL pin
        dirPinAlt->low();
        stepPinAlt->low();

        enablPinAzi->high();
        dirPinAzi->low();
        stepPinAzi->low();
    }

    if (driverId == 2) //DRV8825 Waveshare Hat
    {

        enablPinAlt = std::make_unique<pigpio_wcpp::DigitalOutput>(12); //GPIO 12, physical pin 32
        dirPinAlt = std::make_unique<pigpio_wcpp::DigitalOutput>(13);  //GPIO 13, physical pin 33
        stepPinAlt = std::make_unique<pigpio_wcpp::DigitalOutput>(19);  //GPIO 19, physical pin 35

        enablPinAzi = std::make_unique<pigpio_wcpp::DigitalOutput>(4); //GPIO 4, physical pin 7
        dirPinAzi = std::make_unique<pigpio_wcpp::DigitalOutput>(24);  //GPIO 24, physical pin 18
        stepPinAzi = std::make_unique<pigpio_wcpp::DigitalOutput>(18);  //GPIO 18, physical pin 12

        enablPinAlt->low(); //reversed by NPN transistor cutoff
        dirPinAlt->low();
        stepPinAlt->low();

        enablPinAzi->low();
        dirPinAzi->low();
        stepPinAzi->low();
    }


#else
    std::cout << "Pigpio drivers disabled! Are you running RaspberryPi?" << std::endl;
#endif
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    lastTime = elapsedTimer.nsecsElapsed();
    while (!worker_stopped)
    {

        qint64 actTime = elapsedTimer.nsecsElapsed();
        double timeDeltaD = static_cast<double>(actTime - lastTime) / 1000000000;
        lastTime = actTime;

       //qDebug() << "MotorWorker Azi target pos: " << targetPosAzi << "act: " <<actPosAzi;
       //qDebug() << "MotorWorker Alt target pos: " << targetPosAlt << "act: " <<actPosAlt;

#ifdef RASPBERRYPI
        if (shutterModeEnabled && shutterPressAllowed) //shutterMode isderived from user's decision, shutterPressAlowed is derived from motors state
        {
            if (shutterPin) // check if not nullptr, otherwise will segfault
            {shutterPin->high();}
        }
        else
        {
            if (shutterPin)
            {shutterPin->low();}
        }

        if (driverId == 0)
        {
            driveDRV8814(aPhPinAlt, bPhPinAlt,
                         aPhPinAzi, bPhPinAzi,
                         aEnblPinPWMAlt, bEnblPinPWMAlt,
                         aEnblPinPWMAzi, bEnblPinPWMAzi,
                         decPin, timeDeltaD);
        }
        if (driverId == 1)
        {
            bool reversedEnbl = 1;
            driveDRV8825(dirPinAlt, stepPinAlt,
                         dirPinAzi, stepPinAzi,
                         enablPinAlt,
                         enablPinAzi,
                         decPin, reversedEnbl);
        }
        if (driverId == 2)
        {
            bool reversedEnbl = 0;
            driveDRV8825(dirPinAlt, stepPinAlt,
                         dirPinAzi, stepPinAzi,
                         enablPinAlt,
                         enablPinAzi,
                         decPin, reversedEnbl);
        }
#endif
    }
#ifdef RASPBERRYPI
    if (driverId == 0)
    {
        aEnblPinPWMAlt->off();
        bEnblPinPWMAlt->off();
        aEnblPinPWMAzi->off();
        bEnblPinPWMAzi->off();
    }
#endif
    std::cout << "Driver stopped" << std::endl;
}

MotorWorker::~MotorWorker()
{
#ifdef RASPBERRYPI
    board.kill();
#endif
}

#ifdef RASPBERRYPI
void MotorWorker::driveDRV8814(std::unique_ptr<pigpio_wcpp::DigitalOutput> &aPhPinAlt, std::unique_ptr<pigpio_wcpp::DigitalOutput> &bPhPinAlt,
                               std::unique_ptr<pigpio_wcpp::DigitalOutput> &aPhPinAzi, std::unique_ptr<pigpio_wcpp::DigitalOutput> &bPhPinAzi,
                               std::unique_ptr<pigpio_wcpp::PWM> &aEnblPinPWMAlt, std::unique_ptr<pigpio_wcpp::PWM> &bEnblPinPWMAlt,
                               std::unique_ptr<pigpio_wcpp::PWM> &aEnblPinPWMAzi, std::unique_ptr<pigpio_wcpp::PWM> &bEnblPinPWMAzi,
                               std::unique_ptr<pigpio_wcpp::DigitalOutput> &decPin, double timeDeltaD)
    {
    bool aPhaseAlt = 0, bPhaseAlt = 0, aPhaseAzi = 0, bPhaseAzi = 0;
    double aPWMAlt, bPWMAlt, aPWMAzi, bPWMAzi;
    if (!steppersDisabled)
    {
        calcPinsValuesDRV8814(targetPosAlt, actPosAlt, timeDeltaD, maxSpeedAlt,
                       aPhaseAlt, bPhaseAlt, aPWMAlt, bPWMAlt);
        calcPinsValuesDRV8814(targetPosAzi, actPosAzi, timeDeltaD, maxSpeedAzi,
                       aPhaseAzi, bPhaseAzi, aPWMAzi, bPWMAzi);
        aEnblPinPWMAlt->drive(static_cast<uint>(round(aPWMAlt * pwmRange)));
        bEnblPinPWMAlt->drive(static_cast<uint>(round(bPWMAlt * pwmRange)));

        if (aPhaseAlt)
        {
            aPhPinAlt->high();
        }
        else
        {
            aPhPinAlt->low();
        }
        if (bPhaseAlt)
        {
            bPhPinAlt->high();
        }
        else
        {
            bPhPinAlt->low();
        }

        aEnblPinPWMAzi->drive(static_cast<uint>(round(aPWMAzi * pwmRange)));
        bEnblPinPWMAzi->drive(static_cast<uint>(round(bPWMAzi * pwmRange)));

        if (aPhaseAzi)
        {
            aPhPinAzi->high();
        }
        else
        {
            aPhPinAzi->low();
        }
        if (bPhaseAzi)
        {
            bPhPinAzi->high();
        }
        else
        {
            bPhPinAzi->low();
        }
    }
    else
    {
        aEnblPinPWMAlt->drive(0);
        bEnblPinPWMAlt->drive(0);
        aEnblPinPWMAzi->drive(0);
        bEnblPinPWMAzi->drive(0);

    }
    if (fastDecay)
    {
        if (decPin)
        {decPin->high();}
    }
    else
    {
        if (decPin)
        {decPin->low();}
    }
    usleep(200);
}

void MotorWorker::driveDRV8825(std::unique_ptr<pigpio_wcpp::DigitalOutput> &dirPinAlt, std::unique_ptr<pigpio_wcpp::DigitalOutput> &stepPinAlt,
                               std::unique_ptr<pigpio_wcpp::DigitalOutput> &dirPinAzi, std::unique_ptr<pigpio_wcpp::DigitalOutput> &stepPinAzi,
                               std::unique_ptr<pigpio_wcpp::DigitalOutput> &enablPinAlt,
                               std::unique_ptr<pigpio_wcpp::DigitalOutput> &enablPinAzi,
                               std::unique_ptr<pigpio_wcpp::DigitalOutput> &decPin, bool reversedEn)
{
    bool enabl;
    if (!steppersDisabled)
    {
        for (int motorId = 0; motorId < 2; motorId++)
        {
            int targetUStep = 0;
            long int rUStep = realUstep[motorId];
            if (motorId == 0)
            {
                /*DRV8825 can divide in 32 mcrosteps, but indexes on rising edge only, so pin state has to be changed  2 * 32 times*/
                targetUStep = rint(targetPosAlt * 64.0);
                //qDebug() << "MotorWorker targetUstep " << targetUStep << "rUstep " <<rUStep;
            }
            if (motorId == 1)
            {
                targetUStep = rint(targetPosAzi * 64.0);
            }

            if (targetUStep > rUStep)
            {
                enabl = !reversedEn; //nENBL, LOW state active
                shutterPressAllowed = false;
                rUStep++;
                dirVal[motorId] = 1; // DIR
                if (rUStep % 2)
                    stepVal[motorId] = 1; // STEP rising edge
                else
                    stepVal[motorId] = 0; // STEP falling edge
            }
            else if (targetUStep < rUStep)
            {
                enabl = !reversedEn;
                shutterPressAllowed = false;
                rUStep--;
                dirVal[motorId] = 0;
                if (rUStep % 2)
                    stepVal[motorId] = 1;
                else
                    stepVal[motorId] = 0;
            }
            else
            {
                enabl = !reversedEn;
                shutterPressAllowed = true;
            }
            realUstep[motorId] = rUStep;
            /*driving pins happens here*/
            if (motorId == 0)
            {
                if (dirVal[motorId] == 1)
                    {dirPinAlt->high();}
                else
                    {dirPinAlt->low();}
                if (stepVal[motorId] == 1)
                    {stepPinAlt->high();}
                else
                    {stepPinAlt->low();}
                if (enabl == 1)
                    {enablPinAlt->high();}
                else
                    {enablPinAlt->low();}
            }
            if (motorId == 1)
            {
                if (dirVal[motorId] == 1)
                    {dirPinAzi->high();}
                else
                    {dirPinAzi->low();}
                if (stepVal[motorId] == 1)
                    {stepPinAzi->high();}
                else
                    {stepPinAzi->low();}
                if (enabl == 1)
                    {enablPinAzi->high();}
                else
                    {enablPinAzi->low();};
            }
            if (fastDecay)
            {
                if (decPin)
                {decPin->high();}
            }
            else
            {
                if (decPin)
                {decPin->low();}
            }
        }
        double maxSpd = maxSpeedAzi; //Azimuth speed is used for both axes
        int uStepInterval = rint(1e6 / (64 * maxSpd));
        usleep (uStepInterval);
    }
    else
    {
        //disable steppers
        if (reversedEn)
        {
            enablPinAlt->high();
            enablPinAzi->high();
        }
        else
        {
            enablPinAlt->low();
            enablPinAzi->low();
        }
    }
}

void MotorWorker::calcPinsValuesDRV8814(double targetPos, double &actPos,
                                 double timeDelta, double maxSpeed,
                                 bool &aPhase, bool &bPhase, double &aPWM, double &bPWM)
{

    double speed = 0.0;
    double absPosDiff = abs(targetPos - actPos);

    if (absPosDiff <= allowedError) //avoid Zeno's paradox
    {
        speed = absPosDiff;
        limitPower = true;
    }
    else
    {
       speed = absPosDiff * 0.95;
       limitPower = false;
    }

    if (speed < 0)
        speed = 0;

    if (speed > maxSpeed)
        speed = maxSpeed;
    if (!paused) //do not change position in "paused" state and limit power
    {
        if (actPos < targetPos && absPosDiff > allowedError)
        {
            actPos += speed * timeDelta;
        }
        else if (actPos > targetPos && absPosDiff > allowedError)
        {
            actPos -= speed * timeDelta;
        }
    }
    else
    {
        limitPower = true;
    }


    //qDebug() << "Speed " + QString::number(speed) +  " Pos - actPos " + QString::number(targetPos- actPos)+" Time " +QString::number(actTime);

    double step = 0.0;
    if (actPos >=0) //step value based on modulo is correct only for positive values, for negatives additional operations are needed
    {
        step = fmod(actPos, 4.0);
    }
    else
    {
        step = 4.0 - fmod(abs(actPos), 4.0);
    }


    if (step < 1)
    {
        aPhase = 0;
        bPhase = 1;
    }
    if (step >=1 && step < 2)
    {
        aPhase = 0;
        bPhase = 0;
    }
    if (step >=2 && step < 3)
    {
        aPhase = 1;
        bPhase = 0;
    }
    if (step >=3)
    {
        aPhase = 1;
        bPhase = 1;
    }

    double powerLimit = 1.0;
    if (limitPower)
    {
        shutterPressAllowed = true; //allowe shutter press only when stationary
        powerLimit = holdPower;
    }
    else
    {
        shutterPressAllowed = false;
        powerLimit = runPower;
    }

    aPWM = abs(sin((M_PI * step) / 2)) * powerLimit;
    bPWM = abs(cos((M_PI * step) / 2)) * powerLimit;
}
#endif

void MotorWorker::setPositionAlt(double pos)
{
    targetPosAlt = pos;
}

void MotorWorker::setPositionAzi(double pos)
{
    targetPosAzi = pos;
}

void MotorWorker::setMaxSpeedAlt(int speed)
{
    qDebug() << "MotorWorker Alt speed set: " << speed;
    maxSpeedAlt = speed;
}

void MotorWorker::setMaxSpeedAzi(int speed)
{
    qDebug() << "MotorWorker Azi speed set: " << speed;
    maxSpeedAzi = speed;
}

void MotorWorker::setHoldPWM(int val)
{
    holdPower = static_cast<double>(val) / 100.0;
}
void MotorWorker::setRunPWM(int val)
{
    runPower = static_cast<double>(val) / 100.0;
}

void MotorWorker::disableSteppers(bool val)
{
    steppersDisabled = val;
}

void MotorWorker::setPaused(bool val)
{
    paused = val;
}

void MotorWorker::setFastDecay(bool val)
{
    qDebug() << "Fast decay:" << val;
    fastDecay = val;
}

void MotorWorker::enableShutterMode(bool val)
{
    shutterModeEnabled = val;
}

void MotorWorker::setDriver(int id)
{
    driverId = id;
    qDebug() << "Driver id:" << id;
}




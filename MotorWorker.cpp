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

MotorWorker::MotorWorker(QObject *parent)
                        : QThread(parent)
{

}

void MotorWorker::run()
{
#ifdef RASPBERRYPI
    pigpio_wcpp::Board board = pigpio_wcpp::Board();
    board.initialise();
    qDebug() << "Board version: " << board.version();

    pigpio_wcpp::DigitalOutput decPin = pigpio_wcpp::DigitalOutput(22);  //GPIO 22, physical pin 15
    pigpio_wcpp::DigitalOutput xI0Pin = pigpio_wcpp::DigitalOutput(17); //GPIO17, physical pin 11, current regulation
    pigpio_wcpp::DigitalOutput xI1Pin = pigpio_wcpp::DigitalOutput(27); //GPIO27, physical pin 13

    pigpio_wcpp::PWM aEnblPinPWMAlt = pigpio_wcpp::PWM(4); //GPIO 4, physical pin 7
    pigpio_wcpp::DigitalOutput aPhPinAlt = pigpio_wcpp::DigitalOutput(5);  //GPIO 5, physical pin 29
    pigpio_wcpp::PWM bEnblPinPWMAlt = pigpio_wcpp::PWM(18); //GPIO 18, physical pin 12
    pigpio_wcpp::DigitalOutput bPhPinAlt = pigpio_wcpp::DigitalOutput(6);  //GPIO 6, physical pin 31

    pigpio_wcpp::PWM aEnblPinPWMAzi = pigpio_wcpp::PWM(20); //GPIO 20, physical pin 38
    pigpio_wcpp::DigitalOutput aPhPinAzi = pigpio_wcpp::DigitalOutput(12);  //GPIO 12, physical pin 32
    pigpio_wcpp::PWM bEnblPinPWMAzi = pigpio_wcpp::PWM(21); //GPIO 21, physical pin 21
    pigpio_wcpp::DigitalOutput bPhPinAzi = pigpio_wcpp::DigitalOutput(26);  //GPIO 26, physical pin 37

    pigpio_wcpp::DigitalOutput shutterPin = pigpio_wcpp::DigitalOutput(25);  //GPIO 25, physical pin 22

    xI0Pin.low();
    xI1Pin.low();
    decPin.low();
    aPhPinAlt.low();
    bPhPinAlt.low();
    aPhPinAzi.low();
    bPhPinAzi.low();
    shutterPin.low();

    aEnblPinPWMAlt.setFrequency(40000);
    aEnblPinPWMAlt.setRange(pigpio_wcpp::PWMRange(pwmRange));
    bEnblPinPWMAlt.setFrequency(40000);
    bEnblPinPWMAlt.setRange(pigpio_wcpp::PWMRange(pwmRange));

    aEnblPinPWMAzi.setFrequency(40000);
    aEnblPinPWMAzi.setRange(pigpio_wcpp::PWMRange(pwmRange));
    bEnblPinPWMAzi.setFrequency(40000);
    bEnblPinPWMAzi.setRange(pigpio_wcpp::PWMRange(pwmRange));

    /*xI1 xI0 Current
      1    1    0%
      1    0    38%
      0    1    71%
      0    0    100%*/

#endif

    bool aPhaseAlt, bPhaseAlt, aPhaseAzi, bPhaseAzi;
    double aPWMAlt, bPWMAlt, aPWMAzi, bPWMAzi;
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    lastTime = elapsedTimer.nsecsElapsed();
    while (!worker_stopped)
    {

        qint64 actTime = elapsedTimer.nsecsElapsed();
        double timeDeltaD = static_cast<double>(actTime - lastTime) / 1000000000;
        lastTime = actTime;
        calcPinsValues(targetPosAlt, actPosAlt, timeDeltaD, maxSpeedAlt,
                       aPhaseAlt, bPhaseAlt, aPWMAlt, bPWMAlt);
        calcPinsValues(targetPosAzi, actPosAzi, timeDeltaD, maxSpeedAzi,
                       aPhaseAzi, bPhaseAzi, aPWMAzi, bPWMAzi);

       // qDebug() << "MotorWorker Azi target pos: " << targetPosAzi << "act: " <<actPosAzi;
       // qDebug() << "MotorWorker Alt target pos: " << targetPosAlt << "act: " <<actPosAlt;

#ifdef RASPBERRYPI
        if (!steppersDisabled)
        {

            if (shutterModeEnabled && shutterPressAllowed) //shutterMode isderived from user's decision, shutterPressAlowed is derived from motors state
            {
                shutterPin.high();
            }
            else
            {
                shutterPin.low();
            }

            aEnblPinPWMAlt.drive(static_cast<uint>(round(aPWMAlt * pwmRange)));
            bEnblPinPWMAlt.drive(static_cast<uint>(round(bPWMAlt * pwmRange)));

            if (aPhaseAlt)
            {
                aPhPinAlt.high();
            }
            else
            {
                aPhPinAlt.low();
            }
            if (bPhaseAlt)
            {
                bPhPinAlt.high();
            }
            else
            {
                bPhPinAlt.low();
            }

            aEnblPinPWMAzi.drive(static_cast<uint>(round(aPWMAzi * pwmRange)));
            bEnblPinPWMAzi.drive(static_cast<uint>(round(bPWMAzi * pwmRange)));

            if (aPhaseAzi)
            {
                aPhPinAzi.high();
            }
            else
            {
                aPhPinAzi.low();
            }
            if (bPhaseAzi)
            {
                bPhPinAzi.high();
            }
            else
            {
                bPhPinAzi.low();
            }
        }
        else
        {
            aEnblPinPWMAlt.drive(0);
            bEnblPinPWMAlt.drive(0);
            aEnblPinPWMAzi.drive(0);
            bEnblPinPWMAzi.drive(0);

        }
        if (fastDecay)
        {
            decPin.high();
        }
        else
        {
            decPin.low();
        }
    #endif
        usleep(200);

    }
    #ifdef RASPBERRYPI
    aEnblPinPWMAlt.off();
    bEnblPinPWMAlt.off();
    aEnblPinPWMAzi.off();
    bEnblPinPWMAzi.off();
    board.kill();
    #endif
}

void MotorWorker::calcPinsValues(double targetPos, double &actPos,
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
       speed = absPosDiff * 0.8;
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




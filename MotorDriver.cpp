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

#include "MotorDriver.h"


MotorDriver::MotorDriver(QObject *parent) : QObject(parent)
{


    mWorker = new MotorWorker(this);
    mWorker->start();

    qDebug() << "degPerStepAzi" << degPerStepAzi;
    qDebug() << "degPerStepAlt" << degPerStepAlt;

    connect(this, SIGNAL(setStepperAlt(double)), mWorker, SLOT(setPositionAlt(double)));
    connect(this, SIGNAL(setStepperAzi(double)), mWorker, SLOT(setPositionAzi(double)));

}

void MotorDriver::startDriver()
{
    mWorker->disableSteppers(false);
}
void MotorDriver::stopDriver()
{
    mWorker->disableSteppers(true);
}

void MotorDriver::pauseDriver(bool val)
{
    mWorker->setPaused(val);
}


void MotorDriver::setTargetAltitude (double val)
{
    targetAltitude = val;
    updateAltitudeStepperTarget();


}
void MotorDriver::setTargetAzimuth (double val)
{
    targetAzimuth = val;
    updateAzimuthStepperTarget();
}

void MotorDriver::updateAltitudeStepperTarget()
{
    double currentStepTargetDiff = 0.0;
    if ((targetAltitude + manualAltitudeCorrection) > currAltitude)
    {
        currentStepTargetDiff = (targetAltitude + manualAltitudeCorrection + altHyster - currAltitude) / degPerStepAlt;
    }
    else
    {
        currentStepTargetDiff = (targetAltitude + manualAltitudeCorrection - altHyster - currAltitude) / degPerStepAlt;
    }

    emit setStepperAlt(currentStepTargetDiff);
    //qDebug() << "Altitude Target Step" << currentStepTargetDiff;
}

void MotorDriver::updateAzimuthStepperTarget()
{
    double currentStepTargetDiff = 0.0;

    if ((targetAzimuth + manualAzimuthCorrection) > currAzimuth)
    {
        currentStepTargetDiff = (targetAzimuth + manualAzimuthCorrection + aziHyster - currAzimuth) / degPerStepAzi;
    }
    else
    {
       currentStepTargetDiff = (targetAzimuth + manualAzimuthCorrection - aziHyster - currAzimuth) / degPerStepAzi;
    }

    emit setStepperAzi(currentStepTargetDiff);
    //qDebug() << "Azimuth Target Step" << currentStepTargetDiff;
}
void MotorDriver::setCurrAltitude (double val)
{
    currAltitude = val;
}
void MotorDriver::setCurrAzimuth (double val)
{
    currAzimuth = val;
}

void MotorDriver::setDegPerStepAzi(double val)
{
    degPerStepAzi = val;
    qDebug() << "Degs per Step Azimuth: " << degPerStepAzi;
}
void MotorDriver::setDegPerStepAlt(double val)
{
    degPerStepAlt = val;
    qDebug() << "Degs per Step Altitude: " << degPerStepAlt;
}

void MotorDriver::setHysterAzi(double val)
{
    aziHyster = val / 100 * degPerStepAzi;
    qDebug() << "Azimuth hysteresis: " << aziHyster;
}
void MotorDriver::setHysterAlt(double val)
{
    altHyster = val / 100 * degPerStepAlt;
   qDebug() << "Altitude hysteresis: " << altHyster;
}



void MotorDriver::setFastDecay(bool val)
{
    mWorker->setFastDecay(val);
}

void MotorDriver::altMoveStep(double val)
{
    manualAltitudeCorrection += (val * degPerStepAlt);
    updateAltitudeStepperTarget();
    emit showManualAltCorr(manualAltitudeCorrection);
}


void MotorDriver::aziMoveStep(double val)
{
    manualAzimuthCorrection += (val * degPerStepAzi);
    updateAzimuthStepperTarget();
    emit showManualAziCorr(manualAzimuthCorrection);
}

void MotorDriver::setSpeedAzi(int val)
{
    mWorker->setMaxSpeedAzi(val);
}
void MotorDriver::setSpeedAlt(int val)
{
    mWorker->setMaxSpeedAlt(val);
}

void MotorDriver::setHoldPWM(int val)
{
    mWorker->setHoldPWM(val);
}

void MotorDriver::setRunPWM(int val)
{
    mWorker->setRunPWM(val);
}


MotorDriver::~MotorDriver()
{
    mWorker->stop();
    while(mWorker->isRunning())
    {

    }
}

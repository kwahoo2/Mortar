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

#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#ifdef __arm__
#define RASPBERRYPI
#endif

#include <MotorWorker.h>

#include <QObject>
#include <QDebug>
#include <cmath>


class MotorDriver : public QObject
{
    Q_OBJECT
public:
    explicit MotorDriver(QObject *parent = nullptr);

private:
    double currAltitude = 0.0;
    double currAzimuth = 0.0;
    double targetAltitude = 0.0;
    double targetAzimuth = 0.0;
    double manualAltitudeCorrection = 0.0;
    double manualAzimuthCorrection = 0.0;
    double degPerStepAzi = 360.0/(400.0*44.0);
    double degPerStepAlt = 360.0/(400.0*22.0);
    double aziHyster = 0.1 * degPerStepAzi; //some hysteresis factor to compensate gears clearance, elemnts flexibility, should be much smaller than single step size
    double altHyster = 0.1 * degPerStepAlt;

    bool isSynced = false;
    ~MotorDriver();

    MotorWorker *mWorker;

private slots:

public:
    void startDriver();
    void stopDriver();
    void pauseDriver(bool val);

public slots:
    void setTargetAltitude (double val);
    void setTargetAzimuth (double val);
    void setCurrAltitude (double val);
    void setCurrAzimuth (double val);
    void updateAltitudeStepperTarget();
    void updateAzimuthStepperTarget();

    void aziMoveStep(double val);
    void altMoveStep(double val);

    void setDegPerStepAzi(double val);
    void setDegPerStepAlt(double val);
    void setHysterAzi(double val);
    void setHysterAlt(double val);

    void setSpeedAzi(int val);
    void setSpeedAlt(int val);

    void setFastDecay(bool val);
    void setHoldPWM(int val);
    void setRunPWM(int val);

signals:
    void setStepperAlt(double step);
    void setStepperAzi(double step);

    void showManualAltCorr(double val);
    void showManualAziCorr(double val);


};

#endif // MOTORDRIVER_H

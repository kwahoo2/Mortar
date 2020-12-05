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

#ifndef MOTORWORKER_H
#define MOTORWORKER_H

#ifdef __arm__
#define RASPBERRYPI
#endif

#ifdef RASPBERRYPI
#include <CBoard.h>
#include <CDigitalOutput.h>
#include <CPWM.h>
#endif

#include <QThread>

class MotorWorker : public QThread
{
    Q_OBJECT
public:
    explicit MotorWorker(QObject *parent = nullptr);

public:
    void stop() {worker_stopped = true;}
    void setMaxSpeedAlt(int speed);
    void setMaxSpeedAzi(int speed);
    void disableSteppers(bool val);
    void setPaused(bool val);
    void setHoldPWM(int val);
    void setRunPWM(int val);
    void setFastDecay(bool val);
    void enableShutterMode(bool val);

public slots:
    void setPositionAlt(double pos);
    void setPositionAzi(double pos);

signals:
    void sendPinStates(bool aPh, bool bPh, double aP, double bP);

private:
    void run() override;
    volatile bool worker_stopped = false;
    void calcPinsValues(double targetPos, double &actPos,
                        double timeDeltaD, double maxSpeed,
                        bool &aPhase, bool &bPhase, double &aPWM, double &bPWM);

    double targetPosAlt = 0.0;
    double actPosAlt = 0.0;
    double targetPosAzi = 0.0;
    double actPosAzi = 0.0;
    double maxSpeedAlt = 100.0;
    double maxSpeedAzi = 100.0;
    double allowedError = 0.05;
    bool limitPower = true;
    bool shutterPressAllowed = false;
    bool shutterModeEnabled = false;
    double runPower = 0.4;
    double holdPower = 0.2;
    bool steppersDisabled = false;
    bool paused = false;
    qint64 lastTime;
    uint pwmRange = 1000;
    bool fastDecay = false;

};

#endif // MOTORWORKER_H

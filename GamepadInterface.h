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

#ifndef GAMEPADINTERFACE_H
#define GAMEPADINTERFACE_H

extern "C" { //since xdo is a C not a C++ library
#include <xdo.h>
}
#undef Bool //workaround to X11 and Qt conflicts
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted
#undef True
#undef False
#undef Complex

#include <QObject>
#include <QtGamepad/QGamepad>
#include <QDebug>
#include <QTimer>
#include <cmath>

class GamepadInterface : public QObject
{
    Q_OBJECT
public:
    explicit GamepadInterface(QObject *parent = nullptr);
    ~GamepadInterface();

signals:
    void aziMoveStep(double val);
    void altMoveStep(double val);

private:
    double R2Val = 0.0, xAxisVal = 0.0, yAxisVal = 0.0;
    int poolInterval = 100;
    double deadzone = 0.1;
    int altSpeed = 100, aziSpeed = 100; //these values should be syncronized with motordriver values
    double dPadStepMul = 0.1;

    QGamepad *gamepad;
    xdo_t *x;
    QTimer *pollTimer;

private slots:
    void axisXChanged(double val);
    void axisYChanged(double val);
    void leftPressed(bool val);
    void rightPressed(bool val);
    void upPressed(bool val);
    void downPressed(bool val);
    void R2Changed(double val);
    void pollButtons();

public slots:
    void setSpeedAlt(int val);
    void setSpeedAzi(int val);
    void setStepMul(double val);

};

#endif // GAMEPADINTERFACE_H

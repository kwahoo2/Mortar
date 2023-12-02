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

#include "GamepadInterface.h"

GamepadInterface::GamepadInterface(QObject *parent) : QObject(parent), gamepad(nullptr)
{
    x = xdo_new(":0.0"); //for emulating X keypresses

    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty()) {
        qDebug() << "Did not find any connected gamepads";
    }
    else
    {
        gamepad = new QGamepad(*gamepads.begin(), this);

        connect(gamepad, SIGNAL(axisLeftXChanged(double)), this, SLOT(axisXChanged(double)));
        connect(gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(axisYChanged(double)));

        connect(gamepad, SIGNAL(buttonLeftChanged(bool)), this, SLOT(leftPressed(bool)));
        connect(gamepad, SIGNAL(buttonRightChanged(bool)), this, SLOT(rightPressed(bool)));
        connect(gamepad, SIGNAL(buttonUpChanged(bool)), this, SLOT(upPressed(bool)));
        connect(gamepad, SIGNAL(buttonDownChanged(bool)), this, SLOT(downPressed(bool)));

        connect(gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(R2Changed(double)));
    }
    pollTimer = new QTimer; //timer for polling GPIO buttons
    pollTimer->setInterval(poolInterval);
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(pollButtons()));
    pollTimer->start();

}

void GamepadInterface::axisXChanged(double val)
{
    xAxisVal = val;
}

void GamepadInterface::axisYChanged(double val)
{
    yAxisVal = val;
}

void GamepadInterface::leftPressed(bool val)
{
     if (val)
        emit aziMoveStep(-dPadStepMul * aziSpeed * (static_cast<double>(poolInterval) / 1000.0));
}
void GamepadInterface::rightPressed(bool val)
{
    if (val)
       emit aziMoveStep(dPadStepMul * aziSpeed * (static_cast<double>(poolInterval) / 1000.0));
}
void GamepadInterface::upPressed(bool val)
{
    if (val)
       emit altMoveStep(dPadStepMul * altSpeed * (static_cast<double>(poolInterval) / 1000.0));
}
void GamepadInterface::downPressed(bool val)
{
     if (val)
        emit altMoveStep(-dPadStepMul * altSpeed * (static_cast<double>(poolInterval) / 1000.0));
}

void GamepadInterface::R2Changed(double val)
{
    R2Val = val;
}

void GamepadInterface::moveUpPressed(bool val)
{
    this->upPressed(val);
    yAxisVal = static_cast<double>(-val);
}
void GamepadInterface::moveDownPressed(bool val)
{
    this->downPressed(val);
    yAxisVal = static_cast<double>(val);
}
void GamepadInterface::moveLeftPressed(bool val)
{
    this->leftPressed(val);
    xAxisVal = static_cast<double>(-val);
}
void GamepadInterface::moveRightPressed(bool val)
{
    this->rightPressed(val);
    xAxisVal = static_cast<double>(val);
}

void GamepadInterface::setSpeedAlt(int val)
{
    altSpeed = val;
}
void GamepadInterface::setSpeedAzi(int val)
{
    aziSpeed = val;
}

void GamepadInterface::setStepMul(double val)
{
     dPadStepMul = val;
}



void GamepadInterface::pollButtons()
{
    pollTimer->setInterval(poolInterval);
    if (R2Val > 0.9)
    {
        pollTimer->setInterval(1000); //dont allow toggle to often
        qDebug() << "F11 generated";
        xdo_send_keysequence_window(x, CURRENTWINDOW, "F11", 0); //F11 toggles Stellarium fullscreen and windowed mode
    }

    if (yAxisVal > deadzone || yAxisVal < -deadzone)
    {

       emit altMoveStep(-yAxisVal * altSpeed * (static_cast<double>(poolInterval) / 1000.0)); //Yaxis is reversed
    }

    if (xAxisVal > deadzone || xAxisVal < -deadzone)
    {
       emit aziMoveStep(xAxisVal * aziSpeed * (static_cast<double>(poolInterval) / 1000.0));
    }

}


GamepadInterface::~GamepadInterface()
{
    delete x;
    if (gamepad)
    {
        delete gamepad;
    }
}


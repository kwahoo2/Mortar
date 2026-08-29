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

#include <iostream>
#include "GamepadInterface.h"

GamepadInterface::GamepadInterface(QObject *parent) : QObject(parent), gamepad(nullptr)
{
    x = xdo_new(":0.0"); //for emulating X keypresses

    if (!SDL_Init(SDL_INIT_GAMEPAD)) {
        qWarning() << "SDL_Init Error:" << SDL_GetError();
        return;
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
    SDL_Event event;
    while (SDL_PollEvent(&event)) { // poll until all events are handled
        if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
            int device_index = event.gdevice.which;
            SDL_Gamepad *newGamepad = SDL_OpenGamepad(device_index);
            if (newGamepad) {
                gamepads.push_back(newGamepad);
                std::cout << "Gamepad connected: " << SDL_GetGamepadName(newGamepad)
                          << " (ID: " << device_index << ")" << std::endl;
                if (gamepads.size() == 1) { //connect only the first gamepad
                    gamepad = gamepads[0];
                    connect(this, SIGNAL(axisLeftXChanged(double)), this, SLOT(axisXChanged(double)));
                    connect(this, SIGNAL(axisLeftYChanged(double)), this, SLOT(axisYChanged(double)));
                    connect(this, SIGNAL(buttonLeftChanged(bool)), this, SLOT(leftPressed(bool)));
                    connect(this, SIGNAL(buttonRightChanged(bool)), this, SLOT(rightPressed(bool)));
                    connect(this, SIGNAL(buttonUpChanged(bool)), this, SLOT(upPressed(bool)));
                    connect(this, SIGNAL(buttonDownChanged(bool)), this, SLOT(downPressed(bool)));
                    connect(this, SIGNAL(buttonR2Changed(double)), this, SLOT(R2Changed(double)));
                }

            }
        } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            uint instance_id = event.gdevice.which;
            for (auto it = gamepads.begin(); it != gamepads.end(); ++it) {
                if (SDL_GetGamepadID(*it) == instance_id) {
                    std::cout << "Gamepad disconnected " << SDL_GetGamepadName(*it) << std::endl;
                    SDL_CloseGamepad(*it);
                    gamepads.erase(it);
                    break;
                }
            }
            if (!gamepads.size()) {
                gamepad = nullptr;
                disconnect(this, SIGNAL(axisLeftXChanged(double)), this, SLOT(axisXChanged(double)));
            }
        }
    }
    if (gamepad) {
        double leftX = static_cast<double>(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX))
        / gamepad_axis_range;
        double leftY = static_cast<double>(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY))
                       / gamepad_axis_range;
        emit(axisLeftXChanged(leftX));
        emit(axisLeftYChanged(leftY));
        bool buttonLeft = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        bool buttonRight = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
        bool buttonUp = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
        bool buttonDown = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        emit(buttonLeftChanged(buttonLeft));
        emit(buttonRightChanged(buttonRight));
        emit(buttonUpChanged(buttonUp));
        emit(buttonDownChanged(buttonDown));
        double rightTrigger = static_cast<double>(
                                  SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER))
                              / gamepad_axis_range;
        bool buttonR2 = rightTrigger > 0.7;
        emit(buttonR2Changed(buttonR2));
    }
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
    for (const auto& gamepad : gamepads) {
        SDL_CloseGamepad(gamepad);
    }
    SDL_Quit();
    delete x;
}


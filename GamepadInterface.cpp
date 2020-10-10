#include "GamepadInterface.h"

GamepadInterface::GamepadInterface(QObject *parent) : QObject(parent), gamepad(nullptr)
{
    R2Val = 0.0;
    xAxisVal = 0.0;
    yAxisVal = 0.0;
    baseInterval = 10;

    aziAxisStickSum = 0.0;
    altAxisStickSum = 0.0;

    x = xdo_new(":0.0"); //for emulating X keypresses

    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty()) {
        qDebug() << "Did not find any connected gamepads";
        return;
    }

    gamepad = new QGamepad(*gamepads.begin(), this);

    connect(gamepad, SIGNAL(axisLeftXChanged(double)), this, SLOT(axisXChanged(double)));
    connect(gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(axisYChanged(double)));

    connect(gamepad, SIGNAL(buttonLeftChanged(bool)), this, SLOT(leftPressed(bool)));
    connect(gamepad, SIGNAL(buttonRightChanged(bool)), this, SLOT(rightPressed(bool)));
    connect(gamepad, SIGNAL(buttonUpChanged(bool)), this, SLOT(upPressed(bool)));
    connect(gamepad, SIGNAL(buttonDownChanged(bool)), this, SLOT(downPressed(bool)));

    connect(gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(R2Changed(double)));

    pollTimer = new QTimer; //timer for polling GPIO buttons
    pollTimer->setInterval(baseInterval);
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(pollButtons()));
    pollTimer->start();

}

void GamepadInterface::axisXChanged(double val)
{
    qDebug() << "X" << val;
    xAxisVal = val;
}

void GamepadInterface::axisYChanged(double val)
{
    qDebug() << "Y" << val;
    yAxisVal = val;
}

void GamepadInterface::leftPressed(bool val)
{
     qDebug() << "Left" << val;
     if (val)
        emit aziMoveStep(-1);
}
void GamepadInterface::rightPressed(bool val)
{
    qDebug() << "Right" << val;
    if (val)
       emit aziMoveStep(1);
}
void GamepadInterface::upPressed(bool val)
{
    qDebug() << "Up" << val;
    if (val)
       emit altMoveStep(1);
}
void GamepadInterface::downPressed(bool val)
{
     qDebug() << "Down" << val;
     if (val)
        emit altMoveStep(-1);
}

void GamepadInterface::R2Changed(double val)
{
    qDebug() << "R2" << val;
    R2Val = val;
}

void GamepadInterface::pollButtons()
{
    pollTimer->setInterval(baseInterval);
    if (R2Val > 0.9)
    {
        pollTimer->setInterval(1000); //dont allow toggle to often
        qDebug() << "F11 generated";
        xdo_send_keysequence_window(x, CURRENTWINDOW, "F11", 0); //F11 toggles Stellarium fullscreen and windowed mode
    }
    aziAxisStickSum += xAxisVal;
    altAxisStickSum += yAxisVal;

    if (aziAxisStickSum > 0.95)
    {
       aziAxisStickSum = 0;
       emit aziMoveStep(1);
    }
    if (aziAxisStickSum < -0.95)
    {
       aziAxisStickSum = 0;
       emit aziMoveStep(-1);
    }

    if (altAxisStickSum > 0.95)
    {
       altAxisStickSum = 0;
       emit altMoveStep(-1); //axis seems to be reversed compared to DPAD
    }
    if (altAxisStickSum < -0.95)
    {
       altAxisStickSum = 0;
       emit altMoveStep(1);
    }

}

void GamepadInterface::setInterval(int val)
{
    baseInterval = val;
}


GamepadInterface::~GamepadInterface()
{
    delete x;
    delete gamepad;
}


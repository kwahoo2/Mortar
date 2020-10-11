#include "MotorDriver.h"

MotorDriver::MotorDriver(QObject *parent) : QObject(parent)
{
    #ifdef RASPBERRYPI
    if (wiringPiSetup() == -1)
        throw;
    pinMode(altPh0, OUTPUT);
    pinMode(altPh1, OUTPUT);
    pinMode(altPh2, OUTPUT);
    pinMode(altPh3, OUTPUT);
    pinMode(aziPh0, OUTPUT);
    pinMode(aziPh1, OUTPUT);
    pinMode(aziPh2, OUTPUT);
    pinMode(aziPh3, OUTPUT);

    pinMode(altAPh, OUTPUT);
    pinMode(altBPh, OUTPUT);
    pinMode(aziAPh, OUTPUT);
    pinMode(aziBPh, OUTPUT);
    pinMode(xI0, OUTPUT);
    pinMode(xI1, OUTPUT);
    pinMode(decPin, OUTPUT);
    digitalWrite(decPin, LOW);

    pinMode(pwmPin, OUTPUT); //PWM disabled by default
    digitalWrite(pwmPin, HIGH);

    #endif

    currAltitude = 0.0;
    currAzimuth = 0.0;
    targetAltitude = 0.0;
    targetAzimuth = 0.0;
    baseAltInterval = 10;
    baseAziInterval = 10;
    degPerStepAzi = 360.0/(400.0*44.0);
    degPerStepAlt = 360.0/(400.0*22.0);
    aziHyster = 0.1 * degPerStepAzi; //some hysteresis factor to compensate gears clearance, elemnts flexibility, should be much smaller than single step size
    altHyster = 0.1 * degPerStepAlt;

    motorAziPhSelection = 0; //active phases selection
    motorAltPhSelection = 0;
    motorAziPhOld = -1; //previous step phases selection
    motorAltPhOld = -1;
    holdPWM = 60; //limit power when stopped
    selectedDriver = 0; //0 - L298, 1 - DRV8814
    usePWM = 0; //PWM may require root access
    holdCurrentPreset = 0; //limit power when stopped, for DRV8814 only

    qDebug() << "degPerStepAzi" << degPerStepAzi;
    qDebug() << "degPerStepAlt" << degPerStepAlt;
    aziTimer = new QTimer;
    aziTimer->setInterval(baseAziInterval);
    connect(aziTimer, SIGNAL(timeout()), this, SLOT(aziCheckAndMove()));
    altTimer = new QTimer;
    altTimer->setInterval(baseAltInterval);
    connect(altTimer, SIGNAL(timeout()), this, SLOT(altCheckAndMove()));

    manStepHoldTimer = new QTimer; //timer to avoid full power hold, when Stellarium sychronisation is disabled
    manStepHoldTimer->setInterval(200);
    connect(manStepHoldTimer, SIGNAL(timeout()), this, SLOT(setHold()));


}

void MotorDriver::startDriver()
{
    aziTimer->start();
    altTimer->start();
}
void MotorDriver::stopDriver()
{
    aziTimer->stop();
    altTimer->stop();
    #ifdef RASPBERRYPI
    digitalWrite(altPh0, LOW);
    digitalWrite(altPh1, LOW);
    digitalWrite(altPh2, LOW);
    digitalWrite(altPh3, LOW);
    digitalWrite(aziPh0, LOW);
    digitalWrite(aziPh1, LOW);
    digitalWrite(aziPh2, LOW);
    digitalWrite(aziPh3, LOW);

    digitalWrite(xI0, HIGH); //DRV8814, current 0%
    digitalWrite(xI1, HIGH);
    #endif
}

void MotorDriver::pauseDriver()
{
    aziTimer->stop();
    altTimer->stop();
    #ifdef RASPBERRYPI
    setCurrent(holdCurrentPreset);
    if (usePWM)
    {
        pwmWrite (pwmPin, holdPWM);
    }
    #endif
}

void MotorDriver::aziCheckAndMove ()
{
    aziTimer->stop();
    int divider = static_cast<int>(round(abs((targetAzimuth - currAzimuth)/degPerStepAzi)) + 1);
    int interval = 1000 / divider;
    if (interval < baseAziInterval)
    {
        interval = baseAziInterval;
    }
    qDebug() << "azi Interval" << interval;
    aziTimer->setInterval(interval);

    if ((targetAzimuth + aziHyster - currAzimuth) >= degPerStepAzi)
    {
        qDebug() << "Azimuth STEP +";
        motorAziPhSelection++;
        if (motorAziPhSelection > 3)
            motorAziPhSelection = 0;

        currAzimuth = currAzimuth + degPerStepAzi;
    }
    else if ((-(targetAzimuth - aziHyster - currAzimuth)) >= degPerStepAzi)
    {
        qDebug() << "Azimuth STEP -";
        motorAziPhSelection--;
        if (motorAziPhSelection < 0)
            motorAziPhSelection = 3;

        currAzimuth = currAzimuth - degPerStepAzi;
    }

    checkShouldHold();
    motorAziPhOld = motorAziPhSelection;

    aziSelPhase(motorAziPhSelection);

    aziTimer->start();

}

void MotorDriver::altCheckAndMove ()
{
    altTimer->stop();
    int divider = static_cast<int>(round(abs((targetAltitude - currAltitude)/degPerStepAlt)) + 1);
    int interval = 1000 / divider;
    if (interval < baseAltInterval)
    {
        interval = baseAltInterval;
    }
    qDebug() << "alt Interval" << interval;
    altTimer->setInterval(interval);

    if ((targetAltitude + altHyster - currAltitude) >= degPerStepAlt)
    {
        qDebug() << "Altitute STEP +";
        motorAltPhSelection++;
        if (motorAltPhSelection > 3)
            motorAltPhSelection = 0;

        currAltitude = currAltitude + degPerStepAlt;
    }
    else if ((-(targetAltitude - altHyster - currAltitude)) >= degPerStepAlt)
    {
        qDebug() << "Altitute STEP -";
        motorAltPhSelection--;
        if (motorAltPhSelection < 0)
            motorAltPhSelection = 3;

        currAltitude = currAltitude - degPerStepAlt;
    }


    checkShouldHold();
    motorAltPhOld = motorAltPhSelection;

    altSelPhase(motorAltPhSelection);

    altTimer->start();
}

void MotorDriver::aziSelPhase(int ph)
{
    /*stepper motor phase selection*/
    #ifdef RASPBERRYPI
    if (selectedDriver == 0) //L298
        {
        switch (ph) {
        case 0:
            digitalWrite(aziPh0, LOW);
            digitalWrite(aziPh1, HIGH);
            digitalWrite(aziPh2, HIGH);
            digitalWrite(aziPh3, LOW);
            break;
        case 1:
            digitalWrite(aziPh0, LOW);
            digitalWrite(aziPh1, HIGH);
            digitalWrite(aziPh2, LOW);
            digitalWrite(aziPh3, HIGH);
            break;
        case 2:
            digitalWrite(aziPh0, HIGH);
            digitalWrite(aziPh1, LOW);
            digitalWrite(aziPh2, LOW);
            digitalWrite(aziPh3, HIGH);
            break;
        case 3:
            digitalWrite(aziPh0, HIGH);
            digitalWrite(aziPh1, LOW);
            digitalWrite(aziPh2, HIGH);
            digitalWrite(aziPh3, LOW);
            break;
        default:
            break;
        }
    }
    if (selectedDriver == 1) //DRV8814
        {
        setCurrent(0); //DRV8814, current 100%
        switch (ph) {
        case 0:
            digitalWrite(aziAPh, LOW);
            digitalWrite(aziBPh, HIGH);
            break;
        case 1:
            digitalWrite(aziAPh, LOW);
            digitalWrite(aziBPh, LOW);
            break;
        case 2:
            digitalWrite(aziAPh, HIGH);
            digitalWrite(aziBPh, LOW);
            break;
        case 3:
            digitalWrite(aziAPh, HIGH);
            digitalWrite(aziBPh, HIGH);
            break;
        default:
            break;
        }
    }
    #endif
}
void MotorDriver::altSelPhase(int ph)
{
    /*stepper motor phase selection*/
    #ifdef RASPBERRYPI
    if (selectedDriver == 0) //L298
        {
        switch (ph) {
        case 0:
            digitalWrite(altPh0, LOW);
            digitalWrite(altPh1, HIGH);
            digitalWrite(altPh2, HIGH);
            digitalWrite(altPh3, LOW);
            break;
        case 1:
            digitalWrite(altPh0, LOW);
            digitalWrite(altPh1, HIGH);
            digitalWrite(altPh2, LOW);
            digitalWrite(altPh3, HIGH);
            break;
        case 2:
            digitalWrite(altPh0, HIGH);
            digitalWrite(altPh1, LOW);
            digitalWrite(altPh2, LOW);
            digitalWrite(altPh3, HIGH);
            break;
        case 3:
            digitalWrite(altPh0, HIGH);
            digitalWrite(altPh1, LOW);
            digitalWrite(altPh2, HIGH);
            digitalWrite(altPh3, LOW);
            break;
        }
    }
    if (selectedDriver == 1) //DRV8814
        {
        setCurrent(0); //DRV8814, current 100%
        switch (ph) {
        case 0:
            digitalWrite(altAPh, LOW);
            digitalWrite(altBPh, HIGH);
            break;
        case 1:
            digitalWrite(altAPh, LOW);
            digitalWrite(altBPh, LOW);
            break;
        case 2:
            digitalWrite(altAPh, HIGH);
            digitalWrite(altBPh, LOW);
            break;
        case 3:
            digitalWrite(altAPh, HIGH);
            digitalWrite(altBPh, HIGH);
            break;
        default:
            break;
        }
    }
    #endif
}

void MotorDriver::aziMoveStep(int val)
{
    #ifdef RASPBERRYPI
    setCurrent(0);
    if (usePWM)
    {
        pwmWrite (pwmPin, 100);
    }
    #endif

    motorAziPhSelection += val;

    if (motorAziPhSelection > 3)
        motorAziPhSelection = 0;

    if (motorAziPhSelection < 0)
        motorAziPhSelection = 3;

    aziSelPhase(motorAziPhSelection);
    manStepHoldTimer->start();
}
void MotorDriver::altMoveStep(int val)
{
    #ifdef RASPBERRYPI
    setCurrent(0);
    if (usePWM)
    {
        pwmWrite (pwmPin, 100);
    }
    #endif

    motorAltPhSelection += val;

    if (motorAltPhSelection > 3)
        motorAltPhSelection = 0;

    if (motorAltPhSelection < 0)
        motorAltPhSelection = 3;

    altSelPhase(motorAltPhSelection);
    manStepHoldTimer->start();
}

void MotorDriver::setTargetAltitude (double val)
{
    targetAltitude = val;
}
void MotorDriver::setTargetAzimuth (double val)
{
    targetAzimuth = val;
}
void MotorDriver::setCurrAltitude (double val)
{
    currAltitude = val;
}
void MotorDriver::setCurrAzimuth (double val)
{
    currAzimuth = val;
}

void MotorDriver::checkShouldHold()
{
    #ifdef RASPBERRYPI
    int currPWM;
    if ((motorAltPhOld == motorAltPhSelection) && (motorAziPhOld == motorAziPhSelection)) //limit power when phase did not change
    {
        setCurrent(holdCurrentPreset);
        currPWM = holdPWM;
    }
    else
    {
        setCurrent(0);
        currPWM = 100;

    }
    if (usePWM)
    {
        pwmWrite (pwmPin, currPWM);
    }
    #endif

}

void MotorDriver::setHold()
{
#ifdef RASPBERRYPI
int currPWM;
setCurrent(holdCurrentPreset);
currPWM = holdPWM;

if (usePWM)
{
    pwmWrite (pwmPin, currPWM);
}
#endif

manStepHoldTimer->stop();

}

void MotorDriver::setCurrent(int selcurr)
{
    #ifdef RASPBERRYPI
    if (selectedDriver == 1) //DRV8814
    {
        switch (selcurr) {
        case 0:
            digitalWrite(xI0, LOW); //current 100%
            digitalWrite(xI1, LOW);
            break;
        case 1:
            digitalWrite(xI0, HIGH); //current 71%
            digitalWrite(xI1, LOW);
            break;
        case 2:
            digitalWrite(xI0, LOW); //current 38%
            digitalWrite(xI1, HIGH);
            break;
        case 3:
            digitalWrite(xI0, HIGH); //current 0%
            digitalWrite(xI1, HIGH);
            break;
        default:
            break;
        }
    }
    #endif
}

void MotorDriver::setDegPerStepAzi(double val)
{
    degPerStepAzi = val;
    qDebug() << "degPerStepAzi" << degPerStepAzi;
}
void MotorDriver::setDegPerStepAlt(double val)
{
    degPerStepAlt = val;
    qDebug() << "degPerStepAlt" << degPerStepAlt;
}

void MotorDriver::setIntervalAzi(int val)
{
    baseAziInterval = val;
}
void MotorDriver::setIntervalAlt(int val)
{
    baseAltInterval = val;
}

void MotorDriver::setHysterAzi(double val)
{
    aziHyster = val / 100 * degPerStepAzi;
}
void MotorDriver::setHysterAlt(double val)
{
    altHyster = val / 100 * degPerStepAlt;
}

void MotorDriver::selectDriver(int val)
{
    selectedDriver = val;
    qDebug() << "Selected driver:" << val;
}
void MotorDriver::enablePWM(int val)
{
    if (val)
    {
        if (!usePWM)
        {
            #ifdef RASPBERRYPI
            pinMode(pwmPin, PWM_OUTPUT);
            pwmSetMode(PWM_MODE_MS);
            pwmSetRange (100);
            pwmSetClock(16); //divider
            pwmWrite (pwmPin, 100);
            #endif
            qDebug() << "PWM has been enabled";
        }
    }
    else
    {
        if (usePWM)
        {
            #ifdef RASPBERRYPI
            pinMode(pwmPin, OUTPUT);
            digitalWrite(pwmPin, HIGH);
            #endif
            qDebug() << "PWM has been disabled";
        }
    }
    usePWM = val;
}

void MotorDriver::setHoldPWM(int val)
{
    holdPWM = val;
}
void MotorDriver::selectCurrentPreset(int val)
{
    holdCurrentPreset = val;
    qDebug() << "Selected current preset:" << val;
}
void MotorDriver::setFastDecay(bool val)
{
    qDebug() << "Fast decay:" << val;
    if (val)
    {
        #ifdef RASPBERRYPI
        digitalWrite(decPin, HIGH);
        #endif
    }
    else
    {
        #ifdef RASPBERRYPI
        digitalWrite(decPin, LOW);
        #endif
    }
}


MotorDriver::~MotorDriver()
{
    stopDriver();
}

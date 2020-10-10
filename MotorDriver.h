#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#ifdef __arm__
#define RASPBERRYPI
#endif

#ifdef RASPBERRYPI
#include <wiringPi.h>
#endif

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <cmath>



class MotorDriver : public QObject
{
    Q_OBJECT
public:
    explicit MotorDriver(QObject *parent = nullptr);

private:
    double currAltitude, currAzimuth;
    double targetAltitude, targetAzimuth;
    double degPerStepAzi, degPerStepAlt;
    double aziHyster, altHyster;
    int baseAziInterval, baseAltInterval;
    int holdPWM, selectedDriver, usePWM, holdCurrentPreset;
    int8_t motorAltPhSelection, motorAziPhSelection;
    int8_t motorAltPhOld, motorAziPhOld;
    QTimer *aziTimer, *altTimer;
    ~MotorDriver();

    #ifdef RASPBERRYPI
    //L298
    const int altPh0 = 21;  //GPIO5, pin 29
    const int altPh1 = 22;  //GPIO6, pin 31
    const int altPh2 = 23;  //GPIO13, pin 33
    const int altPh3 = 24;  //GPIO19, pin 35
    const int aziPh0 = 25;  //GPIO26, pin 37
    const int aziPh1 = 26;  //GPIO12, pin 32
    const int aziPh2 = 27;  //GPIO16, pin 36
    const int aziPh3 = 28;  //GPIO20, pin 38

    const int pwmPin = 1; //physical pin 12

    //DRV8814
    const int altAPh = 21;  //GPIO5, pin 29
    const int altBPh = 22;  //GPIO6, pin 31
    const int aziAPh = 25;  //GPIO26, pin 37
    const int aziBPh = 26;  //GPIO12, pin 32
    const int xI0 = 0; //GPIO17, pin 11, current regulation
    const int xI1 = 2; //GPIO27, pin 13
    const int decPin = 3; //GPIO22, pin 15, decay mode
    /*xI1 xI0 Current
      1    1    0%
      1    0    38%
      0    1    71%
      0    0    100%*/

    #endif

private slots:
    void aziCheckAndMove();
    void altCheckAndMove();
    void checkShouldHold();
    void aziSelPhase(int ph);
    void altSelPhase(int ph);
    void setCurrent(int selcurr);

public:
    void startDriver();
    void stopDriver();
    void pauseDriver();

public slots:
    void setTargetAltitude (double val);
    void setTargetAzimuth (double val);
    void setCurrAltitude (double val);
    void setCurrAzimuth (double val);

    void aziMoveStep(int val);
    void altMoveStep(int val);

    void setDegPerStepAzi(double val);
    void setDegPerStepAlt(double val);
    void setIntervalAzi(int val);
    void setIntervalAlt(int val);
    void setHysterAzi(double val);
    void setHysterAlt(double val);

    void selectDriver(int val);
    void enablePWM(int val);
    void setHoldPWM(int val);
    void selectCurrentPreset(int val);
    void setFastDecay(bool val);


};

#endif // MOTORDRIVER_H

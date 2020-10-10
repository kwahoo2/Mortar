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

class GamepadInterface : public QObject
{
    Q_OBJECT
public:
    explicit GamepadInterface(QObject *parent = nullptr);
    ~GamepadInterface();

signals:
    void aziMoveStep(const int val);
    void altMoveStep(const int val);

private:
    double R2Val, xAxisVal, yAxisVal;
    int baseInterval;
    double aziAxisStickSum, altAxisStickSum;


    QGamepad *gamepad;
    xdo_t * x;
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
    void setInterval(int val);

};

#endif // GAMEPADINTERFACE_H

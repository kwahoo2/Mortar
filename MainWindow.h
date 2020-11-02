#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <StellarInterface.h>
#include <GamepadInterface.h>
#include <MotorDriver.h>
#include <SerialDriver.h>
#include <PrefsDialog.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    double currAltitude;
    double currAzimuth;
    Ui::MainWindow *ui;
    StellarInterface *stelin;
    GamepadInterface *gamepadin;
    MotorDriver *motordriver;
    SerialDriver *serialdriver;
    PrefsDialog *prefsdialog;

private slots:
    void setAzimuth (double azimuth);
    void setAltitude (double altitude);
    void askForSerial();
    void setTimeEdit(QTime time);
    void setDateEdit(QDate date);
    void setLocationEdit(QString location);

    void on_syncButton_toggled(bool checked);
    void on_pauseButton_toggled(bool checked);
    void on_actionMotor_Preferences_triggered();

    void on_syncGPSButton_clicked();
    void on_powerDownButton_toggled(bool checked);
};
#endif // MAINWINDOW_H

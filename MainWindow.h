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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <StellarInterface.h>
#include <GamepadInterface.h>
#include <MotorDriver.h>
#include <SerialDriver.h>
#include <PrefsDialog.h>
#include <CorrectionTable.h>

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
    double currAltitude = 0.0;
    double currAzimuth = 0.0;
    double baseAltCorr = 0.0;
    double baseAziCorr = 0.0;
    double altManCorr = 0.0;
    double aziManCorr = 0.0;
    int startStopInterval = 10000;
    bool updatePaused = false;
    Ui::MainWindow *ui;
    StellarInterface *stelin;
    GamepadInterface *gamepadin;
    MotorDriver *motordriver;
    SerialDriver *serialdriver;
    PrefsDialog *prefsdialog;
    CorrectionTable *corrtable;
    QTimer *startStopTimer;

    void enableMotorDriverConnection();
    void disableMotorDriverConnection();

private slots:

    void setAzimuth (double azimuth);
    void setAltitude (double altitude);
    void setManAziCorr(double azimuth);
    void setManAltCorr(double altitude);

    void alignCurrTargetAlt(double alt);
    void alignCurrTargetAzi(double azi);

    void askForSerial();
    void setTimeEdit(QTime time);
    void setDateEdit(QDate date);
    void setLocationEdit(QString location);
    void togglePosUpdate();
    void setLocalizedName(const QString &name);

    void on_syncButton_toggled(bool checked);
    void on_pauseButton_toggled(bool checked);
    void on_actionMotor_Preferences_triggered();

    void on_syncGPSButton_clicked();
    void on_powerDownButton_toggled(bool checked);
    void on_resAziButton_clicked();
    void on_resAltButton_clicked();
    void on_startStopButton_clicked(bool checked);
    void on_startStopIntervSlider_valueChanged(int value);
    void on_actionCorrection_values_triggered();
    void on_shutterModeButton_toggled(bool checked);
    void on_virtTeleRadioButton_toggled(bool checked);
    void on_selObjRadioButton_clicked(bool checked);
};
#endif // MAINWINDOW_H

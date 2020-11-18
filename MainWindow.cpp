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

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    stelin = new StellarInterface(this);
    motordriver = new MotorDriver(this);
    serialdriver = new SerialDriver(this);
    gamepadin = new GamepadInterface(this);
    prefsdialog = new PrefsDialog(this); //dialog with options
    prefsdialog->setModal(false);

    currAltitude = 0.0;
    currAzimuth = 0.0;

    connect(stelin, SIGNAL(sendAzimuthVal(double)), this, SLOT(setAzimuth(double)));
    connect(stelin, SIGNAL(sendAltitudeVal(double)), this, SLOT(setAltitude(double)));
    connect(stelin, SIGNAL(sendTime(QTime)), this, SLOT(setTimeEdit(QTime)));
    connect(stelin, SIGNAL(sendDate(QDate)), this, SLOT(setDateEdit(QDate)));
    connect(stelin, SIGNAL(sendLocationstr(QString)), this, SLOT(setLocationEdit(QString)));

    connect(gamepadin, SIGNAL(aziMoveStep(double)), motordriver, SLOT(aziMoveStep(double)));
    connect(gamepadin, SIGNAL(altMoveStep(double)), motordriver, SLOT(altMoveStep(double)));

    connect(prefsdialog, SIGNAL(setDegPerStepAzi(double)), motordriver, SLOT(setDegPerStepAzi(double)));
    connect(prefsdialog, SIGNAL(setDegPerStepAlt(double)), motordriver, SLOT(setDegPerStepAlt(double)));

    connect(prefsdialog, SIGNAL(setSpeedAzi(int)), motordriver, SLOT(setSpeedAzi(int)));
    connect(prefsdialog, SIGNAL(setSpeedAlt(int)), motordriver, SLOT(setSpeedAlt(int)));
    connect(prefsdialog, SIGNAL(setSpeedAzi(int)), gamepadin, SLOT(setSpeedAzi(int)));
    connect(prefsdialog, SIGNAL(setSpeedAlt(int)), gamepadin, SLOT(setSpeedAlt(int)));

    connect(prefsdialog, SIGNAL(setHysterAzi(double)), motordriver, SLOT(setHysterAzi(double)));
    connect(prefsdialog, SIGNAL(setHysterAlt(double)), motordriver, SLOT(setHysterAlt(double)));

    connect(prefsdialog, SIGNAL(setHoldPWM(int)), motordriver, SLOT(setHoldPWM(int)));
    connect(prefsdialog, SIGNAL(setRunPWM(int)), motordriver, SLOT(setRunPWM(int)));

    connect(prefsdialog, SIGNAL(setFastDecay(bool)), motordriver, SLOT(setFastDecay(bool)));
    connect(serialdriver, SIGNAL(askForSerial()), this, SLOT(askForSerial()));
    connect(serialdriver, SIGNAL(listPorts(QList<QSerialPortInfo>)), prefsdialog, SLOT(addPortsNames(QList<QSerialPortInfo>)));
    connect(prefsdialog, SIGNAL(givePortSelection(QString)), serialdriver, SLOT(setPort(QString)));

    connect(prefsdialog, SIGNAL(changeTelescopeName(QString)), stelin, SLOT(changeTelescopeName(QString)));
    connect(prefsdialog, SIGNAL(changeStellHost(QString)), stelin, SLOT(changeStellHost(QString)));
    connect(serialdriver, SIGNAL(gotSerialString(QString)), stelin, SLOT(gotSerialString(QString)));

    serialdriver->refreshPorts();
    prefsdialog->loadSettings();

    serialdriver->openSerial();

    if (!(serialdriver->isOpened()))
    {
        QMessageBox::information(this, tr("Error"), tr("Unable to open serial port ") + serialdriver->getPort());
        return;
    }

    ui->setupUi(this);

    ui->syncButton->setCheckable(true);
    ui->pauseButton->setCheckable(true);
    ui->powerDownButton->setCheckable(true);

    this->setWindowFlags(this->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint); //don't allow to be hidden by Stellarium in window mode
}

void MainWindow::setAzimuth(double azimuth)
{
    currAzimuth = azimuth;
    ui->labelAzimuth->setNum(azimuth);
}
void MainWindow::setAltitude(double altitude)
{
    currAltitude = altitude;
    ui->labelAltitude->setNum(altitude);
}

MainWindow::~MainWindow()
{
    serialdriver->closeSerial();
    delete ui;
}


void MainWindow::on_syncButton_toggled(bool checked)
{
    if (checked)
    {
        ui->syncButton->setText("Sync active");
        ui->pauseButton->setEnabled(true);
        motordriver->setCurrAzimuth(currAzimuth);
        motordriver->setCurrAltitude(currAltitude);
        motordriver->setTargetAzimuth(currAzimuth);
        motordriver->setTargetAltitude(currAltitude);

        connect(stelin, SIGNAL(sendAzimuthVal(double)), motordriver, SLOT(setTargetAzimuth(double)));
        connect(stelin, SIGNAL(sendAltitudeVal(double)), motordriver, SLOT(setTargetAltitude(double)));

        motordriver->startDriver();
    }
    else
    {
        ui->syncButton->setText("Sync with Stellarium");
        ui->pauseButton->setEnabled(false);
        disconnect(stelin, SIGNAL(sendAzimuthVal(double)), motordriver, SLOT(setTargetAzimuth(double)));
        disconnect(stelin, SIGNAL(sendAltitudeVal(double)), motordriver, SLOT(setTargetAltitude(double)));

        motordriver->stopDriver();
    }


}

void MainWindow::on_pauseButton_toggled(bool checked)
{
    if (checked)
    {
        ui->pauseButton->setText("Movement paused");
        motordriver->pauseDriver(true);
    }
    else
    {
        ui->pauseButton->setText("Pause movement");
        motordriver->pauseDriver(false);
    }
}

void MainWindow::on_actionMotor_Preferences_triggered()
{
    prefsdialog->show();
}

void MainWindow::askForSerial()
{
    QMessageBox::information(this, tr("Wait"), tr("Please connect a serial device"));
    serialdriver->refreshPorts();
}

void MainWindow::setTimeEdit(QTime time)
{
    ui->timeEdit->setTime(time);
}
void MainWindow::setDateEdit(QDate date)
{
    ui->dateEdit->setDate(date);
}

void MainWindow::setLocationEdit(QString location)
{
    ui->locationEdit->setText(location);
}


void MainWindow::on_syncGPSButton_clicked()
{
    stelin->enableGPSSync(true);
}

void MainWindow::on_powerDownButton_toggled(bool checked)
{
    if (checked)
    {
        ui->powerDownButton->setText("Steppers Disabled");
        motordriver->stopDriver();
    }
    else
    {
        ui->powerDownButton->setText("Disable Steppers");
        motordriver->startDriver();
    }
}

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

#include "PrefsDialog.h"
#include "ui_PrefsDialog.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
    stepsPerMotorRotatAzi = 400.0;
    stepsPerMotorRotatAlt = 400.0;
    gearRatioAzi = 44.0;
    gearRatioAlt = 22.0;
    stellHost = "localhost";
    telescopeName ="New Telescope 1";
    ui->driverComboBox->addItem("DRV8814");
    ui->driverComboBox->addItem("DRV8825");
}

void PrefsDialog::loadSettings()
{
    if (settings.contains("Azimuth/stepsperrot") && settings.contains("Azimuth/gearratio"))
    {
        stepsPerMotorRotatAzi = settings.value("Azimuth/stepsperrot").toDouble();
        gearRatioAzi = settings.value("Azimuth/gearratio").toDouble();

        ui->aziStepsSpinBox->setValue(stepsPerMotorRotatAzi);
        ui->aziRatioSpinBox->setValue(gearRatioAzi);

        double degPerStepAzi = (360.0 / stepsPerMotorRotatAzi) / gearRatioAzi;
        emit setDegPerStepAzi(degPerStepAzi);
    }
    if (settings.contains("Altitude/stepsperrot") && settings.contains("Altitude/gearratio"))
    {
        stepsPerMotorRotatAlt = settings.value("Altitude/stepsperrot").toDouble();
        gearRatioAlt = settings.value("Altitude/gearratio").toDouble();

        ui->altStepsSpinBox->setValue(stepsPerMotorRotatAlt);
        ui->altRatioSpinBox->setValue(gearRatioAlt);

        double degPerStepAlt = (360.0 / stepsPerMotorRotatAlt) / gearRatioAlt;
        emit setDegPerStepAlt(degPerStepAlt);
    }
    if (settings.contains("Azimuth/speed"))
    {
        int speedAzi = settings.value("Azimuth/speed").toInt();
        ui->aziSpeedSpinBox->setValue(speedAzi);
        emit setSpeedAzi(speedAzi);
    }
    if (settings.contains("Altitude/speed"))
    {
        int speedAlt = settings.value("Altitude/speed").toInt();
        ui->altSpeedSpinBox->setValue(speedAlt);
        emit setSpeedAlt(speedAlt);
    }
    if (settings.contains("Azimuth/hysterpercentage"))
    {
        double hysterPercentageAzi = settings.value("Azimuth/hysterpercentage").toDouble();
        ui->aziHystSpinBox->setValue(hysterPercentageAzi);
        emit setHysterAzi(hysterPercentageAzi);
    }
    if (settings.contains("Altitude/hysterpercentage"))
    {
        double hysterPercentageAlt = settings.value("Altitude/hysterpercentage").toDouble();
        ui->altHystSpinBox->setValue(hysterPercentageAlt);
        emit setHysterAlt(hysterPercentageAlt);
    }
    if (settings.contains("Common/holdpwm"))
    {
        int holdPWM = settings.value("Common/holdpwm").toInt();
        ui->holdPWMSpinBox->setValue(holdPWM);
        emit setHoldPWM(holdPWM);

    }
    if (settings.contains("Common/runpwm"))
    {
        int runPWM = settings.value("Common/runpwm").toInt();
        ui->runPWMSpinBox->setValue(runPWM);
        emit setRunPWM(runPWM);

    }
    if (settings.contains("Common/decay"))
    {
        bool decay =  settings.value("Common/decay").toBool();
        ui->decayCheckBox->setChecked(decay);
        emit setFastDecay(decay);
    }

    if (settings.contains("Common/dpadstepsize"))
    {
        double stepSize =  settings.value("Common/dpadstepsize").toDouble();
        ui->dpadStepSpinBox->setValue(stepSize);
        emit setDpadStepSize(stepSize);

    }

    if (settings.contains("Stellarium/telescope"))
    {
        telescopeName = settings.value("Stellarium/telescope").toString();
        ui->telescopeLineEdit->setText(telescopeName);
        emit changeTelescopeName(telescopeName);
    }
    if (settings.contains("Stellarium/host"))
    {
        stellHost = settings.value("Stellarium/host").toString();
        ui->hostLineEdit->setText(stellHost);
        emit changeStellHost(stellHost);
    }
    if (settings.contains("Common/corrpointsmindist"))
    {
        double minDist = settings.value("Common/corrpointsmindist").toDouble();
        ui->pointsDistSpinBox->setValue(minDist);
        emit setPointsMinDist(minDist);
    }
    loadDriverSettings();
}

void PrefsDialog::loadPortSettings()
{
    if (settings.contains("Common/port"))
    {
        storedPort = settings.value("Common/port").toString();
        int sel = ui->portsComboBox->findData(storedPort, Qt::DisplayRole);
        if (sel != -1)
        { // -1 for not found
           ui->portsComboBox->setCurrentIndex(sel);
            emit givePortSelection(storedPort);
        }
    }
}

void PrefsDialog::loadDriverSettings()
{
    if (settings.contains("Common/driver"))
    {
        storedDriver = settings.value("Common/driver").toString();
        int sel = ui->driverComboBox->findData(storedDriver, Qt::DisplayRole);
        if (sel != -1)
        { // -1 for not found
           ui->driverComboBox->setCurrentIndex(sel);
           emit setDriver(sel);
        }
    }
}


PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::on_aziStepsSpinBox_valueChanged(double arg1)
{
    settings.setValue("Azimuth/stepsperrot", arg1);
    settings.setValue("Azimuth/gearratio", gearRatioAzi);
    stepsPerMotorRotatAzi = arg1;
    double degPerStepAzi = (360.0 / stepsPerMotorRotatAzi) / gearRatioAzi;
    emit setDegPerStepAzi(degPerStepAzi);
}

void PrefsDialog::on_altStepsSpinBox_valueChanged(double arg1)
{
    settings.setValue("Altitude/stepsperrot", arg1);
    settings.setValue("Altitude/gearratio", gearRatioAlt);
    stepsPerMotorRotatAlt = arg1;
    double degPerStepAlt = (360.0 / stepsPerMotorRotatAlt) / gearRatioAlt;
    emit setDegPerStepAlt(degPerStepAlt);
}

void PrefsDialog::on_aziRatioSpinBox_valueChanged(double arg1)
{
    settings.setValue("Azimuth/gearratio", arg1);
    settings.setValue("Azimuth/stepsperrot", stepsPerMotorRotatAzi);
    gearRatioAzi = arg1;
    double degPerStepAzi = (360.0 / stepsPerMotorRotatAzi) / gearRatioAzi;
    emit setDegPerStepAzi(degPerStepAzi);
}

void PrefsDialog::on_altRatioSpinBox_valueChanged(double arg1)
{
    settings.setValue("Altitude/gearratio", arg1);
    settings.setValue("Altitude/stepsperrot", stepsPerMotorRotatAlt);
    gearRatioAlt = arg1;
    double degPerStepAlt = (360.0 / stepsPerMotorRotatAlt) / gearRatioAlt;
    emit setDegPerStepAlt(degPerStepAlt);
}

void PrefsDialog::on_aziSpeedSpinBox_valueChanged(int arg1)
{
    settings.setValue("Azimuth/speed", arg1);
    emit setSpeedAzi(arg1);
}

void PrefsDialog::on_altSpeedSpinBox_valueChanged(int arg1)
{
    settings.setValue("Altitude/speed", arg1);
    emit setSpeedAlt(arg1);
}

void PrefsDialog::on_aziHystSpinBox_valueChanged(double arg1)
{
    settings.setValue("Azimuth/hysterpercentage", arg1);
    emit setHysterAzi(arg1);
}

void PrefsDialog::on_altHystSpinBox_valueChanged(double arg1)
{
    settings.setValue("Altitude/hysterpercentage", arg1);
    emit setHysterAlt(arg1);
}

void PrefsDialog::on_holdPWMSpinBox_valueChanged(int arg1)
{
    settings.setValue("Common/holdpwm", arg1);
    emit setHoldPWM(arg1);
}

void PrefsDialog::on_runPWMSpinBox_valueChanged(int arg1)
{
    settings.setValue("Common/runpwm", arg1);
    emit setRunPWM(arg1);
}



void PrefsDialog::on_hostLineEdit_textChanged(const QString &arg1)
{
    stellHost = arg1;
    settings.setValue("Stellarium/host", stellHost);
    emit changeStellHost(stellHost);
}


void PrefsDialog::on_telescopeLineEdit_textChanged(const QString &arg1)
{
    telescopeName = arg1;
    settings.setValue("Stellarium/telescope", telescopeName);
    emit changeTelescopeName(telescopeName);
}

void PrefsDialog::addPortsNames(QList <QSerialPortInfo> ports)
{
    ui->portsComboBox->clear();
    int idx = 0;

    for (QSerialPortInfo &port : ports)
    {
        QString portname = port.systemLocation();
        ui->portsComboBox->addItem(portname);
        ui->portsComboBox->setItemData(idx, port.manufacturer(), Qt::ToolTipRole);
        idx++;
    }
    emit givePortSelection(ui->portsComboBox->itemData(0, Qt::DisplayRole).toString());
    loadPortSettings();
}
void PrefsDialog::on_portsComboBox_activated(const QString &arg1)
{
    settings.setValue("Common/port", arg1);
    emit givePortSelection(arg1);
}


void PrefsDialog::on_decayCheckBox_toggled(bool checked)
{
    settings.setValue("Common/decay", checked);
    emit setFastDecay(checked);
}

void PrefsDialog::on_dpadStepSpinBox_valueChanged(double arg1)
{
    settings.setValue("Common/dpadstepsize", arg1);
    emit setDpadStepSize(arg1);
}

void PrefsDialog::on_pointsDistSpinBox_valueChanged(double arg1)
{
    settings.setValue("Common/corrpointsmindist", arg1);
    emit setPointsMinDist(arg1);
}

void PrefsDialog::on_driverComboBox_activated(const QString &arg1)
{
    settings.setValue("Common/driver", arg1);
    int id = ui->driverComboBox->currentIndex();
    emit setDriver(id);
}


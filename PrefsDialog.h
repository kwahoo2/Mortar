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

#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QSerialPortInfo>


namespace Ui {
class PrefsDialog;
}

class PrefsDialog : public QDialog
{
    Q_OBJECT

public:
    void loadSettings();

    explicit PrefsDialog(QWidget *parent = nullptr);
    ~PrefsDialog();

public slots:
    void addPortsNames(QList <QSerialPortInfo> ports);

signals:
    void setDegPerStepAzi(double val);
    void setDegPerStepAlt(const double val);
    void setSpeedAzi(int val);
    void setSpeedAlt(int val);
    void setHysterAzi(double val);
    void setHysterAlt(double val);
    void setHoldPWM(int val);
    void setRunPWM(int val);

    void changeTelescopeName(QString str);
    void changeStellHost(QString str);
    void givePortSelection(QString portsel);
    void giveDriverSelection(QString driversel);
    void setFastDecay(bool val);
    void setDpadStepSize(double val);
    void setPointsMinDist(double val);



private slots:
    void on_aziStepsSpinBox_valueChanged(double arg1);
    void on_altStepsSpinBox_valueChanged(double arg1);
    void on_aziRatioSpinBox_valueChanged(double arg1);
    void on_altRatioSpinBox_valueChanged(double arg1);
    void on_aziSpeedSpinBox_valueChanged(int arg1);
    void on_altSpeedSpinBox_valueChanged(int arg1);
    void on_aziHystSpinBox_valueChanged(double arg1);
    void on_altHystSpinBox_valueChanged(double arg1);
    void on_holdPWMSpinBox_valueChanged(int arg1);
    void on_runPWMSpinBox_valueChanged(int arg1);
    void on_hostLineEdit_textChanged(const QString &arg1);
    void on_telescopeLineEdit_textChanged(const QString &arg1);
    void on_portsComboBox_activated(const QString &arg1);
    void on_decayCheckBox_toggled(bool checked);
    void on_dpadStepSpinBox_valueChanged(double arg1);
    void on_pointsDistSpinBox_valueChanged(double arg1);
    void on_driverComboBox_activated(const QString &arg1);

private:
    double stepsPerMotorRotatAzi, stepsPerMotorRotatAlt;
    double gearRatioAzi, gearRatioAlt;
    QString stellHost, telescopeName;
    QString storedPort, storedDriver;
    void loadPortSettings();
    void loadDriverSettings();

    Ui::PrefsDialog *ui;
    QSettings settings;
};

#endif // PREFSDIALOG_H

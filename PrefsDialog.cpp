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
    if (settings.contains("Azimuth/baseinterval"))
    {
        int baseIntervalAzi = settings.value("Azimuth/baseinterval").toInt();
        ui->aziIntervalSpinBox->setValue(baseIntervalAzi);
        emit setIntervalAzi(baseIntervalAzi);
    }
    if (settings.contains("Altitude/baseinterval"))
    {
        int baseIntervalAlt = settings.value("Altitude/baseinterval").toInt();
        ui->altIntervalSpinBox->setValue(baseIntervalAlt);
        emit setIntervalAlt(baseIntervalAlt);
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
    if (settings.contains("Common/usepwm"))
    {
        int usePWM = settings.value("Common/usepwm").toInt();
        if (usePWM)
        {
           ui->usePWMcheckBox->setCheckState(Qt::Checked);
        }
        else
        {
           ui->usePWMcheckBox->setCheckState(Qt::Unchecked);
        }
        emit enablePWM(usePWM);
    }
    if (settings.contains("Common/driver"))
    {
        int driver = settings.value("Common/driver").toInt();
        ui->driverComboBox->setCurrentIndex(driver);
        emit selectDriver(driver);
        if (driver == 1)
        {
            ui->currentComboBox->setDisabled(false);
            ui->decayCheckBox->setDisabled(false);
        }
        else
        {
            ui->currentComboBox->setDisabled(true);
            ui->decayCheckBox->setDisabled(true);
        }
    }
    if (settings.contains("Common/driver"))
    {
        bool decay =  settings.value("Common/decay").toBool();
        ui->decayCheckBox->setChecked(decay);
        emit setFastDecay(decay);
    }
    if (settings.contains("Common/currentpreset"))
    {
        int curr = settings.value("Common/currentpreset").toInt();
        ui->currentComboBox->setCurrentIndex(curr);
        emit selectCurrentPreset(curr);
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
}

void PrefsDialog::loadPortSettings()
{
    if (settings.contains("Common/port"))
    {
        storedport = settings.value("Common/port").toString();
        int sel = ui->portsComboBox->findData(storedport, Qt::DisplayRole);
        if (sel != -1)
        { // -1 for not found
           ui->portsComboBox->setCurrentIndex(sel);
           emit givePortSelection(storedport);
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

void PrefsDialog::on_aziIntervalSpinBox_valueChanged(int arg1)
{
    settings.setValue("Azimuth/baseinterval", arg1);
    emit setIntervalAzi(arg1);
}

void PrefsDialog::on_altIntervalSpinBox_valueChanged(int arg1)
{
    settings.setValue("Altitude/baseinterval", arg1);
    emit setIntervalAlt(arg1);
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



void PrefsDialog::on_usePWMcheckBox_stateChanged(int arg1)
{
    settings.setValue("Common/usepwm", arg1);
    emit enablePWM(arg1);
}

void PrefsDialog::on_driverComboBox_activated(int index)
{
    settings.setValue("Common/driver", index);
    emit selectDriver(index);
    if (index == 1)
    {
        ui->currentComboBox->setDisabled(false);
        ui->decayCheckBox->setDisabled(false);
    }
    else
    {
        ui->currentComboBox->setDisabled(true);
        ui->decayCheckBox->setDisabled(true);
    }
}

void PrefsDialog::on_currentComboBox_activated(int index)
{
    settings.setValue("Common/currentpreset", index);
    emit selectCurrentPreset(index);
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

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
    void addPortsNames(const QList <QSerialPortInfo> ports);

signals:
    void setDegPerStepAzi(const double val);
    void setDegPerStepAlt(const double val);
    void setIntervalAzi(const int val);
    void setIntervalAlt(const int val);
    void setHysterAzi(const double val);
    void setHysterAlt(const double val);
    void setHoldPWM(const int val);
    void enablePWM(const int val);
    void selectDriver(const int val);
    void selectCurrentPreset(const int val);
    void changeTelescopeName(const QString str);
    void changeStellHost(const QString str);
    void givePortSelection(const QString portsel);
    void setFastDecay(const bool val);



private slots:
    void on_aziStepsSpinBox_valueChanged(double arg1);
    void on_altStepsSpinBox_valueChanged(double arg1);
    void on_aziRatioSpinBox_valueChanged(double arg1);
    void on_altRatioSpinBox_valueChanged(double arg1);
    void on_aziIntervalSpinBox_valueChanged(int arg1);
    void on_altIntervalSpinBox_valueChanged(int arg1);
    void on_aziHystSpinBox_valueChanged(double arg1);
    void on_altHystSpinBox_valueChanged(double arg1);
    void on_holdPWMSpinBox_valueChanged(int arg1);
    void on_usePWMcheckBox_stateChanged(int arg1);
    void on_driverComboBox_activated(int index);
    void on_currentComboBox_activated(int index);
    void on_hostLineEdit_textChanged(const QString &arg1);
    void on_telescopeLineEdit_textChanged(const QString &arg1);
    void on_portsComboBox_activated(const QString &arg1);

    void on_decayCheckBox_toggled(bool checked);

private:
    double stepsPerMotorRotatAzi, stepsPerMotorRotatAlt;
    double gearRatioAzi, gearRatioAlt;
    QString stellHost, telescopeName;
    QString storedport;
    void loadPortSettings();

    Ui::PrefsDialog *ui;
    QSettings settings;
};

#endif // PREFSDIALOG_H

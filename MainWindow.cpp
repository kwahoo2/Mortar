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

    connect(gamepadin, SIGNAL(aziMoveStep(int)), motordriver, SLOT(aziMoveStep(int)));
    connect(gamepadin, SIGNAL(altMoveStep(int)), motordriver, SLOT(altMoveStep(int)));

    connect(prefsdialog, SIGNAL(setDegPerStepAzi(double)), motordriver, SLOT(setDegPerStepAzi(double)));
    connect(prefsdialog, SIGNAL(setDegPerStepAlt(double)), motordriver, SLOT(setDegPerStepAlt(double)));

    connect(prefsdialog, SIGNAL(setIntervalAzi(int)), motordriver, SLOT(setIntervalAzi(int)));
    connect(prefsdialog, SIGNAL(setIntervalAlt(int)), motordriver, SLOT(setIntervalAlt(int)));
    connect(prefsdialog, SIGNAL(setIntervalAzi(int)), gamepadin, SLOT(setInterval(int)));

    connect(prefsdialog, SIGNAL(setHysterAzi(double)), motordriver, SLOT(setHysterAzi(double)));
    connect(prefsdialog, SIGNAL(setHysterAlt(double)), motordriver, SLOT(setHysterAlt(double)));

    connect(prefsdialog, SIGNAL(setHoldPWM(int)), motordriver, SLOT(setHoldPWM(int)));
    connect(prefsdialog, SIGNAL(enablePWM(int)), motordriver, SLOT(enablePWM(int)));
    connect(prefsdialog, SIGNAL(selectDriver(int)), motordriver, SLOT(selectDriver(int)));
    connect(prefsdialog, SIGNAL(selectCurrentPreset(int)), motordriver, SLOT(selectCurrentPreset(int)));
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
        motordriver->pauseDriver();
    }
    else
    {
        ui->pauseButton->setText("Pause movement");
        motordriver->startDriver();
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

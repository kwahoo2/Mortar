#include "SerialDriver.h"


SerialDriver::SerialDriver(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    serialStr = "";
    savedport = "/dev/ttyS0";
}
void SerialDriver::refreshPorts()
{
    QList <QSerialPortInfo> ports;
    ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty())
    {
        emit askForSerial();
    }
    else
    {
        emit listPorts(ports);
    }

}

void SerialDriver::setPort(const QString portsel)
{
    savedport = portsel;
    serial->setPortName(portsel);
    qDebug() << "Port set to " << serial->portName();
}

QString SerialDriver::getPort()
{
    return serial->portName();
}

void SerialDriver::openSerial()
{
    serial->setPortName(savedport);
    serial->setBaudRate(9600);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->open(QIODevice::ReadWrite);

    serial->clear();

    QObject::connect(serial, SIGNAL(readyRead()),
                     this, SLOT(readSerial()));
}

void SerialDriver::clearSer()
{
    serial->clear();
    serialBuffer.clear();
}

void SerialDriver::closeSerial()
{
    QObject::disconnect(serial, SIGNAL(readyRead()),
                     this, SLOT(readSerial()));
    serial->close();
}

bool SerialDriver::isOpened()
{
    return serial->isOpen();
}

SerialDriver::~SerialDriver()
{
    serial->close();
}

void SerialDriver::readSerial()
{
        serialBuffer = serial->readAll();
        serialStr += QString::fromUtf8(serialBuffer);
        QStringList serialLines;
        serialLines = serialStr.split("\r\n"); //<CR> <LF>

        if (serialLines.length() > 1)
        {
            for (auto i = 0; i < (serialLines.length()-1); ++i)
            {
                emit gotSerialString(serialLines.at(i));
            }
        }
        serialStr = serialLines.takeLast() ;

}



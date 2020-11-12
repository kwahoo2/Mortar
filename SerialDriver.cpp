/* Copyright (c) 2020, Adrian Przekwas
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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



/* Copyright (c) 2026, Adrian Przekwas
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

#include "DataSender.h"
#include <QDebug>

DataSender::DataSender(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this)) {
}

void DataSender::openSocket(QString server_ip)
{
    if (isSocketOpened()){
        closeSocket();
    }

    socket->connectToHost(server_ip, DRIVER_PORT);
    if (socket->waitForConnected(1000)) {
        emit remoteConnected();
        qDebug() << "Connected to remote:" << server_ip;
    } else {
        emit remoteDisconnected();
    }
}


void DataSender::closeSocket()
{
    if (isSocketOpened()) {
        socket->disconnectFromHost();
    }
    emit remoteDisconnected();
}

bool DataSender::isSocketOpened() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}


void DataSender::sendData(const RemoteCommand &data) {
    if (!isSocketOpened()) {
        emit remoteDisconnected();
        return;
    }

    qint64 bytesWritten = socket->write(reinterpret_cast<const char*>(&data), sizeof(data));
    if (bytesWritten == -1) {
        qDebug("Failed to send data");
        closeSocket();
        emit remoteDisconnected();
        return;
    }

    if (!socket->waitForBytesWritten(1000)) {
        qDebug("Timeout while sending data");
        closeSocket();
        emit remoteDisconnected();
        return;
    }
}

void DataSender::setPositionAlt(double val)
{
    remoteCommand.id = 0x00;
    remoteCommand.command = 0x00;
    remoteCommand.val.doubleVal = val;
    sendData(remoteCommand);
}

void DataSender::setPositionAzi(double val)
{
    remoteCommand.id = 0x01;
    remoteCommand.command = 0x00;
    remoteCommand.val.doubleVal = val;
    sendData(remoteCommand);
}

void DataSender::setDriver(int val)
{
    // does not care about id
    remoteCommand.command = 0x01;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);
}
void DataSender::setMaxSpeedAlt(int val)
{
    remoteCommand.id = 0x00;
    remoteCommand.command = 0x02;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);
}

void DataSender::setMaxSpeedAzi(int val)
{
    remoteCommand.id = 0x01;
    remoteCommand.command = 0x02;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);
}

void DataSender::disableSteppers(bool val)
{
    remoteCommand.id = 0x00;
    remoteCommand.command = 0x03;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);

}
void DataSender::setPaused(bool val)
{
    remoteCommand.id = 0x00;
    remoteCommand.command = 0x04;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);
}
void DataSender::setHoldPWM(int val)
{
    remoteCommand.id = 0x00;
    remoteCommand.command = 0x05;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);
}
void DataSender::setRunPWM(int val)
{
    remoteCommand.id = 0x00;
    remoteCommand.command = 0x06;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);
}
void DataSender::setFastDecay(bool val)
{
    remoteCommand.id = 0x00;
    remoteCommand.command = 0x07;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);
}
void DataSender::enableShutterMode(bool val)
{
    remoteCommand.id = 0x00;
    remoteCommand.command = 0x08;
    remoteCommand.val.intVal = static_cast<int64_t>(val);
    sendData(remoteCommand);
}

DataSender::~DataSender()
{
    closeSocket();
    delete socket;
}

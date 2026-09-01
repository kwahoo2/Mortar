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

#ifndef DATA_SENDER_H
#define DATA_SENDER_H

#include <cstdint>
#include <QString>
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

#define DRIVER_PORT 2137

struct RemoteCommand
{
    uint8_t id; // 0x00 - ALT, 0x01 - AZI
    uint8_t command; // 0x00 - set target value, 0x01 - set driver, 0x02 - set max speed
    union {
        int64_t intVal; // commands 0x01 and higher
        double doubleVal; // command 0x00
    } val;
};

class DataSender : public QObject
{
    Q_OBJECT
private:
    RemoteCommand remoteCommand{};
    QTcpSocket *socket;
    bool isSocketOpened() const;

public:
    DataSender(QObject *parent = nullptr);
    ~DataSender();

public slots:
    void setPositionAlt(double val);
    void setPositionAzi(double val);
    void openSocket(QString server_ip);
    void sendData(const RemoteCommand &data);
    void closeSocket(void);
    void setDriver(int val);
    void setMaxSpeedAlt(int val);
    void setMaxSpeedAzi(int val);
    void disableSteppers(bool val);
    void setPaused(bool val);
    void setHoldPWM(int val);
    void setRunPWM(int val);
    void setFastDecay(bool val);
    void enableShutterMode(bool val);

signals:
    void remoteConnected();
    void remoteDisconnected();

};


#endif // DATA_SENDER_H

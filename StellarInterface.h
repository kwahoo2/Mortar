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

#ifndef STELLARINTERFACE_H
#define STELLARINTERFACE_H

#include <QObject>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <cmath>


class StellarInterface : public QObject
{
    Q_OBJECT
public:
    explicit StellarInterface(QObject *parent = nullptr);
    void enableGPSSync(bool val);

private:
    double altitudeStell, latitudeStell, longitudeStell;
    bool syncWithGPS;
    int hours, mins;
    double secs;
    QString locationstr;
    QDate date;
    QNetworkAccessManager networkManager, postNetworkManager;
    QUrl telescopeUrl;
    QString stellHost;
    QString telescopeName;
    QNetworkRequest request, postRequest;
    QNetworkReply* currentReply;
    QTimer *timer;
    ~StellarInterface();


public slots:
    void changeTelescopeName(QString str);
    void changeStellHost(QString str);
    void gotSerialString(QString gpsStr);

private slots:
    void onResult(QNetworkReply* reply);
    void getRequest();

signals:
    void sendAzimuthVal(const double azimuth);
    void sendAltitudeVal(const double altitude);
    void sendTime (const QTime time);
    void sendDate (const QDate date);
    void sendLocationstr(QString locationstr);

};

#endif // STELLARINTERFACE_H

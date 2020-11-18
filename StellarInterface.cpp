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

#include "StellarInterface.h"

#include <QJsonObject>
#include <QJsonDocument>

StellarInterface::StellarInterface(QObject *parent) : QObject(parent)
{
    altitudeStell = 0.0;
    latitudeStell = 0.0;
    longitudeStell = 0.0;
    hours = 0;
    mins = 0;
    secs = 0.0;
    locationstr = "";
    date.setDate(2000, 1, 1);

    syncWithGPS = false;

    timer = new QTimer;
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(getRequest()));

    stellHost = "localhost";
    telescopeName ="New Telescope 1";

    telescopeUrl.setUrl("http://" + stellHost + ":8090/api/objects/info?name=" + telescopeName + "&format=json");
    qDebug() << "Stellarium telescope url:" << telescopeUrl;
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResult(QNetworkReply*)));
    request.setUrl(telescopeUrl);

    timer->start();

    postRequest = QNetworkRequest(QUrl("http://" + stellHost + ":8090/api/main/time"));
    postRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

}

void StellarInterface::changeTelescopeName(QString str)
{
    telescopeName = str;
    telescopeUrl.setUrl("http://" + stellHost + ":8090/api/objects/info?name=" + telescopeName + "&format=json");
    request.setUrl(telescopeUrl); //FIXME

    qDebug() << "Stellarium telescope url:" << telescopeUrl;
}
void StellarInterface::changeStellHost(QString str)
{
    stellHost = str;
    telescopeUrl.setUrl("http://" + stellHost + ":8090/api/objects/info?name=" + telescopeName + "&format=json");
    request.setUrl(telescopeUrl);
    qDebug() << "Stellarium telescope url:" << telescopeUrl;
}

void StellarInterface::gotSerialString(QString gpsStr)
{
    //qDebug() << "Serial string" << gpsStr;
    QStringList gpsFields;
    gpsFields = gpsStr.split(',');

    int latitudeDeg = 0;
    double latitudeMins = 0.0;
    int latitudeSign = 0;
    int longitudeDeg = 0;
    double longitudeMins = 0.0;
    int longitudeSign = 0;

    if (gpsFields.at(0) == "$GPGGA" && gpsFields.at(6).toInt()) //Field 6 is Fix
    {
        QString timestr = gpsFields.at(1);
        if (timestr.length() == 10)
        {
            QStringRef hstr(&timestr, 0, 2);
            QStringRef mstr(&timestr, 2, 2);
            QStringRef sstr(&timestr, 4, 6);

            hours = hstr.toInt();
            mins = mstr.toInt();
            secs = sstr.toDouble();
        }
        QString latitudestr =  gpsFields.at(2);
        if (latitudestr.length() == 9)
        {
            QStringRef dstr(&latitudestr, 0, 2);
            QStringRef mstr(&latitudestr, 2, 7);
            latitudeDeg = dstr.toInt();
            latitudeMins = mstr.toDouble();
            locationstr = "Lat " + latitudestr;
        }
        QString latSignstr = gpsFields.at(3);
        locationstr += latSignstr;
        if (latSignstr == "N")
        {
            latitudeSign = 1;
        }
        else if (latSignstr == "S")
        {
            latitudeSign = -1;
        }

        QString longitudestr =  gpsFields.at(4);
        if (longitudestr.length() == 10)
        {
            QStringRef dstr(&longitudestr, 0, 3);
            QStringRef mstr(&longitudestr, 3, 7);
            longitudeDeg = dstr.toInt();
            longitudeMins = mstr.toDouble();
            locationstr += " Long " + longitudestr;
        }
        QString longSignstr = gpsFields.at(5);
        locationstr += longSignstr;
        if (longSignstr == "E")
        {
            longitudeSign = 1;
        }
        else if (longSignstr == "W")
        {
            longitudeSign = -1;
        }
        QString altitudestr = gpsFields.at(9);
        locationstr += " Alt " + altitudestr;
        altitudeStell = altitudestr.toDouble();
    }

    if (gpsFields.at(0) == "$GPRMC")
    {
        QString datestr = gpsFields.at(9);
        if (datestr.length() == 6)
        {
            QStringRef daystr(&datestr, 0, 2);
            QStringRef monthstr(&datestr, 2, 2);
            QStringRef yearstr(&datestr, 4, 2);
            date.setDate(2000 + yearstr.toInt(), monthstr.toInt(), daystr.toInt());
        }

    }

    qint64 jd = date.toJulianDay();
    double jdt = static_cast<double>(jd) + (static_cast<double>(hours - 12) / 24.0) + (static_cast<double>(mins) / 1440.0) + (secs / 86400.0);
    /*qDebug() << "Time is" << hours <<"H"<< mins << "M" << secs << "S";
    qDebug() << "JD" << jd << qSetRealNumberPrecision( 10 ) << "JDT" << jdt;
    qDebug() << "Latitude" << latitudeDeg << "Deg" << latitudeMins << "M";
    qDebug() << "Longitude" << longitudeDeg << "Deg" << longitudeMins << "M";
    qDebug() << "Altitude" << altitudeStell;*/

    if (latitudeDeg != 0 && longitudeDeg !=0) //hack
    {
        latitudeStell = latitudeSign * (latitudeDeg + latitudeMins/60);
        longitudeStell = longitudeSign * (longitudeDeg + longitudeMins/60);
    }
    QTime time = QTime(hours, mins, static_cast<int>(round(secs)));
    emit sendTime(time);
    emit sendDate(date);
    emit sendLocationstr(locationstr);

    if (syncWithGPS)
    {
        postRequest.setUrl(QUrl("http://" + stellHost + ":8090/api/main/time"));
        QString script = "time=" + QString::number(jdt, 'f', 10);
        QByteArray data = script.toUtf8();
        postNetworkManager.post(postRequest, data);

        script = "latitude="+QString::number(latitudeStell, 'f', 5)+"&";
        data.append(script.toUtf8());
        script = "longitude="+QString::number(longitudeStell, 'f', 5)+"&";
        data.append(script.toUtf8());
        script = "altitude="+QString::number(altitudeStell);
        data.append(script.toUtf8());
        postRequest.setUrl(QUrl("http://" + stellHost + ":8090/api/location/setlocationfields"));
        postNetworkManager.post(postRequest, data);
        syncWithGPS = 0; //"Listen very carefully, I shall say this only once." - avoid constant changing postition and time base in Stellarium
    }
}

void StellarInterface::getRequest()
{
       currentReply = networkManager.get(request);  // get JSON data from Stellarium
}

void StellarInterface::onResult(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
        return;

    //QString data = static_cast<QString>(reply->readAll());
    QString data = QString::fromUtf8(reply->readAll());

    reply->deleteLater();

    // Parse document
    QJsonDocument doc(QJsonDocument::fromJson(data.toUtf8()));

    // Get JSON object
    QJsonObject json = doc.object();

    // Access properties
    qDebug() << "Azimuth: " << json["azimuth"].toDouble();
    qDebug() << "Altitude: " << json["altitude"].toDouble();

    emit(sendAzimuthVal(json["azimuth"].toDouble()));
    emit(sendAltitudeVal(json["altitude"].toDouble()));


}

void StellarInterface::enableGPSSync(bool val)
{
    syncWithGPS = val;
    qDebug() << "Synced with GPS:" << val;
}

StellarInterface::~StellarInterface()
{

}


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

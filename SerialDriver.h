#ifndef SERIALDRIVER_H
#define SERIALDRIVER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include "QMessageBox"

class SerialDriver : public QObject
{
    Q_OBJECT
public:
    explicit SerialDriver(QObject *parent = nullptr);
    bool isOpened();
    QString getPort();

protected:

signals:
    void gotSerialString(const QString str);
    void listPorts(const QList <QSerialPortInfo> ports);
    void askForSerial();

private:
    QSerialPort *serial;
    QString savedport;
    QByteArray serialBuffer;
    QString serialStr;
    ~SerialDriver();

private slots:
    void readSerial();

public slots:
    void openSerial();
    void closeSerial();
    void refreshPorts();
    void clearSer();
    void setPort(const QString portsel);
};

#endif // SERIALDRIVER_H

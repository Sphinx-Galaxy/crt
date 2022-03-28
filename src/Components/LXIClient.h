#ifndef LXICLIENT_H
#define LXICLIENT_H

/*
 * Author: Mattis Jaksch
 *
 * LXI Client for communication with weird power supplies
 *
 */

class QTcpSocket;
class QTimer;

#include <QMutex>
#include <QObject>

class LXIClient : public QObject
{
Q_OBJECT

public:
    LXIClient(uint port, const QString& address);
    virtual ~LXIClient();

    bool query(const QString& message, char* buffer=nullptr, int size=0);

signals:
    void connection_status(bool);

private:
    QMutex mutex;
    uint port;
    QString address;
    int device;

    static const int timeout;

    bool read(char* buffer, int size);
    bool write(QString message);
};

#endif // LXICLIENT_H

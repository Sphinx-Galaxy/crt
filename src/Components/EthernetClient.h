#ifndef ETHERNETCLIENT_H
#define ETHERNETCLIENT_H

/*
 * Author: Mattis Jaksch
 *
 * Ethernet Transceiver
 * Creates a socket and connects to a server
 * Currently adapted to communicate via SCPI
 *
 */

class QTcpSocket;
class QTimer;

#include <QObject>

class EthernetClient : public QObject
{
Q_OBJECT

public:
    EthernetClient(uint port, const QString& address);
	virtual ~EthernetClient();

    bool is_connected() const
    {
        return connection_ok;
    }

    bool write(QString message);
    bool query(const QString& message, QString& buffer, int size=0);

signals:
    void connection_status(bool);

private slots:
    void connected();
    void disconnected();

private:
    uint port;
    QString address;

    QTimer* reconnection_timer;
    bool connection_ok = false;
    QTcpSocket* socket;

    static const int timeout = 30000;

    bool read(QString& buffer, int size);
};

#endif // ETHERNETCLIENT_H

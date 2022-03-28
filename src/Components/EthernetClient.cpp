#include "EthernetClient.h"

#include <QTcpSocket>
#include <QTimer>

EthernetClient::EthernetClient(uint port, const QString &address) : port(port), address(address)
{
    socket = new QTcpSocket;
    reconnection_timer = new QTimer;

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(reconnection_timer, SIGNAL(timeout()), this, SLOT(disconnected()));

    socket->connectToHost(address, port);

    reconnection_timer->start(timeout);
}

EthernetClient::~EthernetClient()
{
    delete reconnection_timer;
    socket->deleteLater();
}

void EthernetClient::connected()
{
    connection_ok = true;
    emit connection_status(true);
}

void EthernetClient::disconnected()
{
    connection_ok = false;
    emit connection_status(false);

    /* Attempt to reconnect */
    socket->reset();
    socket->connectToHost(address, port);
}

bool EthernetClient::read(QString& buffer, int size)
{
    bool ok = true;

    if(!connection_ok)
    {
        ok &= false;
    }
    else
    {
        if(!socket->waitForReadyRead(1000)) {
            disconnected();
            ok &= false;
        }
        else
        {
            buffer = socket->readAll();

            int terminator = 0;
            while(terminator < 5 && buffer.size() < size) {
                if(socket->waitForReadyRead(10)) {
                    buffer.append(socket->readAll());
                    terminator = 0;
                }
                else
                    terminator++;
            }

            if(buffer.size() > 0)
                reconnection_timer->start(timeout);
        }
    }

    return ok;
}

bool EthernetClient::write(QString message)
{
    bool ok = true;

    if(!connection_ok)
    {
        ok &= false;
    }
    else
    {
        message += "\r\n";
        int message_size = socket->write(message.toLocal8Bit());

        if(message_size > 0)
        {
            reconnection_timer->start(timeout);
        }

        ok &= message_size == message.size();
    }

    return ok;
}

bool EthernetClient::query(const QString& message, QString& buffer, int size)
{
    bool ok = true;

    if(!connection_ok)
    {
        ok &= false;
    }
    else if(!write(message))
    {
        ok &= false;
    }
    else if(!read(buffer, size))
    {
        ok &= false;
    }

    return ok;
}

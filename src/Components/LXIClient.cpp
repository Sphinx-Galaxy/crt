#include "LXIClient.h"

#include <QTimer>
#include <lxi.h>

const int LXIClient::timeout = 2000;

LXIClient::LXIClient(uint port, const QString& address) : port(port), address(address)
{
    lxi_init();
    device = lxi_connect(address.toStdString().c_str(), port, "inst0", timeout, VXI11);
}

LXIClient::~LXIClient()
{
    lxi_disconnect(device);
}

bool LXIClient::read(char* buffer, int size)
{
    bool ok = lxi_receive(device, buffer, size, timeout);

    if(ok)
    {
        for(int i = 0; i < size; ++i)
        {
            if(buffer[i] == '\n')
            {
                buffer[i] = '\0';
            }
        }
    }
    else
    {
        buffer[0] = '\0';
    }

    return ok;
}

bool LXIClient::write(QString message)
{
    return lxi_send(device, message.toStdString().c_str(), message.size(), timeout);
}

bool LXIClient::query(const QString& message, char* buffer, int size)
{
    QMutexLocker locker(&mutex);
    bool ok = true;

    if(!write(message))
    {
        ok &= false;
    }
    else if(size != 0 && !read(buffer, size))
    {
        ok &= false;
    }

    return ok;
}

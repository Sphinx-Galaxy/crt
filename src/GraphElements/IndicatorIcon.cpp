#include "IndicatorIcon.h"

#include <QIcon>
#include <QLabel>
#include <QTimer>

IndicatorIcon::IndicatorIcon(const QString &name,
                             QPixmap connected,
                             QPixmap disconnected,
                             QSize size)
    : name(name)
{
    this->connected = connected.scaled(size);
    this->disconnected = disconnected.scaled(size);

    setNewPixmap(this->disconnected);

    eventTimer = new QTimer;
    connect(eventTimer, SIGNAL(timeout()), this, SLOT(reset_icon()));
}

IndicatorIcon::IndicatorIcon(const QString& name,
                             QPixmap connected,
                             QPixmap disconnected,
                             QPixmap waiting,
                             QPixmap event,
                             QSize size)
    : name(name)
{

    this->connected = connected.scaled(size);
    this->disconnected = disconnected.scaled(size);
    this->waiting = waiting.scaled(size);
    this->event = event.scaled(size);

    setNewPixmap(this->waiting);

    eventTimer = new QTimer;
    connect(eventTimer, SIGNAL(timeout()), this, SLOT(reset_icon()));
}

IndicatorIcon::~IndicatorIcon() {}

void IndicatorIcon::set_status(int status)
{
    switch(status)
    {
        case -1:
            setNewPixmap(waiting);
            break;
        case 0:
            setNewPixmap(disconnected);
            break;
        case 1:
            setNewPixmap(connected);
            break;
    }
}

void IndicatorIcon::reset_icon()
{
    eventTimer->stop();
    setPixmap(last);
}

void IndicatorIcon::set_event()
{
    setPixmap(event);
    eventTimer->start(500);
}

#ifndef COMPONENT_H
#define COMPONENT_H

/*
 * Author: Mattis Jaksch
 *
 * Abstract class for managing components
 * (e.g. PSU, Labjack, ...)
 * Provides logging and update functionality
 *
 */

#include "src/Configuration/ConfigElement.h"
#include "src/Manager/RunManager.h"

#include <QObject>
#include <QTimer>

class Component : public QObject, public ConfigElement
{
Q_OBJECT

public:
    Component(RunManager* runManager, const QString& config);
    virtual ~Component() override;

    QString get_element_name() const {return elementName;}

public slots:
    virtual void init() = 0;
    virtual void update() = 0;

    virtual void start_logging();
    virtual void stop_logging();

    void set_permanent_logging(int);

    void configure_timer(uint time=1000);

signals:
    void init_done();

    void is_logging(bool);

    void data_available();

protected:
    RunManager* runManager;
    QString elementName;

    QTimer* logTimer = nullptr;

    bool logging = false;
    bool permanent_logging = false;

    virtual QStringList generate_header() = 0;
};

#endif // COMPONENT_H

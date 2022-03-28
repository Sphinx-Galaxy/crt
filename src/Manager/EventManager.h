#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

/*
 * Author: Mattis Jaksch
 *
 * Allows distribution of signals
 * to different objects.
 *
 */

class SubWindow;

#include <QObject>
#include <QVector>

enum SignalType {on, off, start_log, stop_log, special};

struct RegisteredSignal
{
    QString name;
    SignalType st;

    SubWindow* sub;
    void (SubWindow::*sp)(void); // Signal Pointer
};

class EventManager : public QObject
{
Q_OBJECT

public:
    EventManager() {}
    virtual ~EventManager() {}

    QVector<RegisteredSignal*> get_signal_list() const
    {
        return signal_list;
    }

    RegisteredSignal* get_signal(const QString& name) const;

    void add_signal(const QString& name, SignalType st, SubWindow* sub, void (SubWindow::*sp)(void));
    void delete_signal(SubWindow* sub, void (SubWindow::*sp)(void));

public slots:
    void trigger_on();
    void trigger_off();
    void trigger_start_log();
    void trigger_stop_log();
    void trigger_special();

    void call_trigger(const QVector<RegisteredSignal*>& signal_list);
    void call_trigger(enum SignalType st, const QVector<RegisteredSignal*>& signal_list);

signals:
    void signal_added();
    void signal_deleted(RegisteredSignal* reg);

private:
    QVector<struct RegisteredSignal*> signal_list;
};

inline void EventManager::trigger_on()
{
    call_trigger(SignalType::on, signal_list);
}

inline void EventManager::trigger_off()
{
    call_trigger(SignalType::off, signal_list);
}

inline void EventManager::trigger_start_log()
{
    call_trigger(SignalType::start_log, signal_list);
}

inline void EventManager::trigger_stop_log()
{
    call_trigger(SignalType::stop_log, signal_list);
}

inline void EventManager::trigger_special()
{
    call_trigger(SignalType::special, signal_list);
}

#endif // EVENTMANAGER_H

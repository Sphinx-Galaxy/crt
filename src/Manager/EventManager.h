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

enum class SignalType {on, off, start_log, stop_log, special};

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

    QVector<RegisteredSignal*> get_signal_vec() const
    {
        return regSig_vec;
    }

    RegisteredSignal* get_signal(const QString& name) const;

    void add_signal(const QString& name,
                    SignalType st,
                    SubWindow* sub,
                    void (SubWindow::*sp)(void));

    void delete_signal(SubWindow* sub, void (SubWindow::*sp)(void));

public slots:
    void trigger_on();
    void trigger_off();
    void trigger_start_log();
    void trigger_stop_log();
    void trigger_special();

    void call_trigger(const QVector<RegisteredSignal*>& regSig_vec);
    void call_trigger(enum SignalType st, const QVector<RegisteredSignal*>& regSig_vec);

signals:
    void signal_added();
    void signal_deleted(RegisteredSignal* reg);

private:
    QVector<RegisteredSignal*> regSig_vec;
};

inline void EventManager::trigger_on()
{
    call_trigger(SignalType::on, regSig_vec);
}

inline void EventManager::trigger_off()
{
    call_trigger(SignalType::off, regSig_vec);
}

inline void EventManager::trigger_start_log()
{
    call_trigger(SignalType::start_log, regSig_vec);
}

inline void EventManager::trigger_stop_log()
{
    call_trigger(SignalType::stop_log, regSig_vec);
}

inline void EventManager::trigger_special()
{
    call_trigger(SignalType::special, regSig_vec);
}

#endif // EVENTMANAGER_H

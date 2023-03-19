#ifndef SEQUENCER_H
#define SEQUENCER_H

/*
 * Author: Mattis Jaksch
 *
 * Holds a number of task which are routed
 * to available SubWindow signals (e.g. PSU switch off)
 * together with a waiting / sleep timer
 *
*/

class Task;

#include "Component.h"

class Sequencer : public Component
{
Q_OBJECT

public:
    Sequencer(RunManager* runManager, const QString& config);
    Sequencer(RunManager* runManager, const QString& m_element_name, int task_number);
    ~Sequencer() override;

    QVector<Task*> get_task_vec() const
    {
        return task_vec;
    }

    void set_config() override;

public slots:
    void init() override;
    void update() override;

    void set_loop(int);

    void start_seq();
    void stop_seq();

private slots:
    void update_task_vec();

signals:
    void task_vec_changed();

private:
    QVector<Task*> task_vec;
    QVector<Task*>::iterator task_it;
    int task_number = 0;

    bool loop = false;

    QTimer* taskTimer = nullptr;

    QStringList generate_header() override;
};

inline QStringList Sequencer::generate_header()
{
    return {"number", "time", "signal"};
}

inline void Sequencer::set_loop(int loop)
{
    this->loop = loop == 0 ? false : true;
}

class Task : public QObject
{
Q_OBJECT

uint number;
uint time;
QString signal_name;
RegisteredSignal* sig;
bool active = false;

public:
    Task(uint number,
         uint time,
         QString signal_name,
         RegisteredSignal* sig)
        : number(number),
          time(time),
          signal_name(signal_name),
          sig(sig)
    {
    }

    virtual ~Task() {}

    uint get_number() const
    {
        return number;
    }

    uint get_time() const
    {
        return time;
    }

    QString get_signal_name() const
    {
        return signal_name;
    }

    RegisteredSignal* get_signal() const
    {
        return sig;
    }

    bool is_active() const
    {
        return active;
    }

public slots:
    void set_active(bool active);
    void set_signal(RegisteredSignal* sig);
    void set_time(const QString& time);

    void signal_button_clicked();

signals:
    void update_task(Task *);
    void signal_name_changed(const QString&);
    void status_changed(int);
};

inline void Task::set_active(bool active) {
    this->active = active;

    emit status_changed(active ? 1 : 0);
}

inline void Task::set_signal(RegisteredSignal* sig) {
    this->sig = sig;

    if(sig != nullptr)
    {
        this->signal_name = sig->name;

        emit signal_name_changed(this->signal_name);
    }
}

inline void Task::set_time(const QString& time)
{
    this->time = time.toUInt();
}

inline void Task::signal_button_clicked()
{
    emit update_task(this);
}

#endif // SEQUENCER_H

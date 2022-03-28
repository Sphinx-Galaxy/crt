#ifndef RUNMANAGER_H
#define RUNMANAGER_H

/*
 * Author: Mattis Jaksch
 *
 * Information on run status and run management
 * (e.g. start/stop time and logging of components).
 *
 */

class QElapsedTimer;

#include "EventManager.h"
#include "FileManager.h"

#include <QWidget>

enum class RunMode {Creation=0, Init=1, StartRun=3, StartLog=4, StopLog=5,
                  AddComponent=6, RemoveComponent=7, Event=8, StopRun=255};

Q_DECLARE_METATYPE(RunMode) // So it can be used in slots and signals

class RunManager : public QWidget, public FileManager {
Q_OBJECT

public:
    RunManager(EventManager* eventManager);
    virtual ~RunManager() {}

    EventManager* get_eventManager() {return eventManager;}

    bool is_valid() const
    {
        return valid;
    }

    bool is_running() const
    {
        return running;
    }

public slots:
    void start_run();
    void stop_run();

private slots:
    void update_run();
    void create_new_run();

signals:
    void isValid_changed(bool);
    void isRunning_changed(bool);

    void run_name_changed(const QString&);
    void run_time_changed(const QString&);
    void run_mode_changed(enum RunMode);

private:
    QString runName;

    EventManager* eventManager;

    QElapsedTimer* runTime;
    QTimer* updateTime;
    long long offsetTime;

    bool valid = false;
    bool running = false;

    void set_run_valid(bool valid);

    void create_layout();
};

#endif // RUNMANAGER_H

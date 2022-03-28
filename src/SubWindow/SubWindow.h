#ifndef SUBWINDOW_H
#define SUBWINDOW_H

/*
 * Author: Mattis Jaksch
 *
 * SubWindows are the lowest graphical elements; they can be called
 * to be configured or to pass their configuration.
 * As alternative, items can be manually added via dialogs.
 *
 * The add-element dialog has to be created locally by the
 * subwindows, while the add-trigger dialog can be created globally
 *
 */

class SpecSignalDialog;

#include "src/Components/Component.h"
#include "src/Manager/RunManager.h"

#include <QWidget>

class SubWindow : public QWidget
{
Q_OBJECT

public:
    SubWindow(RunManager* runManager, Component* component);
    virtual ~SubWindow();

    QString get_config();

    Component* get_component() const
    {
        return component;
    }

public slots:
    /* Event / Signal management */
    void show_signal_dialog();
    void add_signal();
    void delete_signal(RegisteredSignal* regSig);

protected slots:
    //Configuration
    virtual void post_init(); //For all the signals

    void trigger_signal_vec();

    virtual void create_layout() = 0;

signals:
    /* Event / Signal management */
    void signal_on();
    void signal_off();
    void signal_start_log();
    void signal_stop_log();
    void signal_event();

    //Override for WindowTab
    void destroyed(SubWindow* subWindow);

    void layout_done();

protected:
    /* Event and signal management */
    RunManager* runManager;
    EventManager* eventManager;
    QVector<RegisteredSignal*> regSig_vec;
    SpecSignalDialog* signalDialog = nullptr;

    Component* component;
    QThread* thread;

    QString get_signal_string();
};

inline void SubWindow::trigger_signal_vec()
{
    eventManager->call_trigger(regSig_vec);
}

#endif // SUBWINDOW_H

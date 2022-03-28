#include "SubWindow.h"

#include "src/Dialog/SpecSignalDialog.h"

#include <QCheckBox>
#include <QPushButton>
#include <QThread>

SubWindow::SubWindow(RunManager* runManager, Component* component)
    : runManager(runManager), component(component)
{
    this->eventManager = runManager->get_eventManager();

    /* Thread management */
    thread = new QThread;
    component->moveToThread(thread);
    connect(thread, SIGNAL(started()), component, SLOT(init()));
    connect(component, SIGNAL(init_done()), this, SLOT(create_layout()));

    thread->start();

    /* Announce logging */
    connect(this, SIGNAL(signal_start_log()), component, SLOT(start_logging()));
    eventManager->add_signal(component->get_element_name() + " Start Log", SignalType::start_log, this, &SubWindow::signal_start_log);

    connect(this, SIGNAL(signal_stop_log()), component, SLOT(stop_logging()));
    eventManager->add_signal(component->get_element_name() + " Stop Log", SignalType::stop_log, this, &SubWindow::signal_stop_log);

    /* Post config management */
    connect(eventManager, SIGNAL(signal_added()), this, SLOT(post_init()));
    connect(eventManager, SIGNAL(signal_deleted(RegisteredSignal*)), this, SLOT(delete_signal(RegisteredSignal*)));
}

SubWindow::~SubWindow()
{
    /* Delete signals */
    eventManager->delete_signal(this, &SubWindow::signal_start_log);
    eventManager->delete_signal(this, &SubWindow::signal_stop_log);

    delete signalDialog;

    thread->quit();

    //Notify the WindowTab
    emit destroyed(this);
}

QString SubWindow::get_config()
{
    component->set_config();
    component->set_value("signal", get_signal_string());

    return component->get_config();
}

QString SubWindow::get_signal_string()
{
    QString res = "";

    foreach(RegisteredSignal* regSig, regSig_vec)
    {
        res += regSig->name + ";";
    }

    return res;
}

void SubWindow::post_init()
{
    QString signal_names = component->get_value("signal");

    if(signal_names.count(";") != regSig_vec.size())
    {
        foreach (RegisteredSignal* regSig, eventManager->get_signal_vec())
        {
            if(signal_names.contains(regSig->name) && !regSig_vec.contains(regSig))
            {
                regSig_vec.push_back(regSig);
            }
        }
    }
}

/* The dialog has to be re-created as the signals might change over time */
void SubWindow::show_signal_dialog()
{
    delete signalDialog;

    signalDialog = new SpecSignalDialog;

    /* Check every signal thats already listed */
    foreach (RegisteredSignal* regSig, eventManager->get_signal_vec())
    {
        if(regSig->st != SignalType::start_log && regSig->st != SignalType::stop_log)
        {
            regSig_vec.contains(regSig) ?
                        signalDialog->add_entry(true, regSig) :
                        signalDialog->add_entry(false, regSig);
        }
    }

    connect(signalDialog, SIGNAL(accepted()), this, SLOT(add_signal()));

    signalDialog->create_dialog();
    signalDialog->show();
}

void SubWindow::add_signal()
{
    foreach (RegisteredSignalBox regSigBox, signalDialog->get_regSigBox_vec())
    {
        if(regSigBox.checkBox->isChecked() && !regSig_vec.contains(regSigBox.regSig))
        {
            regSig_vec.push_back(regSigBox.regSig);
        }
    }
}

void SubWindow::delete_signal(RegisteredSignal* regSig)
{
    regSig_vec.removeAll(regSig);
}

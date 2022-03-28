#include "EventManager.h"

void EventManager::add_signal(const QString& name,
                              SignalType st,
                              SubWindow* sub,
                              void (SubWindow::*sp)(void))
{
    regSig_vec.push_back(new RegisteredSignal{name, st, sub, sp});
    emit signal_added();
}

void EventManager::delete_signal(SubWindow* sub, void (SubWindow::*sp)(void))
{
    for(int i = 0; i < regSig_vec.size(); ++i)
    {
        if(regSig_vec[i]->sub == sub && regSig_vec[i]->sp == sp)
        {
            emit signal_deleted(regSig_vec[i]);

            delete regSig_vec[i];
            regSig_vec.remove(i);

            break;
        }
    }
}

void EventManager::call_trigger(const QVector<RegisteredSignal*>& regSig_vec)
{
    foreach(RegisteredSignal* regSig, regSig_vec)
    {
            emit ((regSig->sub)->*(regSig->sp))();
    }
}

void EventManager::call_trigger(enum SignalType st, const QVector<RegisteredSignal*>& regSig_vec)
{
    foreach(RegisteredSignal* regSig, regSig_vec)
    {
        if(regSig->st == st)
        {
            emit ((regSig->sub)->*(regSig->sp))();
        }
    }
}

RegisteredSignal* EventManager::get_signal(const QString& name) const
{
    if(name == "")
    {
        return nullptr;
    }

    foreach(RegisteredSignal* regSig, regSig_vec)
    {
        if(regSig->name == name)
        {
            return regSig;
        }
    }

    return nullptr;
}

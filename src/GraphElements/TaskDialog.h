#ifndef TASKDIALOG_H
#define TASKDIALOG_H

/*
 * Author: Mattis Jaksch
 *
 * Dialog to present signals for
 * individual task in the sequencer
 *
*/

#include "src/Manager/EventManager.h"

#include <QDialog>
#include <QRadioButton>

class TaskDialog : public QDialog
{
Q_OBJECT

public:
    TaskDialog(const QVector<RegisteredSignal*>& signal_list);
    virtual ~TaskDialog();

    RegisteredSignal* get_signal() const {return selectedSignal;}

private slots:
    void set_signal(RegisteredSignal* sig) {selectedSignal = sig;}

private:
    QButtonGroup* buttonGroup = nullptr;

    QVector<RegisteredSignal*> signal_list;
    RegisteredSignal* selectedSignal;

    void create_layout();
};

class TaskRadioButton : public QRadioButton
{
Q_OBJECT

public:
    TaskRadioButton(RegisteredSignal* sig, QWidget* parent = 0)
        : QRadioButton(parent), sig(sig)
    {
        this->setText(sig->name);
        connect(this, SIGNAL(clicked()), this, SLOT(announce_signal()));
    }

signals:
    void toggled(RegisteredSignal*);

private slots:
    void announce_signal()
    {
        emit toggled(sig);
    }

private:
    RegisteredSignal *sig;
};

#endif // TASKDIALOG_H

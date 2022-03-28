#ifndef SIGNALDIALOG_H
#define SIGNALDIALOG_H

/*
 * Author: Mattis Jaksch
 *
 * Dialog to add signals to an element
 * these signals will be triggered under certain conditions
 * leading to actions in the choosen elements
 *
 */

class QCheckBox;

#include "src/Manager/EventManager.h"

#include <QDialog>

struct RegisteredSignalBox {
    QCheckBox* checkBox;

    RegisteredSignal* regSig;
};

class SpecSignalDialog : public QDialog {
Q_OBJECT

public:
    SpecSignalDialog();
    virtual ~SpecSignalDialog();

    QVector<struct RegisteredSignalBox> get_regSigBox_vec() const
    {
        return regSigBox_vec;
    }

    void add_entry(bool isChecked, struct RegisteredSignal *sig);
    void create_dialog();

private:
    QVector<RegisteredSignalBox> regSigBox_vec;
};

#endif // SIGNALDIALOG_H

#ifndef SPECSIGNALDIALOG_H
#define SPECSIGNALDIALOG_H

/*
 * Author: Mattis Jaksch
 *
 * Dialog to add signals to an element
 * these signals will be triggered under certain conditions
 * leading to actions in the choosen elements
 *
 */

class QCheckBox;
class QPushButton;

#include "src/Manager/EventManager.h"

#include <QDialog>

struct RegisteredSignalBox {
    QCheckBox *checkBox;

    struct RegisteredSignal *sig;
};

class SpecSignalDialog : public QDialog {
Q_OBJECT

public:
    SpecSignalDialog();
    virtual ~SpecSignalDialog();

    QVector<struct RegisteredSignalBox> get_registeredSignal_list() const {return registeredSignal_list;}

    void add_entry(bool isChecked, struct RegisteredSignal *sig);
    void create_dialog();

private:
    QVector<struct RegisteredSignalBox> registeredSignal_list;
};

#endif // SPECSIGNALDIALOG_H

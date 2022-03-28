#include "TaskDialog.h"

#include <QButtonGroup>
#include <QPushButton>
#include <QVBoxLayout>

TaskDialog::TaskDialog(const QVector<RegisteredSignal*>& signal_list)
    : signal_list(signal_list)
{
    create_layout();
}

TaskDialog::~TaskDialog()
{
    delete buttonGroup;
}

void TaskDialog::create_layout()
{
    QVBoxLayout* mainVLayout = new QVBoxLayout;
    QVBoxLayout* signalLayout = new QVBoxLayout;

    /* Buttons and signals */
    buttonGroup = new QButtonGroup;

    foreach (RegisteredSignal* signal, signal_list)
    {
        if(signal->st != SignalType::start_log && signal->st != SignalType::stop_log)
        {
            TaskRadioButton* button = new TaskRadioButton(signal);
            connect(button, SIGNAL(toggled(RegisteredSignal*)), this, SLOT(set_signal(RegisteredSignal*)));

            buttonGroup->addButton(button);
            signalLayout->addWidget(button);
        }
    }

    /* Ok - Cancel */
    QHBoxLayout* hlayout = new QHBoxLayout;

    QPushButton* okButton = new QPushButton("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    hlayout->addWidget(okButton);

    QPushButton* cancleButton = new QPushButton("Cancel");
    connect(cancleButton, SIGNAL(clicked()), this, SLOT(reject()));
    hlayout->addWidget(cancleButton);

    mainVLayout->addLayout(signalLayout);
    mainVLayout->addLayout(hlayout);

    setLayout(mainVLayout);
}

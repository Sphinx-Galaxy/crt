#include "SpecSignalDialog.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

SpecSignalDialog::SpecSignalDialog() {}

SpecSignalDialog::~SpecSignalDialog() {}

void SpecSignalDialog::create_dialog() {
    QVBoxLayout * buttonLayout = new QVBoxLayout;

    for(QVector<struct RegisteredSignalBox>::iterator i = registeredSignal_list.begin(); i != registeredSignal_list.end(); i++)
        buttonLayout->addWidget((*i).checkBox);

    if(registeredSignal_list.empty())
        buttonLayout->addWidget(new QLabel("No Signals available!"));

    QHBoxLayout * hlayout = new QHBoxLayout;

    QPushButton *okButton = new QPushButton("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    hlayout->addWidget(okButton);

    QPushButton *cancleButton = new QPushButton("Cancel");
    connect(cancleButton, SIGNAL(clicked()), this, SLOT(reject()));
    hlayout->addWidget(cancleButton);

    QVBoxLayout * vlayout = new QVBoxLayout(this);
    vlayout->addLayout(buttonLayout);
    vlayout->addLayout(hlayout);

    setLayout(vlayout);
}

void SpecSignalDialog::add_entry(bool isChecked, struct RegisteredSignal *sig) {
    registeredSignal_list.push_back({new QCheckBox(sig->name, this), sig});
    registeredSignal_list[registeredSignal_list.size()-1].checkBox->setCheckState((isChecked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked));
}

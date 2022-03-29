#include "SignalDialog.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

SpecSignalDialog::SpecSignalDialog() {}

SpecSignalDialog::~SpecSignalDialog() {}

void SpecSignalDialog::create_dialog() {
    QVBoxLayout* buttonLayout = new QVBoxLayout;

    foreach(RegisteredSignalBox regSigBox, regSigBox_vec)
    {
        buttonLayout->addWidget(regSigBox.checkBox);
    }

    if(regSigBox_vec.empty())
    {
        buttonLayout->addWidget(new QLabel("No Signals available!"));
    }

    QHBoxLayout* hlayout = new QHBoxLayout;

    QPushButton* okButton = new QPushButton("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    hlayout->addWidget(okButton);

    QPushButton* cancleButton = new QPushButton("Cancel");
    connect(cancleButton, SIGNAL(clicked()), this, SLOT(reject()));
    hlayout->addWidget(cancleButton);

    QVBoxLayout* vlayout = new QVBoxLayout;
    vlayout->addLayout(buttonLayout);
    vlayout->addLayout(hlayout);

    setLayout(vlayout);
}

void SpecSignalDialog::add_entry(bool isChecked, RegisteredSignal* regSig)
{
    regSigBox_vec.push_back({new QCheckBox(regSig->name, this), regSig});

    regSigBox_vec[regSigBox_vec.size()-1].checkBox->setCheckState(
                (isChecked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked));
}

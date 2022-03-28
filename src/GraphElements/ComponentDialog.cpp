#include "ComponentDialog.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

SpecAddDialog::SpecAddDialog() {}

SpecAddDialog::~SpecAddDialog() {}

void SpecAddDialog::create_dialog() {
    QFormLayout * formLayout = new QFormLayout;

    for(QVector<struct LineEntry>::iterator i = line_entry_list.begin(); i != line_entry_list.end(); i++)
        formLayout->addRow((*i).display_name, (*i).input_value);

    QHBoxLayout* hlayout = new QHBoxLayout;

    QPushButton *okButton = new QPushButton("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    hlayout->addWidget(okButton);

    QPushButton *cancleButton = new QPushButton("Cancel");
    connect(cancleButton, SIGNAL(clicked()), this, SLOT(reject()));
    hlayout->addWidget(cancleButton);

    QVBoxLayout* vlayout = new QVBoxLayout(this);
    vlayout->addLayout(formLayout);
    vlayout->addLayout(hlayout);

    setLayout(vlayout);
}

void SpecAddDialog::add_entry(const QString &display_name) {
    line_entry_list.push_back({display_name, new QLineEdit});
}

void SpecAddDialog::add_entry(const QString &display_name, const QString &standard_value) {
    QLineEdit * entry = new QLineEdit(standard_value);
    line_entry_list.push_back({display_name, entry});
}

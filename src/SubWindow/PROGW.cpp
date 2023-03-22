#include "PROGW.h"

#include "src/Components/ProgrammStarter.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QTextEdit>
#include <QSpinBox>
#include <QVBoxLayout>

PROGW::PROGW(RunManager* m_runManager, ProgrammStarter* programmStarter)
    : SubWindow(m_runManager, programmStarter), programmStarter(programmStarter)
{
    /* Connect and register signals */
    connect(this, SIGNAL(signal_on()), programmStarter, SLOT(start_programm()));
    eventManager->add_signal(programmStarter->get_element_name() + " Execute", SignalType::on, this, &SubWindow::signal_on);

    connect(this, SIGNAL(signal_off()), programmStarter, SLOT(stop_programm()));
    eventManager->add_signal(programmStarter->get_element_name() + " Kill", SignalType::off, this, &SubWindow::signal_off);
}

PROGW::~PROGW()
{
    eventManager->delete_signal(this, &SubWindow::signal_on);
    eventManager->delete_signal(this, &SubWindow::signal_off);

    delete programmStarter;
}

void PROGW::create_layout()
{
    QVBoxLayout* mainVLayout = new QVBoxLayout;

    QHBoxLayout* optionLayout = new QHBoxLayout;

    /* Trigger */
    QCheckBox* trigger = new QCheckBox("Auto");
    trigger->setToolTip("Start on 'start-test' or by sequencer");
    connect(trigger, SIGNAL(stateChanged(int)), programmStarter, SLOT(set_trigger(int)));
    connect(programmStarter, SIGNAL(announce_shouldrun(bool)), trigger, SLOT(setDisabled(bool)));

    auto restartCheckbox = new QCheckBox("Restart");
    restartCheckbox->setChecked(true);
    restartCheckbox->setToolTip("Restart program automatically after 10 seconds.");
    connect(restartCheckbox, &QCheckBox::stateChanged, programmStarter, &ProgrammStarter::set_restart);
    connect(programmStarter, &ProgrammStarter::announce_shouldrun, restartCheckbox, &QCheckBox::setDisabled);

    auto restartWaitInput = new QSpinBox(this);
    restartWaitInput->setValue(programmStarter->get_restartwait());
    restartWaitInput->setMinimum(0);
    restartWaitInput->setToolTip("Restart delay in seconds");
    restartWaitInput->setSuffix(" s");
    restartWaitInput->setMinimumWidth(75);
    connect(restartCheckbox, &QCheckBox::stateChanged, restartWaitInput, &QSpinBox::setVisible);
    connect(restartWaitInput, QOverload<int>::of(&QSpinBox::valueChanged), programmStarter, &ProgrammStarter::set_restartwait);

    /* Permanent Logging */
    QCheckBox* permanentLogging = new QCheckBox("Permanent Logging");
    permanentLogging->setDisabled(!runManager->is_valid());
    optionLayout->addWidget(permanentLogging);
    connect(permanentLogging, SIGNAL(stateChanged(int)), programmStarter, SLOT(set_permanent_logging(int)));
    connect(runManager, SIGNAL(isInvalid_changed(bool)), permanentLogging, SLOT(setDisabled(bool)));

    /* Start / Stop */
    QPushButton* startButton = new QPushButton("Start");
    connect(startButton, SIGNAL(clicked()), programmStarter, SLOT(execute_programm()));
    connect(programmStarter, SIGNAL(announce_trigger(bool)), startButton, SLOT(setDisabled(bool)));
    connect(programmStarter, SIGNAL(announce_shouldrun(bool)), startButton, SLOT(setDisabled(bool)));

    QPushButton* stopButton = new QPushButton("Stop");
    connect(stopButton, SIGNAL(clicked()), programmStarter, SLOT(kill_programm()));
    connect(programmStarter, SIGNAL(announce_trigger(bool)), stopButton, SLOT(setDisabled(bool)));

    /* Spacer */
    QSpacerItem* space = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    optionLayout->addWidget(trigger);
    optionLayout->addWidget(restartCheckbox);
    optionLayout->addWidget(restartWaitInput);
    optionLayout->addWidget(startButton);
    optionLayout->addWidget(stopButton);
    optionLayout->addSpacerItem(space);

    /* Second Row */
    QHBoxLayout* pathLayout = new QHBoxLayout;

    /* Programmpath */
    QPushButton* setPath = new QPushButton("New Path");
    connect(setPath, SIGNAL(clicked()), this, SLOT(path_dialog()));
    connect(programmStarter, SIGNAL(announce_run(bool)), setPath, SLOT(setDisabled(bool)));

    QLineEdit* programmPath = new QLineEdit;
    QPalette* grey = new QPalette;
    grey->setColor(QPalette::Base, Qt::lightGray);
    grey->setColor(QPalette::Text, Qt::black);
    programmPath->setText(programmStarter->get_path());
    programmPath->setReadOnly(true);
    programmPath->setPalette(*grey);
    connect(programmStarter, SIGNAL(announce_path(const QString&)), programmPath, SLOT(setText(const QString&)));

    /* Arguments */
    QLineEdit* arguments = new QLineEdit;
    arguments->setText(programmStarter->get_arguments());
    connect(arguments, SIGNAL(textChanged(const QString&)), programmStarter, SLOT(set_arguments(const QString&)));
    connect(programmStarter, SIGNAL(announce_run(bool)), arguments, SLOT(setDisabled(bool)));

    pathLayout->addWidget(setPath);
    pathLayout->addWidget(programmPath);
    pathLayout->addWidget(arguments);

    /* Console */
    QTextEdit* consoleText = new QTextEdit;
    connect(programmStarter, SIGNAL(data_available(const QString&)), consoleText, SLOT(append(const QString&)));

    mainVLayout->addLayout(optionLayout);
    mainVLayout->addLayout(pathLayout);
    mainVLayout->addWidget(consoleText);

    setLayout(mainVLayout);

    emit layout_done();
}

void PROGW::path_dialog()
{
    programmStarter->set_path(QFileDialog::getOpenFileName(this, tr("Open File"), "./"));
}

#include "PSUW.h"

#include "src/GraphElements/IndicatorBox.h"
#include "src/Components/PSU.h"
#include "src/Plot/PSUPlot.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

PSUW::PSUW(RunManager* m_runManager, PSU* psu)
    : SubWindow(m_runManager, psu), psu(psu)
{
    /* Connect and register signals */
    connect(this, SIGNAL(signal_on()), psu, SLOT(switch_on()));
    eventManager->add_signal(psu->get_element_name() + " Switch On", SignalType::on, this, &SubWindow::signal_on);

    connect(this, SIGNAL(signal_off()), psu, SLOT(switch_off()));
    eventManager->add_signal(psu->get_element_name() + " Switch Off", SignalType::off, this, &SubWindow::signal_off);
}

PSUW::~PSUW()
{
    /* Degregister signals */
    eventManager->delete_signal(this, &SubWindow::signal_on);
    eventManager->delete_signal(this, &SubWindow::signal_off);

    delete psu;
}

void PSUW::create_layout()
{
    QHBoxLayout* mainHLayout = new QHBoxLayout;

    foreach (PSUChannel* channel, psu->get_channel_vec()) {
        QGroupBox* channelGroupBox = new QGroupBox("Channel: " + QString::number(channel->get_number()));
        QGridLayout* psuGridLayout = new QGridLayout;

        /* Voltage */
        QLabel* voltLabel = new QLabel("[V]:");
        QLineEdit* voltLineEdit = new QLineEdit;
        voltLineEdit->setText(QString::number(channel->get_voltage_set()));
        psuGridLayout->addWidget(voltLabel, 0, 0);
        psuGridLayout->addWidget(voltLineEdit, 1, 0);
        connect(voltLineEdit, SIGNAL(editingFinished()), channel, SLOT(set_voltage()));
        connect(voltLineEdit, SIGNAL(textChanged(const QString &)), channel, SLOT(set_voltage(const QString &)));
        connect(channel, SIGNAL(voltage_changed(const QString &)), voltLineEdit, SLOT(setText(const QString &)));
        connect(psu, SIGNAL(disconnected(bool)), voltLineEdit, SLOT(setDisabled(bool)));

        /* Current */
        QLabel* currentLabel = new QLabel("[mA]:");
        QLineEdit* currentLineEdit = new QLineEdit;
        currentLineEdit->setText(QString::number(channel->get_current_set()));
        psuGridLayout->addWidget(currentLabel, 0, 1);
        psuGridLayout->addWidget(currentLineEdit, 1, 1);
        connect(currentLineEdit, SIGNAL(editingFinished()), channel, SLOT(set_current()));
        connect(currentLineEdit, SIGNAL(textChanged(const QString &)), channel, SLOT(set_current(const QString &)));
        connect(channel, SIGNAL(current_changed(const QString &)), currentLineEdit, SLOT(setText(const QString &)));
        connect(psu, SIGNAL(disconnected(bool)), currentLineEdit, SLOT(setDisabled(bool)));

        /* Enable */
        IndicatorBox* enableBox = new IndicatorBox(QIcon(":/icon/PSU_on.png"), QIcon(":/icon/PSU_off.png"), QIcon(":/icon/PSU_disconnected.png"));
        psuGridLayout->addWidget(enableBox, 0, 2);
        connect(enableBox, SIGNAL(stateChanged(int)), channel, SLOT(set_enable(int)));
        connect(channel, SIGNAL(enable_changed(bool)), enableBox, SLOT(setChecked(bool)));
        connect(psu, SIGNAL(disconnected(bool)), enableBox, SLOT(setDisabled(bool)));

        /* Trigger */
        QCheckBox* triggerBox = new QCheckBox("Auto");
        psuGridLayout->addWidget(triggerBox, 1, 2);
        connect(triggerBox, SIGNAL(stateChanged(int)), channel, SLOT(set_trigger(int)));

        /* Plot */
        QCustomPlot* plot = new QCustomPlot(this);
        PSUPlot* psuplot = new PSUPlot(plot, channel, 30, 30);
        plot->setGeometry(QRect());
        plot->setMinimumHeight(128);
        plot->setMaximumHeight(512);
        connect(psu, SIGNAL(data_available()), psuplot, SLOT(update_plot()));

        QVBoxLayout* psuVLayout = new QVBoxLayout;
        psuVLayout->addLayout(psuGridLayout);
        psuVLayout->addWidget(plot);

        channelGroupBox->setLayout(psuVLayout);
        mainHLayout->addWidget(channelGroupBox);
    }

    /* Master switch */
    if(psu->has_master_switch()) {
        QGridLayout* masterGridLayout = new QGridLayout;
        QGroupBox* masterBoxLayout = new QGroupBox("Master Switch");

        IndicatorBox* masterBox = new IndicatorBox(QIcon(":/icon/PSU_on.png"), QIcon(":/icon/PSU_off.png"), QIcon(":/icon/PSU_disconnected.png"));
        connect(masterBox, SIGNAL(stateChanged(int)), psu, SLOT(set_master_enable(int)));
        connect(psu, SIGNAL(master_changed(bool)), masterBox, SLOT(setChecked(bool)));
        connect(psu, SIGNAL(disconnected(bool)), masterBox, SLOT(setDisabled(bool)));

        QCheckBox* masterTrigger = new QCheckBox("Auto", this);
        connect(masterTrigger, SIGNAL(stateChanged(int)), psu, SLOT(set_master_trigger(int)));

        masterGridLayout->addWidget(masterBox, 0, 0);
        masterGridLayout->addWidget(masterTrigger, 1, 0);

        masterBoxLayout->setLayout(masterGridLayout);
        mainHLayout->addWidget(masterBoxLayout);
    }

    setLayout(mainHLayout);

    emit layout_done();
}

#include "SEQW.h"

#include "src/Components/Sequencer.h"

#include "src/GraphElements/IndicatorIcon.h"
#include "src/GraphElements/TaskDialog.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

SEQW::SEQW(RunManager* m_runManager, Sequencer* seq)
    : SubWindow(m_runManager, seq), seq(seq)
{
    connect(this, SIGNAL(signal_on()), seq, SLOT(start_seq()));
    eventManager->add_signal(seq->get_element_name() + " Start sequence", SignalType::on, this, &SubWindow::signal_on);

    connect(this, SIGNAL(signal_off()), seq, SLOT(stop_seq()));
    eventManager->add_signal(seq->get_element_name() + " Stop sequence", SignalType::off, this, &SubWindow::signal_off);
}

SEQW::~SEQW()
{
    /* Degregister signals */
    eventManager->delete_signal(this, &SubWindow::signal_on);
    eventManager->delete_signal(this, &SubWindow::signal_off);

    delete seq;
}

void SEQW::create_layout()
{
    QVBoxLayout* mainVLayout = new QVBoxLayout;

    /* Loop checkBox */
    QCheckBox* loopBox = new QCheckBox("Loop");
    connect(loopBox, SIGNAL(stateChanged(int)), seq, SLOT(set_loop(int)));

    /* Task layout */
    QGridLayout* taskLayout = new QGridLayout;
    QGroupBox* taskBox = new QGroupBox("Tasks:");

    /* Task header */
    taskLayout->addWidget(new QLabel(""), 0, 0);
    taskLayout->addWidget(new QLabel("#"), 0, 1);
    taskLayout->addWidget(new QLabel("Signal"), 0, 2);
    taskLayout->addWidget(new QLabel("Delay [s]"), 0, 3);
    taskLayout->addWidget(new QLabel("Aktiv"), 0, 4);

    int cnt = 1;
    foreach(Task* task, seq->get_task_vec())
    {
        //Signal button
        QPushButton* signalButton = new QPushButton;
        signalButton->setText("Set Signal");
        connect(signalButton, SIGNAL(clicked()), task, SLOT(signal_button_clicked()));
        connect(task, SIGNAL(update_task(Task *)), this, SLOT(set_task(Task *)));

        //Number
        QLabel* numberLabel = new QLabel(QString::number(task->get_number()));

        //Signal name
        QLineEdit* nameEdit = new QLineEdit;
        nameEdit->setReadOnly(true);
        nameEdit->setText(task->get_signal_name());
        connect(task, SIGNAL(signal_name_changed(const QString &)), nameEdit, SLOT(setText(const QString &)));

        //Sleep
        QLineEdit* timeEdit = new QLineEdit;
        QIntValidator* timeValid = new QIntValidator(1, 60*60*24);
        timeEdit->setValidator(timeValid);
        timeEdit->setText(QString::number(task->get_time()));
        connect(timeEdit, SIGNAL(textChanged(const QString&)), task, SLOT(set_time(const QString &)));

        //Aktiv
        IndicatorIcon* activeIndicator = new IndicatorIcon("<Icon>", QPixmap(":/icon/task_active.png"), QPixmap(":/icon/task_inactive.png"), QSize(24, 24));
        activeIndicator->set_status(task->is_active());
        connect(task, SIGNAL(status_changed(int)), activeIndicator, SLOT(set_status(int)));

        taskLayout->addWidget(signalButton, cnt, 0);
        taskLayout->addWidget(numberLabel, cnt, 1);
        taskLayout->addWidget(nameEdit, cnt, 2);
        taskLayout->addWidget(timeEdit, cnt, 3);
        taskLayout->addWidget(activeIndicator, cnt, 4);
        cnt++;
    }

    taskBox->setLayout(taskLayout);

    mainVLayout->addWidget(loopBox);
    mainVLayout->addWidget(taskBox);

    setLayout(mainVLayout);

    emit layout_done();
}

void SEQW::set_task(Task* task)
{
    //Create dialog
    TaskDialog dia(eventManager->get_signal_vec());

    if(dia.exec())
    {
        task->set_signal(dia.get_signal());
    }
}

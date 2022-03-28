#include "RunManager.h"

#include "EventManager.h"

#include <QElapsedTimer>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

RunManager::RunManager(EventManager* eventManager)
    : eventManager(eventManager)
{
    runTime = new QElapsedTimer;
    offsetTime = 0;

    updateTime = new QTimer;
    updateTime->start(1000);
    connect(updateTime, SIGNAL(timeout()), this, SLOT(update_run()));

    qRegisterMetaType<RunMode>("RunMode");

    create_layout();
}

void RunManager::create_layout()
{
    QHBoxLayout* mainLayout = new QHBoxLayout;

    /* Color palette */
    QPalette* grey = new QPalette;
    grey->setColor(QPalette::Base,Qt::lightGray);
    grey->setColor(QPalette::Text,Qt::black);

    /* Button to create a new run */
    QPushButton* runButton = new QPushButton("New Run");
    connect(runButton, SIGNAL(clicked()), this, SLOT(create_new_run()));
    connect(this, SIGNAL(enable_runButton(bool)), runButton, SLOT(setEnabled(bool)));

    /* Field for run folder */
    QLineEdit* runNameLine = new QLineEdit("<NAME>");
    runNameLine->setReadOnly(true);
    runNameLine->setPalette(*grey);
    connect(this, SIGNAL(run_name_changed(const QString&)), runNameLine, SLOT(setText(const QString&)));

    /* Current run time */
    QLineEdit* runTimeLine = new QLineEdit("000:00:00");
    runTimeLine->setReadOnly(true);
    runTimeLine->setPalette(*grey);
    connect(this, SIGNAL(run_time_changed(const QString&)), runTimeLine, SLOT(setText(const QString&)));

    mainLayout->addWidget(runButton);
    mainLayout->addWidget(runNameLine);
    mainLayout->addWidget(runTimeLine);

    setLayout(mainLayout);
}

void RunManager::create_new_run()
{
    QString folder = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "./");

    //If run is valid, first deregister the RunManager
    if(valid)
    {
        deregister_component(this);
    }

    /* Check if the selected folder exists */
    if(QFileInfo::exists(folder))
    {
        set_root_directory(folder);

        /* Create new run log for start / stop / various */
        register_component(this, "RunManager");
        set_file_header(this, {"Action", "Component"});
        set_run_mode(RunMode::Creation, "Run Manager");

        offsetTime = 0;

        emit run_name_changed(folder);
        set_run_valid(true);
    }
    else
    {
        set_run_valid(false);
    }
}

void RunManager::update_run()
{
    long long time = runTime->isValid() ? runTime->elapsed() / 1000 : 0;
    time += offsetTime;
    uint hour, min, sec;

    sec = time%60;
    time = (time - time%60) / 60;
    min = time%60;
    time = (time - time%60) / 60;
    hour = uint(time);

    QString text = QString("%1:%2:%3").arg(hour, 3, 10, QChar('0')).arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
    emit run_time_changed(text);
}

void RunManager::start_run()
{
    if(!running)
    {
        emit enable_runButton(false);

        runTime->start();
        running = true;

        eventManager->trigger_start_log();
        eventManager->trigger_on();

        set_run_mode(RunMode::StartRun, "Run Manager");
    }
}

void RunManager::stop_run()
{
    if(running)
    {
        offsetTime += runTime->isValid() ? runTime->elapsed() / 1000 : 0;
        runTime->invalidate();
        running = false;

        eventManager->trigger_off();
        eventManager->trigger_stop_log();

        set_run_mode(RunMode::StopRun, "Run Manager");

        emit enable_runButton(true);
    }
}

void RunManager::set_run_mode(enum RunMode mode, const QString& component) {
    if(valid)
    {
        append_values_to_file(this, {QString::number(mode), component});
    }

    emit run_mode_changed(mode);
}

void RunManager::set_run_valid(bool valid)
{
    this->valid = valid;

    emit isValid_changed(valid);
    emit isInvalid_changed(!valid);
}

#include "ProgrammStarter.h"

#include <QFileDialog>
#include <QTimer>
#include <QVariant>

ProgrammStarter::ProgrammStarter(RunManager* runManager, const QString &config)
    : Component(runManager, config)
{
    load_config(config);
    assert(parse_config({"name", "path"}));

    this->elementName = get_value("name");
    path = get_value("path");
    set_arguments(get_value("arguments"));
}

ProgrammStarter::ProgrammStarter(RunManager* runManager, const QString& m_element_name, const QString& path)
    : Component(runManager, m_element_name), path(path)
{
    this->elementName = m_element_name;

    arguments = QStringList("$directory");
}

ProgrammStarter::~ProgrammStarter()
{
    delete process;
}

void ProgrammStarter::set_config()
{
    config_entry_vec.clear();

    set_value("name", elementName);
    set_value("path", path);
    set_value("arguments", get_arguments());
}

void ProgrammStarter::init()
{
    process = new QProcess;

    connect(process, SIGNAL(readyRead()), this, SLOT(receive_data()));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handle_finished_process()));
    connect(process, &QProcess::started, this, &ProgrammStarter::handle_started_process);
    connect(process, &QProcess::errorOccurred, this, &ProgrammStarter::handle_error_process);

    timer_restart = new QTimer(this);
    timer_restart->setSingleShot(true);
    connect(timer_restart, &QTimer::timeout, this, &ProgrammStarter::execute_programm);


    emit init_done();
}

void ProgrammStarter::set_path(const QString& text)
{
    if (!shouldrun && QFileInfo::exists(text))
    {
        path = text;
        emit announce_path(path);
    }
}

void ProgrammStarter::set_trigger(int trigger)
{
    if (!shouldrun)
    {
        this->trigger = trigger > 0 ? true : false;

        emit announce_trigger(this->trigger);
    }
}

void ProgrammStarter::execute_programm()
{
    if (process->state() == QProcess::ProcessState::NotRunning) {

        qDebug("execute_programm `%s %s` ",
            path.toLocal8Bit().data(),
            substitute_arguments().join(" ").toLocal8Bit().data());

        start_logging();

        process->start(path, substitute_arguments());

        shouldrun = true;
        emit announce_shouldrun(true);
    } else {
        qDebug("execute_programm already running");
    }
}

void ProgrammStarter::kill_programm()
{
    timer_restart->stop();

    if (shouldrun) {
        process->kill();
        stop_logging();

        shouldrun = false;
        emit announce_shouldrun(false);
    }
}

void ProgrammStarter::handle_started_process()
{
    qDebug("handle_started_process");

    emit announce_run(true);
}

void ProgrammStarter::handle_error_process(QProcess::ProcessError error)
{
    QString errstr = QVariant::fromValue(error).toString();
    qDebug("handle_error_process %d : %s", error, errstr.toLocal8Bit().data());

    emit data_available(QString("\nerror %1 : %2\n").arg(error).arg(errstr));

    kill_programm();
}

void ProgrammStarter::handle_finished_process()
{
    qDebug("handle_finished_process");


    receive_data();

    emit data_available(QString("\nexited with code %1\n").arg(process->exitCode()));

    //Stop process (once and for all)
    process->kill();
    stop_logging();

    emit announce_run(false);

    if ((shouldrun) && (restart)) {
        emit announce_shouldrun(true);
        timer_restart->start(10000);
    }
}

void ProgrammStarter::receive_data()
{
    QString text = process->readAll();

    if(text.size() > 1)
    {
        if(text.back() == '\n')
        {
            text.remove(text.size()-1, 1);
        }
        else if(text.back() == '\r')
        {
            text.remove(text.size()-1, 1);
        }
    }

    if(logging || permanent_logging)
    {
        runManager->append_value_to_file(this, text);
    }

    emit data_available(text);
}

QStringList ProgrammStarter::substitute_arguments()
{
    QStringList argList;

    foreach (QString element, arguments)
    {
        if(element.contains("$directory"))
        {
            QString suffix = element.mid(element.indexOf("/"));
            argList.push_back(runManager->get_root_directory() + suffix);
        }
        else
        {
            argList.push_back(element);
        }
    }

    return argList;
}

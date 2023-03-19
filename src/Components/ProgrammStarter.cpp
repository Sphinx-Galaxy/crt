#include "ProgrammStarter.h"

#include <QFileDialog>
#include <QProcess>
#include <QThread>

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

    emit init_done();
}

void ProgrammStarter::set_path(const QString& text)
{
    if(!running && QFileInfo::exists(text))
    {
        path = text;
        emit announce_path(path);
    }
}

void ProgrammStarter::set_trigger(int trigger)
{
    if(!running)
    {
        this->trigger = trigger > 0 ? true : false;

        emit announce_trigger(this->trigger);
    }
}

void ProgrammStarter::execute_programm()
{
    if(!running)
    {
        process->start(path, substitute_arguments());
        process->waitForStarted();

        running = true;

        emit announce_run(true);
    }
}

void ProgrammStarter::kill_programm()
{
    if(running)
    {
        running = false;

        process->kill();

        stop_logging();

        emit announce_run(false);
    }
}

void ProgrammStarter::handle_finished_process()
{
    if(running)
    {
        //Stop process (once and for all)
        kill_programm();

        //Wait
        QThread::sleep(30);

        //Start process and logging (again)
        execute_programm();
        start_logging();
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

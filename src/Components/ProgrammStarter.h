#ifndef PROGRAMMSTARTER_H
#define PROGRAMMSTARTER_H

/*
 * Author: Mattis Jaksch
 *
 * A path to an exection program can be specified;
 * The output is logged into a file
 *
 */

#include <QProcess>
class QTimer;

#include "Component.h"

class ProgrammStarter : public Component
{
Q_OBJECT

public:
    ProgrammStarter(RunManager* runManager, const QString& config);
    ProgrammStarter(RunManager* runManager, const QString& m_element_name, const QString& path);
    virtual ~ProgrammStarter();

    QString get_path() const
    {
        return path;
    }

    QString get_arguments();

    void set_config() override;

public slots:
    void init() override;
    void update() override {} //Mix with receive data

    void set_arguments(const QString &text);
    void set_path(const QString &text);
    void set_trigger(int trigger);

    void start_programm();
    void stop_programm();

    void execute_programm();
    void kill_programm();

private slots:
    void receive_data();
    void handle_started_process();
    void handle_finished_process();
    void handle_error_process(QProcess::ProcessError error);

signals:
    void data_available(const QString &);
    void started();
    void stopped();

    void announce_trigger(bool);
    void announce_run(bool);
    void announce_shouldrun(bool);
    void announce_path(const QString &);

private:
    QProcess* process;
    QTimer *timer_restart;

    QString path;
    QStringList arguments;
    bool shouldrun = false;
    bool restart = true;
    bool trigger = false;

    QStringList generate_header() override;

    QStringList substitute_arguments();
};

inline QString ProgrammStarter::get_arguments()
{
    return arguments.join(' ');
}

inline QStringList ProgrammStarter::generate_header()
{
    return {"Line"};
}

inline void ProgrammStarter::set_arguments(const QString& text)
{
    arguments = text.split(' ');
}

inline void ProgrammStarter::start_programm()
{
    if(trigger)
    {
        execute_programm();
    }
}

inline void ProgrammStarter::stop_programm()
{
    if(trigger)
    {
        kill_programm();
    }
}

#endif // PROGRAMMSTARTER_H

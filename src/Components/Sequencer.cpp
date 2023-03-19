#include "Sequencer.h"

Sequencer::Sequencer(RunManager* runManager, const QString& config)
    : Component(runManager, config)
{
    load_config(config);
    assert(parse_config({"tasks"}));

    this->task_number = get_value("tasks").toInt();
}

Sequencer::Sequencer(RunManager* runManager, const QString& m_element_name, int task_number)
    : Component(runManager, m_element_name), task_number(task_number) {}

Sequencer::~Sequencer()
{
    foreach (Task *task, task_vec)
    {
        delete task;
    }
}

void Sequencer::set_config()
{
    set_value("name", elementName);
    set_value("tasks", QString::number(task_vec.size()));

    for(int i = 0; i < task_vec.size(); ++i) {
        set_value("t" + QString::number(i) + "t", QString::number(task_vec[i]->get_time()));
        set_value("t" + QString::number(i) + "s", task_vec[i]->get_signal_name());
    }
}

void Sequencer::init()
{
    connect(runManager->get_eventManager(), SIGNAL(signal_added()), this, SLOT(update_task_vec()));
    connect(runManager->get_eventManager(), SIGNAL(signal_deleted(RegisteredSignal*)), this, SLOT(update_task_vec()));

    for(int i = 0; i < task_number; ++i)
    {
        if(is_empty())
        {
            task_vec.push_back(new Task{(uint)i, 1, "", nullptr});
        }
        else
        {
            uint time = get_value("t" + QString::number(i) + "t").toUInt();
            QString signal_name = get_value("t" + QString::number(i) + "s");
            RegisteredSignal* sig = runManager->get_eventManager()->get_signal(signal_name);

            task_vec.push_back(new Task{(uint)i, time, signal_name, sig});
        }

        task_vec.last()->set_active(false);
    }

    task_it = task_vec.begin();

    taskTimer = new QTimer;
    connect(taskTimer, SIGNAL(timeout()), this, SLOT(update()));

    emit init_done();
    logTimer->stop();
}

void Sequencer::update() {
    if(runManager->is_running() && (task_it != task_vec.end() || loop))
    {
        /* Stop timer */
        taskTimer->stop();

        /* Send signal */
        RegisteredSignal* sig = (*task_it)->get_signal();

        if(sig != nullptr)
        {
            if(logging)
            {
                runManager->append_values_to_file(this, {QString::number((*task_it)->get_number()), QString::number((*task_it)->get_time()), (*task_it)->get_signal_name()});
            }

            if(sig->sub != nullptr && sig->sp != nullptr)
            {
                emit ((sig->sub)->*(sig->sp))();
            }
        }

        //Deactivated task
        (*task_it)->set_active(false);

        /* Start next timer */
        task_it++;
        if(loop && task_it == task_vec.end())
        {
            task_it = task_vec.begin();
        }

        if(task_it != task_vec.end())
        {
            taskTimer->start((*task_it)->get_time()*1000);

            (*task_it)->set_active(true);
        }
    }
}

void Sequencer::update_task_vec()
{
    // Synchronize the task list with the available signals
    foreach (Task* task, task_vec)
    {
        task->set_signal(runManager->get_eventManager()->get_signal(task->get_signal_name()));
    }

    emit task_vec_changed();
}

void Sequencer::start_seq()
{
    task_it = task_vec.begin();

    (*task_it)->set_active(true);
    taskTimer->start((*task_it)->get_time());
}

void Sequencer::stop_seq()
{
    if(task_it != task_vec.end())
    {
        (*task_it)->set_active(false);
    }

    taskTimer->stop();
}

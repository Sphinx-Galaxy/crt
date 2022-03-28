#include "Component.h"

Component::Component(const QString& elementName, RunManager *runManager)
    : runManager(runManager), elementName(elementName)
{

    connect(this, SIGNAL(init_done()), this, SLOT(configure_timer()));

    /* RunManager info */
    runManager->set_run_mode(AddComponent, elementName);
}

Component::Component(RunManager* runManager, const QString& config)
    : runManager(runManager)
{

    connect(this, SIGNAL(init_done()), this, SLOT(configure_timer()));

    load_config(config);
    assert(parse_config({"name"}));

    elementName = get_value("name");

    /* RunManager info */
    runManager->set_run_mode(AddComponent, elementName);
}

Component::~Component()
{
    runManager->set_run_mode(RemoveComponent, elementName);
    runManager->deregister_component(this);
}

void Component::start_logging()
{
    assert(elementName.isEmpty() == false);

    if(!logging)
    {
        runManager->register_component(this, elementName);
        runManager->set_file_header(this, generate_header());

        runManager->set_run_mode(StartLog, elementName);

        logging = true;
        emit is_logging(true);
    }
}

void Component::stop_logging()
{
    if(logging && !permanent_logging)
    {
        logging = false;
        emit is_logging(false);

        runManager->deregister_component(this);

        runManager->set_run_mode(StopLog, elementName);
    }
}

void Component::set_permanent_logging(int permanent_logging)
{
    if(permanent_logging > 0)
    {
        start_logging();
        this->permanent_logging = true;
    }
    else
    {
        stop_logging();
        this->permanent_logging = false;
    }
}

void Component::configure_timer(uint time)
{
    if(logTimer == nullptr)
    {
        logTimer = new QTimer;
        connect(logTimer, SIGNAL(timeout()), this, SLOT(update()));
    }

    logTimer->start(time);
}

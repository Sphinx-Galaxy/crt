#include "Component.h"

Component::Component(RunManager* runManager, const QString& config)
    : runManager(runManager)
{

    connect(this, SIGNAL(init_done()), this, SLOT(configure_timer()));

    load_config(config);
    elementName = get_value("name");
}

Component::~Component()
{
    runManager->deregister_component(this);
}

void Component::start_logging()
{
    if(!logging)
    {
        runManager->register_component(this);
        runManager->set_file_header(this, generate_header());

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
        this->permanent_logging = false;
        stop_logging();
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

#include "OSC.h"

#include "EthernetClient.h"

OSC::OSC(RunManager* runManager, const QString &config)
    : Component(runManager, config)
{
    load_config(config);
    assert(parse_config({"name", "vendor", "master", "address", "channel"}));

    this->address = get_value("address");
    this->channel_max = get_value("channel").toUInt();
    this->vendor = check_vendor(get_value("vendor"));

    update_settings_vendor = select_update_settings_vendor();

    connect(this, SIGNAL(settings_changed()), this, SLOT(update_settings()));
}

OSC::OSC(RunManager* runManager,
         const QString& m_element_name,
         const QString& address,
         const QString& vendor,
         uint channel_max)
    : Component(runManager,
                m_element_name),
                address(address),
                channel_max(channel_max)
{

    this->elementName = m_element_name;
    this->vendor = check_vendor(vendor);

    update_settings_vendor = select_update_settings_vendor();
}

OSC::~OSC() {
    foreach(OSCChannel *channel, channel_vec)
    {
        delete channel;
    }

    delete eth;
}

void OSC::set_config() {
    clear_config();

    set_value("name", elementName);
    set_value("address", address);
    set_value("channel", QString::number(channel_vec.size()));
}

void OSC::init() {
    /* Setup ethernet */
    QString ip_address = address.mid(0, address.indexOf(":"));
    uint port = (address.mid(address.indexOf(":") + 1, address.size() - address.indexOf(":") - 1)).toUInt();

    eth = new EthernetClient(port, ip_address);


    /* Setup channel */
    for(uint8_t i = 0; i < channel_max; ++i)
    {
        channel_vec.push_back(new OSCChannel(i, eth, vendor));
    }

    /* Read values and settings */
    update_settings();

    emit init_done();

    logTimer->start(10);
}

enum OSCChannel::Vendor OSC::check_vendor(const QString& vendor)
{
    enum OSCChannel::Vendor vd = OSCChannel::Vendor::none;

    if(vendor.contains("rigol", Qt::CaseInsensitive))
    {
        vd = OSCChannel::Vendor::rigol;
    }

    return vd;
}

QString OSC::check_vendor(enum OSCChannel::Vendor vendor)
{
    QString res = "none";

    if(vendor == OSCChannel::Vendor::rigol)
    {
        res = "rigol";
    }

    return res;
}

void (OSC::*OSC::select_update_settings_vendor())(void)
{
    switch(vendor)
    {
        case OSCChannel::Vendor::rigol:
            return &OSC::update_settings_rigol;
        case OSCChannel::Vendor::none:
            return &OSC::update_settings_none;
    }
}

void OSC::update()
{
    //Check if the network connection is ok
    if(check_network_connection())
    {

        /* Gather data */
        QVector<QVector<double>> values;
        values.reserve(channel_vec.size());
        foreach(OSCChannel* channel, channel_vec)
        {
            channel->update_values();
            values.push_back(channel->get_values());
        }

        if(logging)
        {
            runManager->append_values_to_file(this, values);
        }
    }

    emit data_available();
}

void OSC::update_settings()
{
    (this->*update_settings_vendor)();
}

void OSC::update_settings_rigol()
{
    eth->write(running ? ":RUN" : ":STOP");

    switch(triggerMode)
    {
        case TriggerMode::Auto:
            eth->write(":TRIGGER:SWEEP AUTO");
            break;
        case TriggerMode::Normal:
            eth->write(":TRIGGER:SWEEP NORMAL");
            break;
        case TriggerMode::Single:
            eth->write(":TRIGGER:SWEEP SINGLE");
            break;
    }

    eth->write(":TRIGGER:EDGE:SOURCE CHANNEL" + QString::number(trigger_channel));

    eth->write(":TIMEBASE[:MAIN]:SCALE " + QString::number(timebase));
}

void OSC::update_settings_none()
{

}

void OSC::set_running(bool running)
{
    this->running = running;

    emit(settings_changed());
}

void OSC::set_trigger_channel(uint channel_number)
{
    this->trigger_channel = channel_number;

    emit(settings_changed());
}

void OSC::set_trigger_mode(enum TriggerMode triggerMode)
{
    this->triggerMode = triggerMode;

    emit(settings_changed());
}

void OSC::set_timebase(double timebase)
{
    this->timebase = timebase;

    emit(settings_changed());
}

bool OSC::check_network_connection()
{
#ifndef DUMMY_DATA
    if(!eth->is_connected()) {
        emit disconnected(true);
        return false;
    }
#endif

    emit disconnected(false);
    return true;
}

QStringList OSC::generate_header()
{
    QStringList header;

    foreach(OSCChannel* channel, channel_vec)
    {
        header.push_back("Ch" + QString::number(channel->get_number()));
    }

    return header;
}

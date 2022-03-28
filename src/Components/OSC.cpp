#include "OSC.h"

#include "EthernetClient.h"

OSC::OSC(RunManager * runManager, const QString &config)
    : Component(runManager, config) {

    load_config(config);
    assert(parse_config({"name", "vendor", "master", "address", "channel"}));

    this->address = get_value("address");
    this->channel_max = get_value("channel").toUInt();
}

OSC::OSC(RunManager * runManager, const QString &m_element_name, const QString &address, const QString &vendor, uint channel_max)
    : Component(m_element_name, runManager), address(address), channel_max(channel_max) {

    this->elementName = m_element_name;
}

OSC::~OSC() {
    delete eth;

    foreach (OSCChannel *channel, channel_list)
        delete channel;
}

void OSC::set_config() {
    config_entry_list.clear();

    set_value("name", elementName);
    set_value("address", address);
    set_value("channel", QString::number(channel_list.size()));
}

void OSC::update() {
    //Check if the network connection is ok
    if(check_network_connection()) {

        /* Gather data */
        QVector<double> values;
        values.reserve(channel_list.size()*2);
        foreach (OSCChannel * channel, channel_list) {
//            channel->meas_voltage();
//            channel->meas_current();
//            values.push_back(channel->get_voltage_meas());
//            values.push_back(channel->get_current_meas());
        }

        if(logging)
            runManager->append_values_to_file(this, values);
    }

    emit data_available();
}

void OSC::init() {
    /* Setup ethernet */
    QString ip_address = address.mid(0, address.indexOf(":"));
    uint port = (address.mid(address.indexOf(":") + 1, address.size() - address.indexOf(":") - 1)).toUInt();

    eth = new EthernetClient(port, ip_address);

    /* Setup channel */

    /* Read values and settings */
    update_settings();

    emit init_done();

    logTimer->start(10);
}

void OSC::switch_on() {

}

void OSC::switch_off() {

}

bool OSC::check_network_connection() {
#ifndef DUMMY_DATA
    if(!eth->is_connected()) {
        emit disconnected(true);
        return false;
    }
#endif

    emit disconnected(false);
    return true;
}

QStringList OSC::generate_header() {
    QStringList header;

    foreach (OSCChannel * channel, channel_list) {
        header.push_back("V" + QString::number(channel->get_number()));
        header.push_back("mA" + QString::number(channel->get_number()));
    }

    return header;
}

void OSC::set_running(bool running) {

}

void OSC::set_trigger_channel(uint channel_number) {

}

void OSC::set_trigger_mode(enum TriggerMode triggerMode) {

}

void OSC::set_timebase(double timebase) {

}

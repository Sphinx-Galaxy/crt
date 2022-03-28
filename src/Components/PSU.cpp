#include "PSU.h"

#include "LXIClient.h"

PSU::PSU(RunManager* runManager, const QString &config)
    : Component(runManager, config)
{
    load_config(config);
    assert(parse_config({"name", "vendor", "master", "address", "channel"}));

    this->elementName = get_value("name");
    this->vd = check_vendor(get_value("vendor"));
    this->master_switch = get_value("master").toInt();
    this->address = get_value("address");
    this->channel_max = get_value("channel").toUInt();
}

PSU::PSU(RunManager* runManager,
         const QString& m_element_name,
         const QString& address,
         const QString& vendor,
         uint channel_max,
         double voltage_max,
         double current_max)
    : Component(m_element_name, runManager),
      address(address),
      channel_max(channel_max),
      voltage_max(voltage_max),
      current_max(current_max)
{

    this->elementName = m_element_name;
    this->vd = check_vendor(vendor);

    if(vd == PSUChannel::rohdeSchwarz) // || vendor)
    {
        master_switch = true;
    }
}

PSU::~PSU()
{
    foreach (PSUChannel* channel, channel_list)
    {
        delete channel;
    }

    //delete lxi;
}

void PSU::set_config()
{
    config_entry_list.clear();

    set_value("name", elementName);
    set_value("vendor", check_vendor(vd));
    set_value("master", QString::number(master_switch));
    set_value("address", address);
    set_value("channel", QString::number(channel_list.size()));

    for(int i = 0; i < channel_list.size(); i++)
    {
        set_value("c" + QString::number(i) + "vs", QString::number(channel_list[i]->get_voltage_set()));
        set_value("c" + QString::number(i) + "cs", QString::number(channel_list[i]->get_current_set()));
        set_value("c" + QString::number(i) + "vm", QString::number(channel_list[i]->get_voltage_max()));
        set_value("c" + QString::number(i) + "cm", QString::number(channel_list[i]->get_current_max()));
    }
}

void PSU::update()
{
    /* Gather data */
    QVector<double> values;
    values.reserve(channel_list.size()*2);
    foreach (PSUChannel* channel, channel_list) {
        channel->meas_voltage();
        channel->meas_current();
        values.push_back(channel->get_voltage_meas());
        values.push_back(channel->get_current_meas());
    }

    if(logging)
    {
        runManager->append_values_to_file(this, values);
    }

    emit disconnected(false); // The LXI cannot tell anymore if the connection is stable
    emit data_available();
}

enum PSUChannel::vendor PSU::check_vendor(const QString& vendor)
{
    enum PSUChannel::vendor vd = PSUChannel::none;

    if(vendor.contains("rohde", Qt::CaseInsensitive) ||
            vendor.contains("schwarz", Qt::CaseInsensitive))
    {
        vd = PSUChannel::rohdeSchwarz;
    }
    else if(vendor.contains("tti", Qt::CaseInsensitive))
    {
        vd = PSUChannel::tti;
    }

    return vd;
}

QString PSU::check_vendor(enum PSUChannel::vendor vd)
{
    QString res = "none";

    if(vd == PSUChannel::tti)
    {
        res = "tti";
    }
    else if(vd == PSUChannel::rohdeSchwarz)
    {
        res = "rohdeschwarz";
    }

    return res;
}

void PSU::init()
{
    /* Setup lxi */
#ifndef DUMMY_DATA
    QString ip_address = address.mid(0, address.indexOf(":"));
    uint port = (address.mid(address.indexOf(":") + 1, address.size() - address.indexOf(":") - 1)).toUInt();

    lxi = new LXIClient(port, ip_address);

    // Write reset
    lxi->query("RST");
#endif

    /* Setup channel */
    for(uint i = 0; i < channel_max; i++)
    {
        if(is_empty())
        {
            channel_list.push_back(new PSUChannel{i, lxi, vd, 0, 0, voltage_max, current_max});
        }
        else
        {
            assert(parse_config({"c" + QString::number(i) + "vs", "c" + QString::number(i) + "cs", "c" + QString::number(i) + "vm", "c" + QString::number(i) + "cm"}));

            double voltage_set = get_value("c" + QString::number(i) + "vs").toDouble();
            double current_set = get_value("c" + QString::number(i) + "cs").toDouble();
            double voltage_max = get_value("c" + QString::number(i) + "vm").toDouble();
            double current_max = get_value("c" + QString::number(i) + "cm").toDouble();

            channel_list.push_back(new PSUChannel{i, lxi, vd, voltage_set, current_set, voltage_max, current_max});
        }
    }

    /* Read values and settings */
    update_settings();

    emit init_done();

    logTimer->start(1000);
}

void PSU::set_master_enable(int master_enable) {
    this->master_enable = master_enable == 0 ? false : true;

#ifndef DUMMY_DATA
    if(vd == PSUChannel::rohdeSchwarz)
    {
        set_master_rohdeschwarz();
    }
//  else if(vd == vendor)
//        set_master_vendor();
#endif

    emit master_changed(this->master_enable);
}

void PSU::set_master_rohdeschwarz()
{
    lxi->query("OUTP:MAST " + QString(master_enable ? "ON" : "OFF"));
}

void PSU::switch_on()
{
    foreach (PSUChannel * channel, channel_list)
    {
        if(channel->get_trigger())
        {
            channel->set_enable(true);
        }
    }

    if(has_master_switch() && master_trigger)
    {
        set_master_enable(1);
    }
}

void PSU::switch_off() {
    if(has_master_switch() && master_trigger)
    {
        set_master_enable(0);
    }

    foreach (PSUChannel * channel, channel_list)
    {
        if(channel->get_trigger())
        {
            channel->set_enable(false);
        }
    }
}

QStringList PSU::generate_header()
{
    QStringList header;

    foreach (PSUChannel * channel, channel_list)
    {
        header.push_back("V" + QString::number(channel->get_number()));
        header.push_back("mA" + QString::number(channel->get_number()));
    }

    return header;
}

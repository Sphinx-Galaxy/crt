#include "PSU.h"

#include "LXIClient.h"

#include <float.h>

PSU::PSU(RunManager* runManager, const QString &config)
    : Component(runManager, config)
{
    load_config(config);

    this->elementName = get_value("name");
    this->vendor = check_vendor(get_value("vendor"));
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
    : Component(runManager, m_element_name),
      address(address),
      channel_max(channel_max),
      voltage_max(voltage_max),
      current_max(current_max)
{
    this->elementName = m_element_name;
    this->vendor = check_vendor(vendor);

    if(this->vendor == PSUChannel::Vendor::rohdeSchwarz) // || vendor)
    {
        master_switch = true;
    }
    else
    {
        master_switch = false;
    }
}

PSU::~PSU()
{
    foreach(PSUChannel* channel, channel_vec)
    {
        delete channel;
    }

    delete lxi;
}

void PSU::set_config()
{
    clear_config();

    set_value("name", elementName);
    set_value("vendor", check_vendor(vendor));
    set_value("master", QString::number(master_switch));
    set_value("address", address);
    set_value("channel", QString::number(channel_vec.size()));

    for(int i = 0; i < channel_vec.size(); ++i)
    {
        set_value("c" + QString::number(i) + "vs", QString::number(channel_vec[i]->get_voltage_set()));
        set_value("c" + QString::number(i) + "cs", QString::number(channel_vec[i]->get_current_set()));
        set_value("c" + QString::number(i) + "vm", QString::number(channel_vec[i]->get_voltage_max()));
        set_value("c" + QString::number(i) + "cm", QString::number(channel_vec[i]->get_current_max()));
    }
}

void PSU::update()
{
    /* Gather data */
    QVector<double> values;
    values.reserve(channel_vec.size()*2);

    foreach(PSUChannel* channel, channel_vec)
    {
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

void PSU::update_settings()
{
    foreach(PSUChannel* channel, channel_vec)
    {
        channel->update();
    }
}

enum PSUChannel::Vendor PSU::check_vendor(const QString& vendor)
{
    enum PSUChannel::Vendor vd = PSUChannel::Vendor::none;

    if(vendor.contains("rohde", Qt::CaseInsensitive) ||
            vendor.contains("schwarz", Qt::CaseInsensitive))
    {
        vd = PSUChannel::Vendor::rohdeSchwarz;
    }

    return vd;
}

QString PSU::check_vendor(enum PSUChannel::Vendor vendor)
{
    QString res = "none";

    if(vendor == PSUChannel::Vendor::rohdeSchwarz)
    {
        res = "rohdeschwarz";
    }

    return res;
}

void PSU::init()
{
    /* Setup lxi */
    if(vendor != PSUChannel::Vendor::none)
    {
        QString ip_address = address.mid(0, address.indexOf(":"));
        uint port = (address.midRef(address.indexOf(":") + 1, address.size() - address.indexOf(":") - 1)).toUInt();

        lxi = new LXIClient(port, ip_address);

        // Write reset
        lxi->query("RST");
    }

    /* Setup channel */
    for(uint i = 0; i < channel_max; i++)
    {
        if(is_empty())
        {
            channel_vec.push_back(new PSUChannel{i, lxi, vendor, 0, 0, voltage_max, current_max});
        }
        else
        {
            double voltage_set = 0;
            double current_set = 0;
            double voltage_max = DBL_MAX;
            double current_max = DBL_MAX;

            if(parse_config({"c" + QString::number(i) + "vs",
                            "c" + QString::number(i) + "cs",
                            "c" + QString::number(i) + "vm",
                            "c" + QString::number(i) + "cm"}))
            {
                voltage_set = get_value("c" + QString::number(i) + "vs").toDouble();
                current_set = get_value("c" + QString::number(i) + "cs").toDouble();
                voltage_max = get_value("c" + QString::number(i) + "vm").toDouble();
                current_max = get_value("c" + QString::number(i) + "cm").toDouble();
            }

            channel_vec.push_back(new PSUChannel{i, lxi, vendor, voltage_set, current_set, voltage_max, current_max});
        }
    }

    /* Read values and settings */
    update_settings();

    emit init_done();

    logTimer->start(1000);
}

void PSU::set_master_enable(int master_enable) {
    this->master_enable = master_enable == 0 ? false : true;

    if(vendor == PSUChannel::Vendor::rohdeSchwarz)
    {
        lxi->query("OUTP:MAST " + QString(master_enable ? "ON" : "OFF"));
    }

    emit master_changed(this->master_enable);
}

void PSU::set_master_trigger(int master_trigger)
{
    this->master_trigger = master_trigger > 0 ? true : false;
}

void PSU::switch_on()
{
    foreach(PSUChannel* channel, channel_vec)
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

void PSU::switch_off()
{
    if(has_master_switch() && master_trigger)
    {
        set_master_enable(0);
    }

    foreach(PSUChannel* channel, channel_vec)
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

    foreach(PSUChannel* channel, channel_vec)
    {
        header.push_back("V" + QString::number(channel->get_number()));
        header.push_back("mA" + QString::number(channel->get_number()));
    }

    return header;
}

#include "OSCChannel.h"

#include "EthernetClient.h"

#include <algorithm>

#ifdef DUMMY_DATA
#include <QRandomGenerator>
#include <QtMath>
#endif

OSCChannel::OSCChannel(uint number, EthernetClient* eth, enum Vendor vendor) {
    this->number = number;
    this->eth = eth;
    this->vendor = vendor;

    update_settings_vendor = select_update_settings_vendor();
    update_values_vendor = select_update_values_vendor();
}

void (OSCChannel::*OSCChannel::select_update_settings_vendor())(void)
{
    switch(vendor)
    {
        case Vendor::rigol:
            return &OSCChannel::update_settings_rigol;
        case Vendor::none:
            return &OSCChannel::update_settings_none;
    }
}

void (OSCChannel::*OSCChannel::select_update_values_vendor())(void)
{
    switch(vendor)
    {
        case Vendor::rigol:
            return &OSCChannel::update_values_rigol;
        case Vendor::none:
            return &OSCChannel::update_values_none;
    }
}

void OSCChannel::update_settings()
{
    (this->*update_settings_vendor)();
}

void OSCChannel::update_values()
{
    (this->*update_values_vendor)();

    vmin = *std::min(values.begin(), values.end());
    vmax = *std::max(values.begin(), values.end());
    vpp = vmax - vmin;
    frequency = 0;

    emit vmin_changed(QString::number(vmin));
    emit vmax_changed(QString::number(vmax));
    emit vpp_changed(QString::number(vpp));
    emit frequency_changed(QString::number(frequency));

    emit values_changed(get_values());
}

void OSCChannel::update_settings_rigol()
{
    eth->write(":CHAN" + QString::number(number + 1) + ":DISP " + QString::number(enable));
    eth->write(":CHAN" + QString::number(number + 1) + ":RANG " + QString::number(range));
}

void OSCChannel::update_values_rigol()
{
    eth->write(":WAV:SOUR" + QString::number(number + 1));
    eth->write(":WAV:MODE NORM");
    eth->write(":WAV:FORM BYTE");

    QString output;
    if(eth->query(":WAV:DATA?", output))
    {
        values = stream_to_vector(output);
    }

    values = {0.0};
}

void OSCChannel::update_settings_none()
{

}

void OSCChannel::update_values_none()
{
    values = {1.0, 3.0, 5.0, 7.0, 11.0, 13.0, 17.0, 19.0, 23.0, 29.0, 31.0};
}


inline QVector<double> OSCChannel::stream_to_vector(const QString &input) const
{
    QVector<double> result;

    for(QString::const_iterator it = input.begin(); it != input.end(); ++it)
    {
        result.push_back((QString(*it)).toDouble());
    }

    return result;
}

#include "PSUChannel.h"

#include "LXIClient.h"

#ifdef DUMMY_DATA
#include <QRandomGenerator>
#include <QtMath>
#endif

PSUChannel::PSUChannel(uint number,
                       LXIClient* lxi,
                       enum vendor vd,
                       double voltage_set,
                       double current_set,
                       double voltage_max,
                       double current_max)
    : number(number),
      lxi(lxi),
      vd(vd),
      voltage_max(voltage_max),
      current_max(current_max)
{
    this->voltage_set = voltage_set > voltage_max ? voltage_max : voltage_set;
    this->current_set = current_set > current_max ? current_max : current_set;
}

void PSUChannel::set_enable(int enable)
{
    this->enable = enable == 0 ? false : true;

    update();

    emit enable_changed(this->enable);
}

void PSUChannel::set_voltage()
{
    update();

    emit voltage_changed(QString::number(voltage_set));
}

void PSUChannel::set_voltage(const QString& voltage)
{
    double dvoltage = voltage.toDouble();

    voltage_set = dvoltage > voltage_max ? voltage_max : dvoltage;
}

void PSUChannel::set_current()
{
    update();

    emit current_changed(QString::number(current_set));
}

void PSUChannel::set_current(const QString& current)
{
    double dcurrent = current.toDouble();

    current_set = dcurrent > current_max ? current_max : dcurrent;
}

void PSUChannel::overcurrent_protection()
{
    if(current_set < current_meas*0.95)
    {
        if(trigger)
        {
            set_enable(0);
        }

        emit overcurrent_event();
    }
}

void PSUChannel::update()
{
    overcurrent_protection();

#ifndef DUMMY_DATA
    if(vd == vendor::rohdeSchwarz)
    {
        update_rohdeschwarz();
    }
    else if(vd == vendor::tti)
    {
        update_tti();
    }
#endif

//    if(vd == vendor::vendor)
//        update_vendor();
}

void PSUChannel::meas_voltage()
{
#ifdef DUMMY_DATA
    voltage_meas = voltage_set + double(QRandomGenerator::global()->bounded(-qint16(100), qint16(100))) / double(200);
    voltage_meas = voltage_meas < 0 ? 0 : voltage_meas;
    return ;
#endif

    if(vd == vendor::rohdeSchwarz)
    {
        meas_voltage_rohdeschwarz();
    }
    else if(vd == vendor::tti)
    {
        meas_voltage_tti();
    }

//    if(vd == vendor::vendor)
//        meas_voltage_vendor();
}

void PSUChannel::meas_current()
{
#ifdef DUMMY_DATA
    current_meas = (current_set/2) * (double(QRandomGenerator::global()->bounded(qint16(80), qint16(120))) / double(100));
    current_meas = current_meas < 0 || !enable ? 0 : current_meas;
    return ;
#endif

    if(vd == vendor::rohdeSchwarz)
    {
        meas_current_rohdeschwarz();
    }
    else if(vd == vendor::tti)
    {
        meas_current_tti();
    }

//    if(vd == vendor::vendor)
//        meas_current_vendor();
}

void PSUChannel::update_rohdeschwarz()
{
    if(lxi->query("INST OUT" + QString::number(number + 1)))
    {
        lxi->query(("VOLT " + QString::number(voltage_set)));
        lxi->query(("CURR " + QString::number(current_set/1000.0)));
        lxi->query("OUTP:CHAN " + QString(enable ? "ON" : "OFF"));
    }
}

void PSUChannel::meas_voltage_rohdeschwarz()
{
    if(lxi->query("INST OUT" + QString::number(number + 1)))
    {
        char output[11];

        if(lxi->query("MEAS:VOLT ?", output, 11))
        {
           voltage_meas = (QString(output)).toDouble();
        }
    }
}

void PSUChannel::meas_current_rohdeschwarz()
{
    if(lxi->query("INST OUT" + QString::number(number + 1)))
    {
        char output[11];

        if(lxi->query("MEAS:CURR ?", output, 11))
        {
            current_meas = (QString(output)).toDouble()*1000;
        }
    }
}

void PSUChannel::update_tti() {

}

void PSUChannel::meas_voltage_tti() {

}

void PSUChannel::meas_current_tti() {

}

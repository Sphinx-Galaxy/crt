#include "OSCChannel.h"

#include "EthernetClient.h"

#ifdef DUMMY_DATA
#include <QRandomGenerator>
#include <QtMath>
#endif

OSCChannel::OSCChannel(uint number, EthernetClient * eth, enum vendor vd) {
    this->number = number;
    this->eth = eth;
    this->vd = vd;
}

bool OSCChannel::update() {

}

double OSCChannel::get_range() const {
    QString output;
    if(eth->query(":CHAN" + QString::number(number + 1) + ":RANG?", output))
        return output.toDouble();

    return 0.0;
}

void OSCChannel::set_range(double range) {
    eth->write(":CHAN" + QString::number(number + 1) + ":RANG " + QString::number(range));
}

bool OSCChannel::get_enable() const {
    QString output;
    if(eth->query(":CHAN" + QString::number(number + 1) + ":DISP?", output))
        return output.toInt();

    return 0;
}

void OSCChannel::set_enable(int enable) {
    eth->write(":CHAN" + QString::number(number + 1) + ":DISP " + QString::number(enable));
}

void OSCChannel::set_offset(double offset) {

}

QVector<double> OSCChannel::get_values() const {
    eth->write(":WAV:SOUR" + QString::number(number + 1));
    eth->write(":WAV:MODE NORM");
    eth->write(":WAV:FORM BYTE");

    QString output;
    if(eth->query(":WAV:DATA?", output))
        return stream_to_vector(output);

    return {0.0};
}

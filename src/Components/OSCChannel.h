#ifndef OSCCHANNEL_H
#define OSCCHANNEL_H

/*
 * Author: Mattis Jaksch
 *
 * Single channel of a multi-channel OSC;
 *
 */

class EthernetClient;

#include <QWidget>

class OSCChannel : public QWidget {
Q_OBJECT

public:
    enum class Vendor {none, rigol};

    OSCChannel(uint number, EthernetClient* eth, enum Vendor vendor);
    ~OSCChannel() {}

    uint get_number() const {return number;}
    bool get_enable() const {return enable;}
    double get_range() const {return range;}
    double get_vmax() const {return vmax;}
    double get_vmin() const {return vmin;}
    double get_vpp() const {return vpp;}
    double get_frequency() const {return frequency;}

    QVector<double> get_values() const {return values;}

public slots:
    void set_enable(int enable) {this->enable = enable;}
    void set_range(double range) {this->range = range;}

    void update_settings();
    void update_values();

signals:
    void vmin_changed(const QString&);
    void vmax_changed(const QString&);
    void vpp_changed(const QString&);
    void frequency_changed(const QString&);

    void values_changed(const QVector<double>&);

private:
    uint number;
    EthernetClient* eth;
    enum Vendor vendor;

    bool enable;
    double range;
    double vmin;
    double vmax;
    double vpp;
    double frequency;

    QVector<double> values;

    void (OSCChannel::*select_update_settings_vendor())(void);
    void (OSCChannel::*select_update_values_vendor())(void);

    void (OSCChannel::*update_settings_vendor)();
    void (OSCChannel::*update_values_vendor)();

    /* Rigol */
    void update_settings_rigol();
    void update_values_rigol();

    /* NONE */
    void update_settings_none();
    void update_values_none();

    /* VENDOR */
    //void update_vendor();
    //void get_values_vendor();

    QVector<double> stream_to_vector(const QString &input) const;
};

#endif // OSCCHANNEL_H

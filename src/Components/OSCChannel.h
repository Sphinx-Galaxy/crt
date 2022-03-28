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
    enum vendor {none, rigol};

    OSCChannel(uint number, EthernetClient *eth, enum vendor vd);
    ~OSCChannel() {}

    uint get_number() const {return number;}

    bool get_enable() const;
    double get_vmax() const;
    double get_vmin() const;
    double get_vpp() const;
    double get_frequency() const;

    double get_range() const;
    double get_offset() const;

    QVector<double> get_values() const;

public slots:
    void set_enable(int enable);
    void set_range(double range);
    void set_offset(double offset);

    bool update();

signals:
    void enable_changed(bool enable);
    void vmax_changed(const QString&);
    void vmin_changed(const QString&);
    void vpp_changed(const QString&);
    void frequency_changed(const QString&);

    void values_changed(const QVector<double>&);

private:
    uint number;
    EthernetClient *eth;
    enum vendor vd;

    /* VENDOR */
    //void update_vendor();
    //void meas_voltage_vendor();
    //void meas_current_vendor();

    QVector<double> stream_to_vector(const QString &input) const;
};

inline QVector<double> OSCChannel::stream_to_vector(const QString &input) const {
    QVector<double> result;

    for(QString::const_iterator it = input.begin(); it != input.end(); ++it)
        result.push_back((QString(*it)).toDouble());

    return result;
}

#endif // OSCCHANNEL_H

#ifndef PSUCHANNEL_H
#define PSUCHANNEL_H

/*
 * Author: Mattis Jaksch
 *
 * Single channel of a multi-channel PSU;
 * Current is always milliampere
 *
 */

class LXIClient;

#include <QWidget>

class PSUChannel : public QWidget
{
Q_OBJECT

public:
    enum vendor {none, rohdeSchwarz, tti};

    PSUChannel(uint number,
               LXIClient* lxi,
               enum vendor vd,
               double voltage_set,
               double current_set,
               double voltage_max,
               double current_max);

    virtual ~PSUChannel() {}

    uint get_number() const
    {
        return number;
    }

    bool get_enable() const
    {
        return enable;
    }

    bool get_trigger() const
    {
        return trigger;
    }

    double get_voltage_meas() const
    {
        return voltage_meas;
    }

    double get_current_meas() const
    {
        return current_meas;
    }

    double get_voltage_set() const
    {
        return voltage_set;
    }

    double get_current_set() const
    {
        return current_set;
    }

    double get_voltage_max() const
    {
        return voltage_max;
    }

    double get_current_max() const
    {
        return current_max;
    }

public slots:
    void set_enable(int enable);
    void set_trigger(int trigger);
    void set_voltage(const QString& voltage);
    void set_current(const QString& current);
    void set_voltage();
    void set_current();

    void update();
    void meas_voltage();
    void meas_current();

signals:
    void voltage_changed(const QString& text);
    void current_changed(const QString& text);
    void enable_changed(bool enable);
    void overcurrent_event();

private:
    uint number;
    LXIClient* lxi;
    enum vendor vd;
    bool enable = false;
    bool trigger = false;

    double voltage_meas = 0, current_meas = 0;
    double voltage_set, current_set;
    double voltage_max, current_max;

    void overcurrent_protection();

    /* Rohde Schwarz */
    void update_rohdeschwarz();
    void meas_voltage_rohdeschwarz();
    void meas_current_rohdeschwarz();

    /* TTI */
    void update_tti();
    void meas_voltage_tti();
    void meas_current_tti();

    /* VENDOR */
    //void update_vendor();
    //void meas_voltage_vendor();
    //void meas_current_vendor();
};

inline void PSUChannel::set_trigger(int trigger)
{
    this->trigger = trigger > 0 ? true : false;
}

#endif // PSUCHANNEL_H

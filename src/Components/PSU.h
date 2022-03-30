#ifndef PSU_H
#define PSU_H

/*
 * Author: Mattis Jaksch
 *
 * (Hardware-)Interface from a PSU Window to a real
 * powersupply
 *
 * Address example: 192.168.3.103::5025
 *
 */

class LXIClient;

#include "Component.h"
#include "PSUChannel.h"

class PSU : public Component
{
Q_OBJECT

public:
    PSU(RunManager* runManager, const QString& config);
    PSU(RunManager* runManager,
        const QString& m_element_name,
        const QString& address,
        const QString& vendor,
        uint channel_max,
        double voltage_max,
        double current_max);

    virtual ~PSU() override;

    enum PSUChannel::Vendor get_vendor() const
    {
        return vendor;
    }

    QVector<PSUChannel *> get_channel_vec() const
    {
        return channel_vec;
    }

    bool get_master_enable() const
    {
        return master_enable;
    }

    bool has_master_switch() const
    {
        return master_switch;
    }

    void set_config() override;

public slots:
    void set_master_enable(int master_enable);
    void set_master_trigger(int master_trigger);

    void init() override;
    void update() override;
    void update_settings();

    void switch_on();
    void switch_off();

signals:
    void master_changed(bool master_set);
    void disconnected(bool);

protected:
    QString address;
    enum PSUChannel::Vendor vendor;
    bool master_switch;
    bool master_enable = false;
    bool master_trigger = false;
    uint channel_max = 0;
    double voltage_max = 0, current_max = 0;
    LXIClient* lxi = nullptr;

    QVector<PSUChannel*> channel_vec;

private:
    enum PSUChannel::Vendor check_vendor(const QString& vendor);
    QString check_vendor(enum PSUChannel::Vendor vendor);

    bool check_network_connection();

    QStringList generate_header() override;
};

#endif // PSU_H

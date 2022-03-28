#ifndef OSC_H
#define OSC_H

/*
 * Author: Mattis Jaksch
 *
 * (Hardware-)Interface from a OSC Window to a real
 * oscilloscope
 *
 * Address example: 192.168.3.103::5025
 *
 */

class EthernetClient;

#include "Component.h"
#include "OSCChannel.h"

class OSC : public Component {
Q_OBJECT

enum TriggerMode {Auto, Normal, Single};

public:
OSC(RunManager * runManager, const QString &config);
OSC(RunManager * runManager, const QString &m_element_name, const QString &address, const QString &vendor, uint channel_max);
virtual ~OSC() override;

    void set_config() override;

    int get_running() const;
    void get_trigger_channel() const;
    enum TriggerMode get_trigger_mode() const;
    double get_timebase() const;

public slots:
    void init() override;
    void update() override;
    void update_settings();

    void set_running(bool running);
    void set_trigger_channel(uint channel_number);
    void set_trigger_mode(enum TriggerMode triggerMode);
    void set_timebase(double timebase);

    void switch_on();
    void switch_off();

signals:
    void disconnected(bool);

    void running_changed(int);
    void trigger_channel_changed(int);
    void trigger_mode_changed(int);
    void timebase_changed(double);

protected:
    QString address;
    uint channel_max = 0;
    EthernetClient *eth;

    QVector<OSCChannel *> channel_list;

private:
    bool check_network_connection();

    QStringList generate_header() override;
};

inline void OSC::update_settings() {
    foreach (OSCChannel * channel, channel_list)
        channel->update();
}

#endif // OSC_H

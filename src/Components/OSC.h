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

enum class TriggerMode {Auto, Normal, Single};

Q_DECLARE_METATYPE(TriggerMode) // So it can be used in slots and signals

class OSC : public Component {
Q_OBJECT

public:
    OSC(RunManager* runManager, const QString& config);
    OSC(RunManager* runManager,
        const QString& m_element_name,
        const QString& address,
        const QString& vendor,
        uint channel_max);

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

signals:
    void disconnected(bool);

    void settings_changed();

protected:
    QString address;
    uint channel_max = 0;
    EthernetClient* eth;
    enum OSCChannel::Vendor vendor;

    QVector<OSCChannel*> channel_vec;

    bool running;
    uint8_t trigger_channel;
    enum TriggerMode triggerMode;
    double timebase;

private:
    bool check_network_connection();

    enum OSCChannel::Vendor check_vendor(const QString& vendor);
    QString check_vendor(enum OSCChannel::Vendor vendor);

    void (OSC::*select_update_settings_vendor())(void);

    void (OSC::*update_settings_vendor)();

    /* Rigol */
    void update_settings_rigol();

    /* NONE */
    void update_settings_none();

    QStringList generate_header() override;
};

#endif // OSC_H

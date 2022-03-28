#ifndef LABJACK_H
#define LABJACK_H

/*
 * Author: Mattis Jaksch
 *
 * (Hardware-)Interface from a Labjack Window to a real
 * labjack (T7)
 *
 */


class QElapsedTimer;

#include "Component.h"
#include "LabjackChannel.h"

#include <LabJackM.h>

class Labjack : public Component
{
Q_OBJECT

public:
    Labjack(RunManager* runManager, const QString &config);
    Labjack(RunManager* runManager,
            const QString& m_element_name,
            const QString& channel_name,
            int connectionType,
            const QString& identifier,
            const QString& pchannel,
            const QString& nchannel);

    virtual ~Labjack() override;

    int get_main_resolution_address() const
    {
        return 43903;
    }

    int get_main_range_address() const
    {
        return 43900;
    }

    int get_main_settling_address() const
    {
        return 43904;
    }

    QVector<LabjackChannel*> get_channel_vec() const
    {
        return channel_vec;
    }

    void set_config() override;

public slots:
    void init() override;
    void update() override;

    void set_main_settling(const QString& text);
    void set_main_resolution(int index);
    void set_samplerate(const QString& text);
    void set_maximum_samplerate(int is_maximum);

signals:
    void samplerate_changed(const QString& text);

private:
    int handle;
    bool is_connected;
    int connectionType;
    QString identifier;

    QVector<QString> m_name;
    QVector<int> m_pchannel;
    QVector<int> m_nchannel;

    uint channel;
    QVector<LabjackChannel*> channel_vec;
    QVector<int> address_vec;
    QVector<int> type_vec;
    QVector<double> value_vec;

    int* aAddresses;
    int* aTypes;
    double* aValues;

    QElapsedTimer* sampleTimer;
    QElapsedTimer* rangeTimer;
    QElapsedTimer* dataTimer;
    int samplerate = 1, maxSamplerate = 1, fixedSamplerate = 1;
    bool is_maximum = false;

    void get_channel_addresses(const QString& input, QVector<int>& output);
    void get_channel_names(const QString& input, QVector<QString>& output);

    QStringList generate_header() override;

    void open_labjack();

    int write(int address, const int TYPE, double value);
    int read(int address, const int TYPE, double& value);
    int read(QVector<double>& value);

    void adapt_channel_range();
    void adapt_sample_rate(qint64 time);
    void change_samplerate();

    void create_dummy_data(int size, double * values);
};

inline int Labjack::write(int address, const int TYPE, double value)
{
    return is_connected ? LJM_eWriteAddress(handle, address, TYPE, value) : 0;
}

inline int Labjack::read(int address, const int TYPE, double& value)
{
    return is_connected ? LJM_eReadAddress(handle, address, TYPE, &value) : 0;
}

inline void Labjack::set_maximum_samplerate(int is_maximum)
{
    this->is_maximum = is_maximum > 0 ? true : false;
}

inline void Labjack::set_main_settling(const QString& text)
{
    write(get_main_settling_address(), LJM_FLOAT32, text.toInt());
}

inline void Labjack::set_main_resolution(int index)
{
    write(get_main_resolution_address(), LJM_UINT16, index);
}


inline void Labjack::change_samplerate()
{
    if(samplerate <= 0)
        samplerate = 1;

    logTimer->start(1000/samplerate);

    emit samplerate_changed(QString::number(samplerate));
}

#endif // LABJACK_H

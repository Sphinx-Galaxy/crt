#ifndef LABJACKCHANNEL_H
#define LABJACKCHANNEL_H

/*
 * Author: Mattis Jaksch
 *
 * Manages Labjack T7 channels regarding address,
 * differential, (interal/external) gain, boundary and resolution
 *
 */

#include <LabJackM.h>
#include <QObject>
#include <QVector>

class LabjackChannel : public QObject
{
Q_OBJECT

public:
    LabjackChannel(QString const& name,
                   int* handle,
                   int p_chan,
                   int n_chan,
                   double boundary = 0,
                   double range = 10,
                   int gain = 1);

    virtual ~LabjackChannel() override;

    void update_value(double value);

    double get_value() const
    {
        return virtual_gain*value;
    }

    QString get_name() const
    {
        return name;
    }

    int get_pchan() const
    {
        return p_chan;
    }

    int get_nchan() const
    {
        return n_chan;
    }

    double get_boundary() const
    {
        return boundary;
    }

    double get_gain() const
    {
        return virtual_gain;
    }

    double get_range() const
    {
        return range;
    }

    int get_range_index() const
    {
        return range_index;
    }

    int get_range_index(double range);

    int get_pchan_address() const
    {
        return p_chan*2;
    }

    int get_nchan_address() const
    {
        return n_chan*2;
    }

    int get_pchan_resolution_address() const
    {
        return 41500 + p_chan;
    }

    int get_nchan_resolution_address() const
    {
        return 41500 + n_chan;
    }

    int get_pchan_negative_address() const
    {
        return 41000 + p_chan;
    }

    int get_nchan_negative_address() const
    {
        return 41000 + n_chan;
    }

    int get_pchan_range_address() const
    {
        return 40000 + 2*p_chan;
    }

    int get_nchan_range_address() const
    {
        return 40000 + 2*n_chan;
    }

    int get_pchan_settling_address() const
    {
        return 42000 + 2*p_chan;
    }

    int get_nchan_settling_address() const
    {
        return 42000 + 2*n_chan;
    }

    void set_range();

public slots:
    void update();

    void set_boundary(const QString& text);
    void set_virtual_gain(const QString& text);
    void set_range(int index);
    void set_resolution(uint index);
    void set_settling(uint index);

    void refresh_value();

signals:
    void value_changed(const QString&);
    void value_refreshed(const QString&);

    void range_changed(const QString&);
    void boundary_check_failed();

private:
    QString name;
    int* handle;
    int p_chan, n_chan;
    double boundary, range;
    int range_index;
    int virtual_gain;
    bool is_input, is_differential;
    double value = 0.0;

    QVector<double> range_list = {0.01, 0.1, 1, 10};

    void set_differential();

    bool check_boundary();

    int write(int address, const int TYPE, double value);
    int read(int address, const int TYPE, double& value);
};

inline int LabjackChannel::write(int address, const int TYPE, double value)
{
    int res = -1;

    if(*handle > 0)
    {
        res = LJM_eWriteAddress(*handle, address, TYPE, value);
    }

    return res;
}

inline int LabjackChannel::read(int address, const int TYPE, double& value)
{
    int res = -1;

    if(*handle > 0)
    {
        res = LJM_eReadAddress(*handle, address, TYPE, &value);
    }

    return res;
}

inline void LabjackChannel::update_value(double value)
{
    this->value = value;
    check_boundary();
    emit value_changed(QString::number(get_value()));
}

inline void LabjackChannel::refresh_value()
{
    emit value_refreshed(QString::number(get_value()));
}

inline void LabjackChannel::set_differential() {
    write(get_pchan_negative_address(), LJM_UINT16, n_chan);
}

inline void LabjackChannel::set_boundary(const QString& text) {
    boundary = text.toDouble();
}

inline void LabjackChannel::set_virtual_gain(const QString& text) {
    virtual_gain = text.toInt();
}

inline void LabjackChannel::set_resolution(uint index) {
    write(get_pchan_resolution_address(), LJM_UINT16, index);
    write(get_nchan_resolution_address(), LJM_UINT16, index);
}

inline void LabjackChannel::set_settling(uint index) {
    write(get_pchan_settling_address(), LJM_UINT16, index);
    write(get_nchan_settling_address(), LJM_UINT16, index);
}

#endif // LABJACKCHANNEL_H

#ifndef PLOT_H
#define PLOT_H

/*
 * Author: Mattis Jaksch
 *
 * Abstract plot to provide easy use of
 * specialized plots
 *
 */

class QElapsedTimer;

#include "qcustomplot.h"

#include <QWidget>

class Plot : public QWidget
{
Q_OBJECT

public:
    Plot(QCustomPlot* plot, int datapoints, int seconds);
    virtual ~Plot();

public slots:
    virtual void update_plot() = 0;
    void update_time_axis();

protected:
    QElapsedTimer* realTime;

    QCustomPlot* plot;
    QVector<double> timeAxis;
    QVector<int> limits;

    int datapoints, seconds;
    long counter = 0;

    virtual void create_layout() = 0;

    virtual int get_maximum(const QVector<double>& data);
    virtual int get_minimum(const QVector<double>& data);
    int max_function(const QVector<double>& data, bool compare(int value, int limit));
    int min_function(const QVector<double>& data, bool compare(int value, int limit));
    QVector<double> abs_vec(const QVector<double>& vec);

    void shift_into_vector(QVector<double>& vector, double value);

    void recreate_time_axis();
    virtual void recreate_axis(QVector<double>& vec);

};

inline void Plot::shift_into_vector(QVector<double>& vector, double value)
{
    vector.append(value);
    vector.pop_front();
}

inline QVector<double> Plot::abs_vec(const QVector<double>& vec)
{
    QVector<double> absv;
    absv.reserve(vec.size());

    for(QVector<double>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        absv.append(qAbs(*it));
    }

    return absv;
}

inline int Plot::max_function(const QVector<double>& data, bool compare(int value, int limit))
{
    int pos = -1;
    while(pos < (limits.size()-1) &&
          compare(limits[++pos], int(*std::max_element(data.begin(), data.end()))));

    return limits[pos];
}

inline int Plot::min_function(const QVector<double>& data, bool compare(int value, int limit))
{
    int pos = limits.size();
    while(pos > 0 && compare(limits[--pos], int(*std::min_element(data.begin(), data.end()))));

    return limits[pos];
}

inline int Plot::get_maximum(const QVector<double>& data)
{
    return max_function(data, [](int value, int limit) {return value < 1.1*limit;});
}

inline int Plot::get_minimum(const QVector<double> &data)
{
    if(*std::min_element(data.begin(), data.end()) > 0)
    {
        return min_function(data, [](int value, int limit){return limit < 0.9*value;});
    }

    return -max_function(abs_vec(data), [](int value, int limit){return value < 1.1*limit;});
}

inline void Plot::update_time_axis()
{
    if(counter >= datapoints)
    {
        long lastTimepoint = realTime->elapsed() + long(double(seconds * 1000) / double(datapoints));
        shift_into_vector(timeAxis, double(lastTimepoint) / 1000);
    }
}

#endif // PLOT_H

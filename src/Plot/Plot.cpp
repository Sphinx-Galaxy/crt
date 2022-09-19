#include "Plot.h"

#include <QElapsedTimer>

Plot::Plot(QCustomPlot *plot, int datapoints, int seconds)
    : plot(plot), datapoints(datapoints), seconds(seconds)
{
    for(int i = 0; i < 16; ++i)
    {
       limits.append(qPow(2, i)*0.01);
    }

    realTime = new QElapsedTimer;
    realTime->start();

    recreate_time_axis();
}

Plot::~Plot() {}

void Plot::recreate_time_axis()
{
    long lastTimepoint = realTime->elapsed();
    timeAxis.clear();
    timeAxis.reserve(datapoints);

    for(int i = 0; i < datapoints; i++)
    {
        timeAxis.push_back(lastTimepoint / 1000.0 + i * (double(seconds) / double(datapoints)));
    }
}

void Plot::recreate_axis(QVector<double>& vec)
{
    vec.clear();
    vec.reserve(datapoints);

    for(int i = 0; i < datapoints; i++)
    {
        if(datapoints < vec.size())
        {
            vec[i] = 0.0;
        }
        else
        {
            vec.push_back(0.0);
        }
    }
}

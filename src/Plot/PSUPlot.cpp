#include "PSUPlot.h"

#include "src/Components/PSUChannel.h"

PSUPlot::PSUPlot(QCustomPlot* m_plot, PSUChannel* channel, int m_datapoints, int m_seconds)
    : Plot(m_plot, m_datapoints, m_seconds), channel(channel)
{
    recreate_axis(voltage_axis);
    recreate_axis(current_axis);

    create_layout();
}

PSUPlot::~PSUPlot() {}

void PSUPlot::update_plot()
{
    /* Fill either vector up or shift the whole vector to add one datapoint */
    if(counter >= datapoints)
    {
        shift_into_vector(voltage_axis, channel->get_voltage_meas());
        shift_into_vector(current_axis, channel->get_current_meas());
    }
    else
    {
        voltage_axis[int(counter)] = channel->get_voltage_meas();
        current_axis[int(counter)] = channel->get_current_meas();
    }

    update_time_axis();

    int max_voltage = get_maximum(voltage_axis);
    max_voltage = max_voltage > channel->get_voltage_max() ? ceil(channel->get_voltage_max()) : max_voltage;
    max_voltage = max_voltage < 4 ? 4 : max_voltage;

    int max_current = get_maximum(current_axis);
    max_current = max_current > channel->get_current_max() ? ceil(channel->get_current_max()) : max_current;
    max_current = max_current < 10 ? 10 : max_current;

    plot->yAxis->setRange(0, max_voltage);
    plot->yAxis2->setRange(0,max_current);

    plot->graph(0)->setData(timeAxis, voltage_axis);
    plot->graph(1)->setData(timeAxis, current_axis);
    plot->xAxis->setRange(*std::min_element(timeAxis.begin(), timeAxis.end()), *std::max_element(timeAxis.begin(), timeAxis.end()));

    plot->replot();

    counter++;
}

void PSUPlot::create_layout()
{
    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::blue));

    //Assign current graph to the second yAxis
    plot->addGraph(plot->xAxis, plot->yAxis2);
    plot->graph(1)->setPen(QPen(Qt::red));

    plot->xAxis->setLabel("t[s]");
    plot->xAxis->setRange(0, datapoints);

    plot->yAxis->setLabel("[V]");    plot->yAxis->setRange(0, 30);

    plot->yAxis2->setLabel("[mA]");
    plot->yAxis2->setRange(0, 3000);
    plot->yAxis2->setVisible(true);

    plot->replot();
}

#include "LBJPlot.h"

#include "src/Components/LabjackChannel.h"

LBJPlot::LBJPlot(QCustomPlot* m_plot, int m_datapoints, int m_seconds)
    : Plot(m_plot, m_datapoints, m_seconds)
{
    recreate_axis(standard_axis);

    plot_timer = new QElapsedTimer;
    plot_timer->start();

    create_layout();
}

void LBJPlot::update_plot()
{
    /* Return if plot is inactive */
    if(plot_active && !(plot_timer->elapsed() < 9))
    {
        plot_timer->restart();

        PlotElement* plotElement;

        /* Fill either vector up or shift the whole vector to add one datapoint */
        if(counter >= datapoints)
        {
            foreach (plotElement, plotElement_list)
            {
                shift_into_vector(plotElement->get_axis(), plotElement->get_channel()->get_value());
            }
        }
        else
        {
            foreach (plotElement, plotElement_list)
            {
                plotElement->get_axis()[int(counter)] = plotElement->get_channel()->get_value();
            }
        }

        update_time_axis();

        /* Set y-axis */
        int maximum = get_total_limit(true);
        int minimum = get_total_limit(false);

        plot->yAxis->setRange(minimum, maximum);

        /* Plot active elements */
        plot_active = false;
        for(int i = 0; i < plotElement_list.size(); ++i)
        {
            if(plotElement_list[i]->is_plotted())
            {
                plot->graph(i)->setData(timeAxis, plotElement_list[i]->get_axis());
                plot_active = true;
            }
            else
            {
                plot->graph(i)->setData(timeAxis, standard_axis);
            }
        }

        plot->xAxis->setRange(*std::min_element(timeAxis.begin(), timeAxis.end()), *std::max_element(timeAxis.begin(), timeAxis.end()));
        plot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);

        counter++;
    }
}

void LBJPlot::create_layout()
{
    plot->xAxis->setLabel("[s]");
    plot->xAxis->setRange(0, datapoints);

    plot->yAxis->setLabel("[ ]");
    plot->yAxis->setRange(-10, 10);

    plot->replot();
}

void LBJPlot::add_channel(LabjackChannel* channel, QColor color)
{
    plotElement_list.push_back(new PlotElement{channel, standard_axis, false});
    plot->addGraph();
    plot->graph(plotElement_list.size() - 1)->setPen(QPen(color));

    connect(plotElement_list.last(), SIGNAL(plot_active(bool)), this, SLOT(set_plot_active(bool)));
}

void LBJPlot::set_datarate(const QString& datarate)
{
    int drate = datarate.toInt();

    if(drate < 1) drate = 1;
    if(drate > 100) drate = 100;

    datapoints = drate*seconds;

    recreate_time_axis();
    recreate_axis(standard_axis);

    foreach (PlotElement* plotElement, plotElement_list)
    {
        recreate_axis(plotElement->get_axis());
    }

    counter = 0;
}

int LBJPlot::get_total_limit(bool maximum)
{
    int minmax = 0;
    for(QVector<PlotElement *>::iterator it = plotElement_list.begin(); it != plotElement_list.end(); ++it)
    {
        if((*it)->is_plotted())
        {
            if(maximum)
            {
                minmax = get_maximum((*it)->get_axis()) > minmax ? get_maximum((*it)->get_axis()) : minmax;
            }
            else
            {
                minmax = get_minimum((*it)->get_axis()) < minmax ? get_minimum((*it)->get_axis()) : minmax;
            }
        }
    }

    return minmax;
}

void LBJPlot::set_plot_active(bool active)
{
    if(!plot_active)
    {
        recreate_time_axis();
    }

    plot_active = active;
}

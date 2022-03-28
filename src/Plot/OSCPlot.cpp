#include "OSCPlot.h"

#include "src/Components/OSC.h"

OSCPlot::OSCPlot(QCustomPlot* m_plot,  OSC* osc)
    : Plot(m_plot, 256, 30), osc(osc)
{
    update_xAxis(1);

    plotUpdateTimer = new QTimer;
    connect(plotUpdateTimer, SIGNAL(timeout()), this, SLOT(update_plot()));
    plotUpdateTimer->start(1000);

    create_layout();
}

OSCPlot::~OSCPlot()
{
    delete layoutUpdateTimer;
    delete plotUpdateTimer;
}

void OSCPlot::update_plot() {
    /* Set y-axis */
    int maximum = 1;
    int minimum = -1;

    plot->yAxis->setRange(minimum, maximum);

    for(int i = 0; i < plot_list.size(); ++i) {
        if(plot_list[i]->get_enable())
            plot->graph(i)->setData(xAxis, plot_list[i]->get_data());
        else
            plot->graph(i)->setData(xAxis, standard_axis);
    }

    plot->xAxis->setRange(*std::min_element(xAxis.begin(), xAxis.end()), *std::max_element(xAxis.begin(), xAxis.end()));
    plot->replot();
}

void OSCPlot::create_layout() {

}

void OSCPlot::add_channel(OSCChannel *oscchannel, QColor color) {
    plot_list.push_back(new OSCPlotElement(oscchannel));
    recreate_axis(plot_list.last()->get_data());
    connect(oscchannel, SIGNAL(values_changed(const QVector<double>&)),
            plot_list.last(), SLOT(set_data(const QVector<double>&)));

    plot->addGraph();
    plot->graph(plot_list.size() - 1)->setPen(QPen(color));
}

void OSCPlot::update_xAxis(int timebase) {
    //Recreate xAxis
    //Recreate standardAxis
}

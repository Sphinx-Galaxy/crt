#ifndef LBJPLOT_H
#define LBJPLOT_H

/*
 * Author: Mattis Jaksch
 *
 * Custom plot to show the voltage
 * of multiple labjack channels
 *
 */

class LabjackChannel;
class PlotElement;

#include "Plot.h"

static constexpr int LBJ_ADC_BITS = 4;

class LBJPlot : public Plot {
Q_OBJECT

public:
    LBJPlot(QCustomPlot* m_plot, int m_datapoints=30, int m_seconds=30);
    virtual ~LBJPlot() override {}

    QVector<PlotElement*> get_plotelement_list() {return plotElement_list;}

    void add_channel(LabjackChannel * channel, QColor color);

public slots:
    void update_plot() override;

    void set_datarate(const QString &datarate);
    void set_plot_active(bool active);

private:
    QVector<double> standard_axis;
    QVector<PlotElement*> plotElement_list;

    bool plot_active = false;

    QElapsedTimer* plot_timer;

    double get_total_limit(bool maximum);

    void create_layout() override;
};

/* Connection between channel and its graphical representation */
class PlotElement : public QObject
{
Q_OBJECT

public:
    PlotElement(const LabjackChannel* channel, QVector<double> axis, bool plot_enabled)
        : channel(channel), axis(axis), plot_enabled(plot_enabled) {}

    virtual ~PlotElement() {}

    QVector<double>& get_axis()
    {
        return axis;
    }

    const LabjackChannel* get_channel() const
    {
        return channel;
    }

    bool is_plotted() const
    {
        return plot_enabled;
    }

public slots:
    /* Performance: Plot is only active if at least one channel is shown */
    void set_plot_active(int is_plotted)
    {
        plot_enabled = is_plotted > 0 ? true : false;
        if(plot_enabled)
        {
            emit plot_active(true);
        }
    }

signals:
    void plot_active(bool active);

private:
    const LabjackChannel* channel;
    QVector<double> axis;
    bool plot_enabled;
};

#endif // PSUPLOT_H

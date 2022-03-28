#ifndef PSUPLOT_H
#define PSUPLOT_H

/*
 * Author: Mattis Jaksch
 *
 * Custom plot to show the voltage and
 * current of a single channel of the powersupply
 *
 */

class PSUChannel;

#include "Plot.h"

class PSUPlot : public Plot {
Q_OBJECT

public:
    PSUPlot(QCustomPlot* m_plot, PSUChannel* channel, int m_datapoints=30, int m_seconds=30);
    virtual ~PSUPlot() override;

public slots:
    void update_plot() override;

private:
    QVector<double> voltage_axis;
    QVector<double> current_axis;

    PSUChannel* channel;

    void create_layout() override;
};

#endif // PSUPLOT_H

#ifndef OSCPLOT_H
#define OSCPLOT_H

/*
 * Author: Mattis Jaksch
 *
 * Custom plot to show the voltage and of an oscilloscope
 *
 */

class OSC;
class OSCPlotElement;

#include "src/Components/OSCChannel.h"

#include "Plot.h"

class OSCPlot : public Plot
{
Q_OBJECT

public:
    OSCPlot(QCustomPlot* m_plot, OSC* osc);
    virtual ~OSCPlot() override;

    void add_channel(OSCChannel* oscchannel, QColor color);

public slots:
    void update_plot() override;
    void update_xAxis(int timebase);

private:
    OSC *osc;
    QVector<OSCPlotElement*> plot_list;

    QVector<double> xAxis, standard_axis;

    QTimer *layoutUpdateTimer;
    QTimer *plotUpdateTimer;

    void create_layout() override;
    void modify_time_axis();
};

class OSCPlotElement : public QObject {
Q_OBJECT

QVector<double> data;
bool enable;

public:
    OSCPlotElement(OSCChannel *oscchannel) {
        enable = oscchannel->get_enable();
    }
    virtual ~OSCPlotElement() {}

    QVector<double> & get_data() {return data;}
    bool get_enable() const {return enable;}

public slots:
    void set_data(const QVector<double> &data) {this->data = data;}
};

#endif // OSCPLOT_H

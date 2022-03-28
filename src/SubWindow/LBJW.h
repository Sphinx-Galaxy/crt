#ifndef LBJW_H
#define LBJW_H

/*
 * Author: Mattis Jaksch
 *
 * Labjack window to visualize and control
 * one device from labjack.
 *
 */

class Labjack;

class QVBoxLayout;

#include "SubWindow.h"

struct ChannelColor
{
    QString name;
    QColor color;
};

class LBJW : public SubWindow
{
Q_OBJECT

public:
    LBJW(RunManager* m_runManager, Labjack* lbj);
    virtual ~LBJW() override;

private:
    Labjack* lbj;

    QVector<ChannelColor> color_list =
                {{"black", Qt::black},
                 {"blue", Qt::blue},
                 {"red", Qt::red},
                 {"green", Qt::green},
                 {"magenta", Qt::magenta},
                 {"yellow", Qt::yellow}};

    void create_layout() override;
};

#endif // LBJW_H

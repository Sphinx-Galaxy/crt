#ifndef OSCW_H
#define OSCW_H

/*
 * Author: Mattis Jaksch
 *
 * Powersupply window to visualize and control
 * one supply with multiple channels.
 *
 */

class OSC;
class OSCPlot;

#include "SubWindow.h"

class OSCW : public SubWindow {
Q_OBJECT

public:
    OSCW(RunManager* m_runManager, OSC* osc);
    virtual ~OSCW() override;

private:
    OSC* osc;

    void create_layout() override;
};

#endif // OSCW_H

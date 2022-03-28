#ifndef PSUW_H
#define PSUW_H

/*
 * Author: Mattis Jaksch
 *
 * Powersupply window to visualize and control
 * one supply with multiple channels.
 *
 */

class PSU;
class PSUPlot;

#include "SubWindow.h"

class PSUW : public SubWindow
{
Q_OBJECT

public:
    PSUW(RunManager* m_runManager, PSU* psu);
    virtual ~PSUW() override;

private:
    PSU* psu;

    void create_layout() override;
};

#endif // PSUW_H

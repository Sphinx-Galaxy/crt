#ifndef PROGW_H
#define PROGW_H

/*
 * Author: Mattis Jaksch
 *
 * Allows to set a path to a program which can be started
 * either manually or automatically
 *
 */

class ProgrammStarter;

#include "SubWindow.h"

class PROGW : public SubWindow {
Q_OBJECT

public:
    PROGW(RunManager* m_runManager, ProgrammStarter* programmStarter);
    virtual ~PROGW() override;

private slots:
    void path_dialog();

private:
    ProgrammStarter* programmStarter;

    void create_layout() override;
};

#endif // PROGW_H

#ifndef PROGTAB_H
#define PROGTAB_H

/*
 * Author: Mattis Jaksch
 *
 * Tab containing to specify an additional program
 * to run for the test
 *
 */

#include "src/Tabs/WindowTab.h"

class PROGTab : public WindowTab
{
Q_OBJECT

public:
    PROGTab(ConfigManager* m_configManager, RunManager* m_runManager);

private slots:
    void push_new_subwindow(const QString& config) override;
    void create_subwindow_from_dialog() override;

private:
    void create_add_subwindow_dialog() override;
};

#endif // PROGTAB_H

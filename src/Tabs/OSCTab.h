#ifndef OSCTAB_H
#define OSCTAB_H

/*
 * Author: Mattis Jaksch
 *
 * Tab containing multiple oscilloscopes.
 *
 */

#include "src/Tabs/WindowTab.h"

class OSCTab : public WindowTab {
Q_OBJECT

public:
    OSCTab(ConfigManager *m_configManager, RunManager *m_runManager);

private slots:
    void push_new_subwindow(const QString &config) override;
    void create_subwindow_from_dialog() override;

private:
    void create_add_subwindow_dialog() override;
};

#endif // OSCTAB_H

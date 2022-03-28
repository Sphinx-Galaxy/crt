#ifndef PSUTAB_H
#define PSUTAB_H

/*
 * Author: Mattis Jaksch
 *
 * Tab containing multiple powersupplies.
 *
 */

#include "src/Tabs/WindowTab.h"

class PSUTab : public WindowTab
{
Q_OBJECT

public:
    PSUTab(ConfigManager* m_configManager, RunManager* m_runManager);

private slots:
    void push_new_subwindow(const QString& config) override;
    void create_subwindow_from_dialog() override;

private:
    void create_add_subwindow_dialog() override;
};

#endif // PSUTAB_H

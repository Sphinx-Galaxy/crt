#ifndef LBJTAB_H
#define LBJTAB_H

/*
 * Author: Mattis Jaksch
 *
 * Tab containing multiple labjack devices
 *
 */

#include "src/Tabs/WindowTab.h"

class LBJTab : public WindowTab {
Q_OBJECT

public:
    LBJTab(ConfigManager* m_configManager, RunManager* m_runManager);

private slots:
    void push_new_subwindow(const QString& config) override;
    void create_subwindow_from_dialog() override;

private:
    void create_add_subwindow_dialog() override;

    int get_connectionType(const QString& text);
};

#endif // LBJTAB_H

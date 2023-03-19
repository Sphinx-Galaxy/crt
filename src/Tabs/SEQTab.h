#ifndef SEQTAB_H
#define SEQTAB_H

/*
 * Author: Mattis Jaksch
 *
 * Contains sequences to be executed
 * during the test
 *
 */

#include "WindowTab.h"

class SEQTab : public WindowTab
{
Q_OBJECT

public:
    SEQTab(ConfigManager* m_configManager, RunManager* m_runManager);

private slots:
    void push_new_subwindow(const QString& config) override;
    void create_subwindow_from_dialog() override;

private:
    void create_add_subwindow_dialog() override;
};

#endif // SEQTAB_H

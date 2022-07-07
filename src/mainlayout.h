#ifndef MAINLAYOUT_H
#define MAINLAYOUT_H

/*
 * Author: Mattis Jaksch
 *
 * Mainlayout including a menue bar and different (individual) tabs.
 * The configuration, run and event manager is passed down to the tabs in
 * order to provide a load-/saveable config and the exchange of signals
 *
 */

class ConfigManager;

class LBJTab;

class QAction;
class QToolBar;
class QTabWidget;
class QVBoxLayout;

#include "Manager/RunManager.h"

#include <QWidget>

class MainLayout : public QWidget
{
Q_OBJECT

public:
    MainLayout();
    virtual ~MainLayout();

public slots:
    void detach_tab();

private slots:
    void set_start_button(enum RunMode mode);
    void set_stop_button(enum RunMode mode);

private:
    ConfigManager* configManager;
    RunManager* runManager;

    QVBoxLayout* mainVerticalLayout;
    QTabWidget* windowTabs;
    QToolBar* toolbar;

    QAction* startTestButton;
    QAction* stopTestButton;
    QAction* detachTestButton;

    void create_layout();

    QTabWidget* create_window_tabs();
    QToolBar* create_toolbar();

};

#endif // MAINLAYOUT_H

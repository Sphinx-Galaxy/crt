#ifndef WINDOWTAB_H
#define WINDOWTAB_H

/*
 * Author: Mattis Jaksch
 *
 * Abstract class for generating tabs included in the mainlayout.
 * Tabs can contain (abstract) SubWindows, which are configurable by
 * a config passed down from the ConfigManager
 *
 */

class SubWindow;

class QVBoxLayout;

#include "src/Configuration/ConfigManager.h"
#include "src/Manager/RunManager.h"
#include "src/GraphElements/ComponentDialog.h"

#include <QWidget>

class WindowTab : public QWidget
{
Q_OBJECT

public:
    WindowTab(ConfigManager* configManager, RunManager* runManager);
    virtual ~WindowTab();

protected slots:
    void update_layout();

    virtual void create_layout();

    virtual void load_from_config();
    virtual void push_new_subwindow(const QString& config) = 0;

    void save_to_config();

    virtual void create_subwindow_from_dialog() = 0;
    void add_subwindow_from_dialog();

private slots:
    void erase_window(SubWindow* window);

signals:
    void clean_layout();

protected:
    QString sectionName = "";

    ConfigManager* configManager;
    RunManager* runManager;

    QList<SubWindow*> subWindow_list;
    SpecAddDialog* addDialog = nullptr;

    void clear_subwindow_list();

    virtual void create_add_subwindow_dialog() = 0;
    void layout_updater(SubWindow* sub);
};

#endif // WINDOWTAB_H

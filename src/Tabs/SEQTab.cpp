#include "SEQTab.h"

#include "src/Components/Sequencer.h"
#include "src/SubWindow/SEQW.h"

SEQTab::SEQTab(ConfigManager* m_configManager, RunManager* m_runManager)
    : WindowTab(m_configManager, m_runManager)
{
    sectionName = "SEQ";
}

void SEQTab::push_new_subwindow(const QString& config)
{
    SEQW* seq = new SEQW(runManager, new Sequencer(runManager, config));
    subWindow_list.push_back(seq);

    layout_updater(seq);
}

void SEQTab::create_subwindow_from_dialog()
{
    QString name = addDialog->get_entry_list()[0].input_value->text();
    uint tasks = addDialog->get_entry_list()[1].input_value->text().toUInt();

    SEQW* seq = new SEQW(runManager, new Sequencer(runManager, name, tasks));
    subWindow_list.push_back(seq);

    layout_updater(seq);
}

void SEQTab::create_add_subwindow_dialog()
{
    addDialog->add_entry("Name", "Power Cycle");
    addDialog->add_entry("Tasks", "4");
}

#include "LBJTab.h"

#include "src/Components/Labjack.h"
#include "src/SubWindow/LBJW.h"

#include "LabJackM.h"

LBJTab::LBJTab(ConfigManager* m_configManager, RunManager* m_runManager)
    : WindowTab(m_configManager, m_runManager)
{
    sectionName = "LabJack";
}

void LBJTab::push_new_subwindow(const QString& config)
{
    LBJW* lbjw = new LBJW(runManager, new Labjack(runManager, config));
    subWindow_list.push_back(lbjw);

    layout_updater(lbjw);
}

void LBJTab::create_subwindow_from_dialog()
{
    QString name = addDialog->get_entry_list()[0].input_value->text();
    int connectionType = get_connectionType(addDialog->get_entry_list()[1].input_value->text());
    QString identifier = addDialog->get_entry_list()[2].input_value->text();
    QString channel_name = addDialog->get_entry_list()[3].input_value->text();
    QString pchannel = addDialog->get_entry_list()[4].input_value->text();
    QString nchannel = addDialog->get_entry_list()[5].input_value->text();

    LBJW* lbjw = new LBJW(runManager, new Labjack(runManager, name, channel_name, connectionType, identifier, pchannel, nchannel));
    subWindow_list.push_back(lbjw);

    layout_updater(lbjw);
}

void LBJTab::create_add_subwindow_dialog() {
    addDialog->add_entry("Name", "Rotes T7");
    addDialog->add_entry("Connection", "ETH");
    addDialog->add_entry("Identifier", "192.168.3.133");

    addDialog->add_entry("Channel Name", "AIN62/68, AIN63");
    addDialog->add_entry("Channel Positive", "62, 63");
    addDialog->add_entry("Channel Negative", "68, 199");
}

int LBJTab::get_connectionType(const QString &text) {
    if(text.contains("USB", Qt::CaseInsensitive))
        return LJM_ctUSB;

    if(text.contains("ETH", Qt::CaseInsensitive))
        return LJM_ctETHERNET;

    return LJM_ctANY;
}

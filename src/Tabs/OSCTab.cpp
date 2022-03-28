#include "OSCTab.h"

#include "src/Components/OSC.h"
#include "src/SubWindow/OSCW.h"

OSCTab::OSCTab(ConfigManager *m_configManager, RunManager *m_runManager)
    : WindowTab(m_configManager, m_runManager) {
    sectionName = "OSC";
}

void OSCTab::push_new_subwindow(const QString &config) {
    OSCW * oscw = new OSCW(runManager, new OSC(runManager, config));
    subWindow_list.push_back(oscw);

    layout_updater(oscw);
}

void OSCTab::create_subwindow_from_dialog() {
    QString name = addDialog->get_entry_list()[0].input_value->text();
    QString vendor = addDialog->get_entry_list()[1].input_value->text();
    QString address = addDialog->get_entry_list()[2].input_value->text();
    uint channel = addDialog->get_entry_list()[3].input_value->text().toUInt();

    OSCW *oscw = new OSCW(runManager, new OSC(runManager, name, address, vendor, channel));
    subWindow_list.push_back(oscw);

    layout_updater(oscw);
}

void OSCTab::create_add_subwindow_dialog() {
    addDialog->add_entry("Name", "PSU Links");
    addDialog->add_entry("Vendor", "Rohde Schwarz");
    addDialog->add_entry("Address", "192.168.3.103:5025");
    addDialog->add_entry("Channel", "3");
}

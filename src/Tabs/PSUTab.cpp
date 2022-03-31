#include "PSUTab.h"

#include "src/Components/PSU.h"
#include "src/SubWindow/PSUW.h"

PSUTab::PSUTab(ConfigManager* m_configManager, RunManager* m_runManager)
    : WindowTab(m_configManager, m_runManager)
{
    sectionName = "PSU";
}

void PSUTab::push_new_subwindow(const QString& config)
{
    if(PSU::parse_config(config, {"name", "vendor", "address", "channel"}) == false)
    {
        PSUW* psuw = new PSUW(runManager, new PSU(runManager, config));
        subWindow_list.push_back(psuw);

        layout_updater(psuw);
    }
    else
    {
        handle_parsing_error("PSU");
    }
}

void PSUTab::create_subwindow_from_dialog() {
    QString name = addDialog->get_entry_list()[0].input_value->text();
    QString vendor = addDialog->get_entry_list()[1].input_value->text();
    QString address = addDialog->get_entry_list()[2].input_value->text();
    uint channel = addDialog->get_entry_list()[3].input_value->text().toUInt();
    double max_voltage = addDialog->get_entry_list()[4].input_value->text().toDouble();
    double max_current = addDialog->get_entry_list()[5].input_value->text().toDouble();

    PSUW* psuw = new PSUW(runManager, new PSU(runManager, name, address, vendor, channel, max_voltage, max_current));
    subWindow_list.push_back(psuw);

    layout_updater(psuw);
}

void PSUTab::create_add_subwindow_dialog() {
    addDialog->add_entry("Name", "PSU Links");
    addDialog->add_entry("Vendor", "Rohde Schwarz");
    addDialog->add_entry("Address", "192.168.3.103:1024");
    addDialog->add_entry("Channel", "3");
    addDialog->add_entry("Max Voltage [V]", "30");
    addDialog->add_entry("Max Current [mA]", "3000");
}

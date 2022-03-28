#include "ConfigManager.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>

const QString ConfigManager::sectionStart = "Section";
const QString ConfigManager::sectionEnd = "EndSection";

bool ConfigManager::get_config_section(QString name, QString& section)
{
    QFile file(configName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream config(&file);
    QString line;
    int pos = 0;
    bool section_start_found = false;
    bool section_end_found = false;

    while(!config.atEnd() && !section_end_found)
    {
        line = config.readLine();

        if(!section_start_found && line.contains(sectionStart))
        {
            if(line.contains(name))
            {
                if(!is_defined(name, pos))
                {
                    section_start_found = true;
                }
                else
                {
                    pos++;
                }
            }
        }

        if(section_start_found) {
            if(line.contains(sectionEnd))
            {
                section_end_found = true;
            }
            else
            {
                section += line + "\n";
            }
        }
    }

    file.close();

    return (section_start_found && section_end_found);
}

/* Check how many sections of the same kind have already been found */
bool ConfigManager::is_defined(const QString& name, int pos)
{
    bool section_exists = false;
    bool section_defined = false;

    for(QVector<section_position>::iterator i = section_positions.begin(); i != section_positions.end(); i++)
    {
        if((*i).sectionName == name)
        {
            section_exists = true;

            if((*i).pos < pos)
            {
                (*i).pos++;
                section_defined = false;
                break;
            }
            else
            {
                section_defined = true;
                break;
            }
        }
    }

    //Add if the section is not yet in the list
    if(!section_exists)
    {
        section_positions.push_back({name, pos});
        section_defined = false;
    }

    return section_defined;
}

void ConfigManager::load_config()
{
    /* Show dialog */
    QString tmp = QFileDialog::getOpenFileName(this, tr("Open File"), "./");

    if(QFileInfo::exists(tmp))
    {
        configName = tmp;

        //Validate config
        assert(parse_config());

        section_positions.clear();

        emit loading_config();
    }
}

void ConfigManager::save_config()
{
    /* Show dialog */
    QString tmp = QFileDialog::getSaveFileName(this, tr("Open File"), "./");

    if(!tmp.isEmpty())
    {
        //Empty config content
        content.clear();
        content = "";

        //Emit saving config signal so all the (sub)components can pass their input
        emit saving_config();

        //Save config
        QFile file(tmp);
        file.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream in(&file);
        in << content;
        file.close();
    }
}

bool ConfigManager::parse_config()
{
    bool ok = true;
    bool in_section = false;

    QFile file(configName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    QString line;

    while(!in.atEnd()) {
        line = in.readLine();

        if(line != "\n" && line.size() > 0)
        {
            // Check if there is code outside of 'Section' 'EndSection'
            if(!in_section && !line.contains(sectionStart) && !line.contains(sectionEnd))
            {
                ok &= false;
                break;
            }

            // Check if there is an unmatching 'EndSection'
            if(!in_section && line.contains(sectionEnd))
            {
                ok &= false;
                break;
            }

            // Check if there is a stacked 'Section' in 'Section' or missing 'EndSection'
            if(in_section && !line.contains(sectionEnd) && line.contains(sectionStart))
            {
                ok &= false;
                break;
            }

            // Get into section
            if(line.contains(sectionStart) && !line.contains(sectionEnd))
            {
                in_section = true;

                // Check if section has a name of at least 3 characters
                if(line.length() < QString(sectionStart).length()+3)
                {
                    ok &= false;
                    break;
                }
            }

            // Get out of section
            if(line.contains(sectionEnd))
            {
                in_section = false;
            }
        }
    }

    return ok;
}

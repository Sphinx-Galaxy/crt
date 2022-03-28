#include "ConfigManager.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMutex>
#include <QTextStream>

const QString ConfigManager::sectionStart = "Section";
const QString ConfigManager::sectionEnd = "EndSection";

void ConfigManager::load_config()
{
    /* Show dialog */
    QString tmp = QFileDialog::getOpenFileName(this, tr("Open File"), "./");

    /* Validate config */
    if(QFileInfo::exists(tmp))
    {
        if(parse_config(tmp))
        {
            configName = tmp;
            section_position_vec.clear();

            emit loading_config();
        }
        else
        {
            QMessageBox info(QMessageBox::Icon::Critical,
                             "Loading Config",
                             "Could not parse config file");
            info.exec();
        }
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

bool ConfigManager::parse_config(const QString& filename)
{
    bool ok = true;
    bool in_section = false;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&file);
    QString line;

    while(!in.atEnd())
    {
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

bool ConfigManager::get_config_section(const QString& name, QString& sectionBuffer)
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
                sectionBuffer += line + "\n";
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

    foreach(SectionPosition sec, section_position_vec)
    {
        if(sec.sectionName == name)
        {
            section_exists = true;

            if(sec.pos < pos)
            {
                sec.pos++;
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
        section_position_vec.push_back({name, pos});
        section_defined = false;
    }

    return section_defined;
}

void ConfigManager::set_new_config(const QString& configName)
{
    this->configName = configName;
}

void ConfigManager::append_content(const QString& section, const QString& content)
{
    static QMutex mtx;

    mtx.lock();
    this->content += sectionStart + " " + section + "\n" + content + sectionEnd + "\n\n";
    mtx.unlock();
}

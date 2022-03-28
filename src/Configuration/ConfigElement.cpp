#include "ConfigElement.h"

ConfigElement::~ConfigElement()
{
    clear_config();
}

void ConfigElement::load_config(const QString& content)
{
    QString line = "";

    for(QString::const_iterator it = content.begin(); it != content.end(); ++it)
    {
          if((*it) == '\n')
          {
              QStringRef line_name(&line, 0, line.indexOf("="));
              QStringRef line_value(&line, line.indexOf("=")+1, line.size()-line.indexOf("=")-1);

              config_entry_vec.push_back(new ConfigEntry{line_name.toString(), line_value.toString()});

              line = "";
          }
          else
          {
              line += (*it);
          }
    }
}

bool ConfigElement::parse_config(const QVector<QString>& entry_name_vec)
{
    bool ok = true;

    foreach(QString entry_name, entry_name_vec)
    {
        if(get_entry((entry_name)) == nullptr)
        {
            ok &= false;
            break;
        }
    }

    return ok;
}

QString ConfigElement::get_config()
{
    QString config = "";

    foreach(ConfigEntry* entry, config_entry_vec)
    {
        config += entry->name + "=" + entry->value + "\n";
    }

    return config;
}

QString ConfigElement::get_value(const QString& name)
{
    return (get_entry(name) != nullptr) ? get_entry(name)->value : "";
}

void ConfigElement::set_value(const QString& name, const QString& value)
{
    ConfigEntry* entry = get_entry(name);

    if(entry == nullptr)
    {
        config_entry_vec.push_back(new ConfigEntry{name, value});
    }
    else
    {
        entry->value = value;
    }
}

ConfigEntry* ConfigElement::get_entry(const QString& name)
{
   foreach(ConfigEntry* entry, config_entry_vec)
   {
       if(entry->name.contains(name))
       {
           return entry;
       }
   }

    return nullptr;
}

void ConfigElement::clear_config()
{
    foreach(ConfigEntry* entry, config_entry_vec)
    {
        delete entry;
    }
}

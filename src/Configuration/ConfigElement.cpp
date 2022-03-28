#include "ConfigElement.h"

QString ConfigElement::get_config()
{
    QString config = "";

    for(QVector<config_entry>::iterator it = config_entry_list.begin(); it != config_entry_list.end(); it++)
    {
        config += (*it).name + "=" + (*it).value + "\n";
    }

    return config;
}

void ConfigElement::load_config(const QString& content)
{
    QString line = "";

    for(QString::const_iterator it = content.begin(); it != content.end(); it++)
    {
          if((*it) == '\n')
          {
              QStringRef line_name(&line, 0, line.indexOf("="));
              QStringRef line_value(&line, line.indexOf("=")+1, line.size()-line.indexOf("=")-1);

              config_entry_list.push_back({line_name.toString(), line_value.toString()});

              line = "";
          }
          else
          {
              line += (*it);
          }
    }
}

bool ConfigElement::parse_config(const QVector<QString>& entries)
{
    bool ok = true;

    for(QVector<QString>::const_iterator it = entries.begin(); it != entries.end(); it++)
    {
        if(get_entry((*it)) == nullptr)
        {
            ok &= false;
            break;
        }
    }

    return ok;
}

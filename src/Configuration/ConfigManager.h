#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

/*
 * Author: Mattis Jaksch
 *
 * Management to load and verify a config.
 * Other objects can call for their config section
 * and also pass a saveable config section
 *
 */

#include <QWidget>

struct section_position
{
    QString sectionName;
    int pos;
};

class ConfigManager : public QWidget
{
Q_OBJECT

public:
    ConfigManager() {}
    ConfigManager(QString &configName)
        : configName(configName) {}

    virtual ~ConfigManager() {}

    void set_new_config(const QString& configName);
    bool get_config_section(QString name, QString& section);

    void append_content(const QString& section, const QString& content);

public slots:
    void load_config();
    void save_config();

signals:
    void loading_config();
    void saving_config();

private:
    QVector<section_position> section_positions;
    QString configName = ".cfg";
    QString content;

    static const QString sectionStart, sectionEnd;

    bool parse_config();
    bool is_defined(const QString& name, int pos);
};

inline void ConfigManager::set_new_config(const QString& configName)
{
    this->configName = configName;
}

inline void ConfigManager::append_content(const QString& section, const QString& content)
{
    this->content += sectionStart + " " + section + "\n" + content + sectionEnd + "\n\n";
}

#endif // CONFIGMANAGER_H

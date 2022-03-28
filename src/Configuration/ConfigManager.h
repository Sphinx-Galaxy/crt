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

struct SectionPosition
{
    QString sectionName;
    int pos;
};

class ConfigManager : public QWidget
{
Q_OBJECT

public:
    ConfigManager() {}
    ConfigManager(const QString& configName)
        : configName(configName) {}

    virtual ~ConfigManager() {}

    void set_new_config(const QString& configName);
    bool get_config_section(const QString& name, QString& sectionBuffer);

    void append_content(const QString& section, const QString& content);

public slots:
    void load_config();
    void save_config();

signals:
    void loading_config();
    void saving_config();

private:
    QVector<SectionPosition> section_position_vec;
    QString configName = ".cfg";
    QString content;

    static const QString sectionStart, sectionEnd;

    bool parse_config(const QString& filename);
    bool is_defined(const QString& name, int pos);
};

#endif // CONFIGMANAGER_H

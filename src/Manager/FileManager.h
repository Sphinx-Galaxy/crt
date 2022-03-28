#ifndef FILEMANAGER_H
#define FILEMANAGER_H

/*
 * Author: Mattis Jaksch
 *
 * File manager to log values from the devices
 * hosted by the subwindows in the various tabs.
 * Components can be (de-)registered for logging.
 *
 */

#include <QFile>
#include <QMutex>
#include <QVector>

struct ComponentFile
{
    QString name;

    QFile* file;

    const void* subComponent;

    long long creationTime;
};

class FileManager
{

public:
    FileManager() {}
    virtual ~FileManager() {}

    ComponentFile * get_file(const void * subComponent);

    QVector<ComponentFile*> get_file_list() const
    {
        return file_list;
    }

    QString get_root_directory() const
    {
        return root_directory;
    }

    void set_file_header(const void* subComponent, const QStringList& header);
    void set_root_directory(const QString& directory);

    void append_value_to_file(const void* subComponent, double value);
    void append_value_to_file(const void* subComponent, const QString& value);
    void append_values_to_file(const void* subComponent, const QVector<double>& values);
    void append_values_to_file(const void* subComponent, const QStringList& values);

    void register_component(const void* subComponent, const QString name);
    void deregister_component(const void* subComponent);

protected:
    QString root_directory;

    QVector<ComponentFile*> file_list;

    bool file_exists(const QFile* ref_file);

    QString vector_to_string(const QVector<double>& vector);

private:
    QMutex mutex;

    QString escape_text(const QString& text);

    void update_root_directory();
};

inline bool FileManager::file_exists(const QFile* ref_file) {
    bool exists = false;

    foreach (ComponentFile* file, file_list)
    {
        if(file->file->fileName() == ref_file->fileName())
        {
            exists &= true;
            break;
        }
    }

    return exists;
}

inline ComponentFile* FileManager::get_file(const void* subComponent)
{
    foreach (ComponentFile* file, file_list)
    {
        if(file->subComponent == subComponent)
        {
            return file;
        }
    }

    return nullptr;
}

#endif // FILEMANAGER_H

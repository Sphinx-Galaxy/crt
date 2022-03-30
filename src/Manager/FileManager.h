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

class Component;

#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QVector>

struct LogFile
{
    QFile* file_p;
    Component* component_p;

    long long creation_time;
};

class FileManager
{

public:
    FileManager() {}
    virtual ~FileManager() {}

    LogFile* get_file(Component* component);

    QVector<LogFile*> get_file_vec() const
    {
        return file_vec;
    }

    QString get_root_directory() const
    {
        return root_directory;
    }

    void set_file_header(Component* component, const QStringList& header);
    void set_root_directory(const QString& directory);

    void append_value_to_file(Component* component, double value);
    void append_value_to_file(Component* component, const QString& value);
    void append_values_to_file(Component* component, const QVector<double>& values);
    void append_values_to_file(Component* component, const QVector<QVector<double>>& values);
    void append_values_to_file(Component* component, const QStringList& values);

    void register_component(Component* component);
    void deregister_component(Component* component);

protected:
    QString root_directory;

    QVector<LogFile*> file_vec;

    bool file_exists(const QFile* ref_file);

    QString vector_to_string(const QVector<double>& vector, char seperator = ';');

    void update_root_directory();

private:
    static QMutex mtx;

    QString escape_text(const QString& text);
};

inline void FileManager::append_value_to_file(Component* component, double value)
{
    long long time = QDateTime::currentMSecsSinceEpoch() - get_file(component)->creation_time*1000;

    get_file(component)->file_p->write((QString::number(time)
                                         + ";"
                                         + QString(QString::number(value) + "\n")).toUtf8());

    get_file(component)->file_p->flush();
}

inline void FileManager::append_value_to_file(Component* component, const QString& value)
{
    long long time = QDateTime::currentMSecsSinceEpoch() - get_file(component)->creation_time*1000;

    get_file(component)->file_p->write((QString::number(time)
                                         + ";"
                                         + QString(escape_text(value) + "\n")).toUtf8());

    get_file(component)->file_p->flush();
}

inline void FileManager::append_values_to_file(Component* component, const QVector<double>& values)
{
    long long  time = QDateTime::currentMSecsSinceEpoch() - get_file(component)->creation_time*1000;

    QVector<double> internal_values;
    internal_values.append(values);

    QString res = QString::number(time) + ";" + vector_to_string(internal_values) + "\n";

    get_file(component)->file_p->write(res.toUtf8());
    get_file(component)->file_p->flush();
}

inline void FileManager::append_values_to_file(Component* component, const QVector<QVector<double>>& values)
{
    long long  time = QDateTime::currentMSecsSinceEpoch() - get_file(component)->creation_time*1000;

    QStringList internal_values;

    foreach(QVector<double> val_vec, values)
    {
        internal_values.append(vector_to_string(val_vec, ','));
    }

    QString res = QString::number(time) + ";" + internal_values.join(';') + "\n";

    get_file(component)->file_p->write(res.toUtf8());
    get_file(component)->file_p->flush();
}

inline void FileManager::append_values_to_file(Component* component, const QStringList& values)
{
    long long  time = QDateTime::currentMSecsSinceEpoch() - get_file(component)->creation_time*1000;

    QStringList internal_values;
    internal_values.push_back(QString::number(time));
    internal_values.append(values);

    get_file(component)->file_p->write((internal_values.join(';') + "\n").toUtf8());
    get_file(component)->file_p->flush();
}

inline void FileManager::set_file_header(Component* component, const QStringList& header)
{
    QStringList internal_header = {"Time"};
    internal_header.append(header);

    get_file(component)->file_p->write((internal_header.join(';') + "\n").toUtf8());
    get_file(component)->file_p->flush();
}

inline QString FileManager::vector_to_string(const QVector<double>& vector, char seperator)
{
    QString msg = "";
    QVectorIterator<double> it(vector);

    while(it.hasNext())
    {
        msg += QString::number(it.next()) + (it.hasNext() ? QString(seperator) : "");
    }

    return msg;
}

/* Escape line breaks and seperators ';' */
inline QString FileManager::escape_text(const QString& text)
{
    QString res;
    res.reserve(text.size());

    for(QString::const_iterator it = text.begin(); it != text.end(); it++)
    {
        if((*it) == '\n' || (*it) == '\r')
        {
            res.push_back('\\');
            res.push_back((*it) == '\n' ? 'n' : 'r');
        }
        else if((*it) == ';')
        {
            res.push_back('\\');
        }
        else
        {
            res.push_back(*it);
        }
    }

    return res;
}

inline bool FileManager::file_exists(const QFile* ref_file) {
    bool exists = false;

    foreach(LogFile* file, file_vec)
    {
        if(file->file_p->fileName() == ref_file->fileName())
        {
            exists &= true;
            break;
        }
    }

    return exists;
}

inline LogFile* FileManager::get_file(Component* component)
{
    foreach(LogFile* file, file_vec)
    {
        if(file->component_p == component)
        {
            return file;
        }
    }

    return nullptr;
}

#endif // FILEMANAGER_H

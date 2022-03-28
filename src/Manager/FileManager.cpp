#include "FileManager.h"

#include <QDateTime>
#include <QDir>

void FileManager::set_root_directory(const QString& directory)
{
    QDir dir(directory);

    if(!dir.exists())
        dir.mkdir(directory);

    root_directory = directory;

    update_root_directory();
}

void FileManager::update_root_directory()
{
    foreach (ComponentFile* file, file_list) {
        const void* subComponent = file->subComponent;
        QString name = file->name;

        deregister_component(subComponent);
        register_component(subComponent, name);
    }
}

void FileManager::register_component(const void* subComponent, const QString name)
{
    QString filename = root_directory
            + "/"
            + name
            + "_"
            + QString::number(QDateTime::currentSecsSinceEpoch());

    QFile* new_file = new QFile(filename);

    assert(new_file->open(QIODevice::WriteOnly | QIODevice::Append));

    file_list.push_back(new ComponentFile{filename, new_file, subComponent, QDateTime::currentSecsSinceEpoch()});
}

void FileManager::deregister_component(const void* subComponent)
{
    QMutexLocker locker(&mutex);

    for(int i = 0; i < file_list.size(); i++)
    {
        if(file_list[i]->subComponent == subComponent)
        {
            file_list[i]->file->close();
            delete file_list[i]->file;
            delete file_list[i];
            file_list.remove(i);

            break;
        }
    }
}

void FileManager::append_value_to_file(const void* subComponent, double value)
{
    long long time = QDateTime::currentMSecsSinceEpoch() - get_file(subComponent)->creationTime*1000;

    get_file(subComponent)->file->write((QString::number(time)
                                         + ";"
                                         + QString(QString::number(value) + "\n")).toUtf8());

    get_file(subComponent)->file->flush();
}

void FileManager::append_value_to_file(const void* subComponent, const QString& value)
{
    long long time = QDateTime::currentMSecsSinceEpoch() - get_file(subComponent)->creationTime*1000;

    get_file(subComponent)->file->write((QString::number(time)
                                         + ";"
                                         + QString(escape_text(value) + "\n")).toUtf8());

    get_file(subComponent)->file->flush();
}

void FileManager::append_values_to_file(const void * subComponent, const QVector<double> &values)
{
    long long  time = QDateTime::currentMSecsSinceEpoch() - get_file(subComponent)->creationTime*1000;

    QVector<double> internal_values;
//    internal_values.push_back(time);
    internal_values.append(values);

    QString res = QString::number(time) + ";" + vector_to_string(internal_values) + "\n";

    get_file(subComponent)->file->write(res.toUtf8());
    get_file(subComponent)->file->flush();
}

void FileManager::append_values_to_file(const void* subComponent, const QStringList& values)
{
    long long  time = QDateTime::currentMSecsSinceEpoch() - get_file(subComponent)->creationTime*1000;

    QStringList internal_values;
    internal_values.push_back(QString::number(time));
    internal_values.append(values);

    get_file(subComponent)->file->write((internal_values.join(';') + "\n").toUtf8());
    get_file(subComponent)->file->flush();
}

void FileManager::set_file_header(const void* subComponent, const QStringList& header)
{
    QStringList internal_header = {"Time"};
    internal_header.append(header);

    get_file(subComponent)->file->write((internal_header.join(';') + "\n").toUtf8());
    get_file(subComponent)->file->flush();
}

QString FileManager::vector_to_string(const QVector<double>& vector)
{
    QString msg = "";
    QVectorIterator<double> it(vector);

    while(it.hasNext())
    {
        msg += QString::number(it.next()) + (it.hasNext() ? ";" : "");
    }

    return msg;
}

/* Escape line breaks and seperators ';' */
QString FileManager::escape_text(const QString& text)
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

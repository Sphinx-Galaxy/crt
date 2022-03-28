#include "FileManager.h"

#include "src/Components/Component.h"

#include <QDir>

QMutex FileManager::mtx = QMutex();

void FileManager::set_root_directory(const QString& directory)
{
    QDir dir(directory);

    if(!dir.exists())
    {
        dir.mkdir(directory);
    }

    root_directory = directory;

    update_root_directory();
}

void FileManager::update_root_directory()
{
    foreach(LogFile* file, file_vec) {
        deregister_component(file->component_p);
        register_component(file->component_p);
    }
}

void FileManager::register_component(Component* component)
{
    QString filename = root_directory
            + "/"
            + component->get_element_name()
            + "_"
            + QString::number(QDateTime::currentSecsSinceEpoch());

    QFile* file = new QFile(filename);

    assert(file->open(QIODevice::WriteOnly | QIODevice::Append));

    file_vec.push_back(new LogFile{file, component, QDateTime::currentSecsSinceEpoch()});
}

void FileManager::deregister_component(Component* component)
{
    QMutexLocker locker(&mtx);

    for(int i = 0; i < file_vec.size(); i++)
    {
        if(file_vec[i]->component_p == component)
        {
            file_vec[i]->file_p->close();
            delete file_vec[i]->file_p;
            delete file_vec[i];
            file_vec.remove(i);

            break;
        }
    }
}

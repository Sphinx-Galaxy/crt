#include "WindowTab.h"

#include "src/SubWindow/SubWindow.h"

#include <QGroupBox>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

WindowTab::WindowTab(ConfigManager* configManager, RunManager* runManager)
    : configManager(configManager), runManager(runManager)
{
    connect(configManager, SIGNAL(loading_config()), this, SLOT(load_from_config()));
    connect(configManager, SIGNAL(saving_config()), this, SLOT(save_to_config()));

    create_layout();
}

WindowTab::~WindowTab() {}

void WindowTab::create_layout()
{
    QVBoxLayout* mainTabLayout = new QVBoxLayout;
    QVBoxLayout* subTabLayout = new QVBoxLayout;

    foreach(SubWindow* window, subWindow_list)
    {
        /* Main Box for the window */
        QGroupBox* windowGroupBox = new QGroupBox(window->get_component()->get_element_name());

        /* Create delete button */
        QPushButton* deleteButton = new QPushButton(window);
        deleteButton->setIcon(QIcon(":/icon/deleteButton.png"));
        deleteButton->setFixedSize(QSize(32,32));

        /* Destroy the window */
        connect(deleteButton, SIGNAL(clicked()), window, SLOT(deleteLater()));
        connect(window, SIGNAL(destroyed(SubWindow*)), this, SLOT(erase_window(SubWindow*)));

        /* Put everything together */
        QHBoxLayout* windowHLayout = new QHBoxLayout;
        windowHLayout->addWidget(window);
        windowHLayout->addWidget(deleteButton);

        windowGroupBox->setLayout(windowHLayout);

        subTabLayout->addWidget(windowGroupBox);
    }

    /* Create add button */
    QPushButton* addButton = new QPushButton;
    addButton->setIcon(QIcon(":/icon/addButton.png"));
    addButton->setFixedSize(QSize(32,32));
    connect(addButton, SIGNAL(clicked()), this, SLOT(add_subwindow_from_dialog()));
    connect(this, SIGNAL(clean_layout()), addButton, SLOT(deleteLater()));

    subTabLayout->addWidget(addButton);
    subTabLayout->setAlignment(Qt::AlignTop);

    /* Scroll area */
    QWidget* mainTabWidget = new QWidget;
    mainTabWidget->setLayout(subTabLayout);

    QScrollArea* scrollArea = new QScrollArea;
    connect(this, SIGNAL(clean_layout()), scrollArea, SLOT(deleteLater()));

    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(mainTabWidget);
//    scrollArea->setMaximumHeight(windowGroupBox->sizeHint().height()*1.1);

    mainTabLayout->addWidget(scrollArea);

    setLayout(mainTabLayout);
}

void WindowTab::update_layout()
{
    emit clean_layout();
    delete layout();
    create_layout();
}

void WindowTab::erase_window(SubWindow* window)
{
    subWindow_list.removeAll(window);

    update_layout();
}

void WindowTab::save_to_config()
{
    foreach(SubWindow* window, subWindow_list)
    {
        configManager->append_content(sectionName, window->get_config());
    }
}

void WindowTab::clear_subwindow_list()
{
    foreach(SubWindow* window, subWindow_list)
    {
        delete window;
    }

    subWindow_list.clear();
}

void WindowTab::add_subwindow_from_dialog()
{
    if(addDialog == nullptr)
    {
        addDialog = new SpecAddDialog;
        connect(addDialog, SIGNAL(accepted()), this, SLOT(create_subwindow_from_dialog()));

        create_add_subwindow_dialog();
        addDialog->create_dialog();
    }

    addDialog->show();
}

void WindowTab::load_from_config()
{
    clear_subwindow_list();

    QString section_content;
    while(configManager->get_config_section(sectionName, section_content))
    {
        push_new_subwindow(section_content);
        section_content.clear();
    }
}

void WindowTab::layout_updater(SubWindow* sub)
{
    connect(sub, SIGNAL(layout_done()), this, SLOT(update_layout()));
}

void WindowTab::handle_parsing_error(const QString& element_name, const ConfigError* configError)
{
    QMessageBox info(QMessageBox::Icon::Critical,
                     "Loading " + element_name + " Config",
                     "Could not parse config file, unable to find argument \'" + configError->name + "\'");
    info.exec();
}

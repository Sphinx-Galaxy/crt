#include "mainwindow.h"

#include "mainlayout.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMessageBox>

MainWindow::MainWindow()
{
    create_layout();
}

MainWindow::~MainWindow() {}

void MainWindow::create_layout()
{
    setWindowTitle("CRT GUI");

    setMinimumSize(1024,768);

    setCentralWidget(new MainLayout);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Quit", "Do you really want to quit?",
                                    QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    } else {
        event->ignore();
    }
}

#include "mainwindow.h"

#include "mainlayout.h"

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


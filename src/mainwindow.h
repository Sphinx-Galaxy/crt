#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
 * Author: Mattis Jaksch
 *
 * Mainwindow for this application including the
 * Mainlayout, Title and Resolution
 *
 */

#include <QMainWindow>

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();

private:
    void create_layout();

};

#endif // MAINWINDOW_H

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
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H

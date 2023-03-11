#ifndef DETACHEDWINDOW_H
#define DETACHEDWINDOW_H

/*
 * Author: Ferdinand Stehle
 *
 * DetachedWindow for detached tabs
 *
 */

#include <QMainWindow>

class QTabWidget;

class DetachedWindow : public QMainWindow
{
Q_OBJECT

public:
    DetachedWindow(QTabWidget *tabwidget, QWidget *child, QString title);
    virtual ~DetachedWindow();

private:
    QTabWidget *mtabwidget;
    void closeEvent(QCloseEvent *event) override;

};

#endif // DETACHEDWINDOW_H

#include "detachedwindow.h"

#include <QTabWidget>

DetachedWindow::DetachedWindow(QTabWidget *tabwidget, QWidget *child, QString title)
{
    mtabwidget = tabwidget;

    setWindowTitle(title);
    setGeometry(child->frameGeometry());
    setCentralWidget(child);
    child->show();
}

DetachedWindow::~DetachedWindow() {}

void DetachedWindow::closeEvent(QCloseEvent *event) {

    mtabwidget->addTab(centralWidget(), windowTitle());
}

#include <QStackedWidget>
#include "mainwindow.h"
#include "imageviewer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QStackedWidget *stackedWidget = new QStackedWidget;
    imageViewer = new ImageViewer;
    stackedWidget->addWidget(imageViewer);
    setCentralWidget(stackedWidget);
}

MainWindow::~MainWindow()
{
    delete imageViewer;
}

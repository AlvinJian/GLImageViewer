#include <QStackedWidget>
#include <QFileDialog>
#include <QAction>
#include <QToolBar>
#include "mainwindow.h"
#include "imageviewer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    fileDirDialog = new QFileDialog(this, Qt::Dialog);
    QStackedWidget *stackedWidget = new QStackedWidget;
    imageViewer = new ImageViewer;
    stackedWidget->addWidget(imageViewer);
    currentViewer = imageViewer;
    QToolBar* toolbar = addToolBar(QString("main_toolbar"));
    openFileAct = toolbar->addAction("Open Image", [&](bool checked) {
        fileDirDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
        QStringList mimeTypeFilters;
        mimeTypeFilters << "image/jpeg" // will show "JPEG image (*.jpeg *.jpg *.jpe)
                    << "image/png";  // will show "PNG image (*.png)"
        fileDirDialog->setMimeTypeFilters(mimeTypeFilters);
        if (fileDirDialog->exec()) {
            QStringList files = fileDirDialog->selectedFiles();
            QString filePath = files.at(0);
            imageViewer->showImage(filePath);
            this->adjustSize();
        }
    });
    setCentralWidget(stackedWidget);
}

MainWindow::~MainWindow()
{
    delete imageViewer;
    delete fileDirDialog;
}

QSize MainWindow::minimumSizeHint() const {
    return currentViewer->minimumSizeHint();
}

QSize MainWindow::sizeHint() const {
    return currentViewer->sizeHint();
}

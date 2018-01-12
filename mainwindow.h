#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ImageViewer;
class GLImageView;
class QFileDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

private:
    ImageViewer* imageViewer;
    QWidget* currentViewer;
    QFileDialog* fileDirDialog;
    QAction* openFileAct;
};

#endif // MAINWINDOW_H

#include <QHBoxLayout>
#include <iostream>
#include "imageviewer.h"
#include "glimageview.h"

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QHBoxLayout;
    viewer = new GLImageView;
    char* path = ":/images/test.jpeg";
    viewer->loadImage(path);
    layout->addWidget(viewer);
    setLayout(layout);
    QPalette pal = palette();
    // set background
    pal.setColor(QPalette::Background, Qt::red);
    setAutoFillBackground(true);
    setPalette(pal);
}

ImageViewer::~ImageViewer()
{
}

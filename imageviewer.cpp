#include <QHBoxLayout>
#include <iostream>
#include "imageviewer.h"
#include "glimageview.h"

#define LAYOUT_DEBUG 0

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QHBoxLayout;
    viewer = new GLImageView;
    QString path = ":/images/test.jpeg";
    viewer->loadImage(path);
    layout->addWidget(viewer);
    setLayout(layout);
#if LAYOUT_DEBUG
    // set background
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::red);
    setAutoFillBackground(true);
    setPalette(pal);
#endif
}

ImageViewer::~ImageViewer()
{
}

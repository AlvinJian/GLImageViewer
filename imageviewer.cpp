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
    viewer->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
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

void ImageViewer::showImage(QString &imagePath) {
    viewer->loadImage(imagePath);
    viewer->update();
}

QSize ImageViewer::minimumSizeHint() const {
    return viewer->minimumSizeHint();
}

QSize ImageViewer::sizeHint() const {
    return viewer->sizeHint();
}

ImageViewer::~ImageViewer()
{
}

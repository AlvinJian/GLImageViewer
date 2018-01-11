#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <tuple>
#include <memory>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

class GLImageView;


class ImageViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();

signals:

public slots:

private:
    GLImageView *viewer;
    std::unique_ptr<QOpenGLShaderProgram> pProgram;
};

#endif // IMAGEVIEWER_H

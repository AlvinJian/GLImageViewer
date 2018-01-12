#ifndef GLIMAGEVIEW_H
#define GLIMAGEVIEW_H

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <memory>

class GLImageView : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLImageView(QWidget *parent = nullptr);
    ~GLImageView();
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void loadImage(QString& path);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void setupDefaultShaderProgram();
    void setupDefaultTransform();
    void drawImage();
    QVector3D predictCameraPos(QVector3D& translation);
    void moveCamera(QVector3D& translation);

    std::unique_ptr<QOpenGLShaderProgram> shaderProgram;
    std::unique_ptr<QOpenGLTexture> texture;
    std::unique_ptr<QImage> image;
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer ebo;
    bool isTextureSync;
    QColor clearColor;
    float norm_h;
    QSize viewSize;

    QMatrix4x4 model;
    QMatrix4x4 view;
    QVector3D cameraPos;
};

#endif // GLIMAGEVIEW_H

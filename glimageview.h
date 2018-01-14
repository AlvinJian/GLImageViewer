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
    QMatrix4x4 getViewMatrix() const;
    QMatrix4x4 getModelMatrix() const;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void setupDefaultShaderProgram();
    void setupDefaultTransform();
    void drawImage();
    void moveImage(const QPointF& cursorPos);
    void rotateImage(const QPointF& cursorPos);

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

    QVector3D cameraPos;
    QVector3D imagePos;
    QVector3D imageAngle;
    float viewAngle;
    float focalLength;

    QPointF lastClickPos;
    bool isRotMode;
};

#endif // GLIMAGEVIEW_H

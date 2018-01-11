#include "glimageview.h"
#include <vector>
#include <iostream>
#include <QResizeEvent>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

namespace {
    QDebug debugger(QtMsgType::QtDebugMsg);
}

GLImageView::GLImageView(QWidget *parent)
    : QOpenGLWidget(parent),
      clearColor(Qt::black),
      shaderProgram(nullptr),
      texture(nullptr),
      image(nullptr),
      isTextureSync(false),
      norm_h(-1.0f),
      viewSize(640,640),
      vpPos(0, 0),
      ebo(QOpenGLBuffer::Type::IndexBuffer)
{
}

GLImageView::~GLImageView()
{
}

void GLImageView::initializeGL()
{
    initializeOpenGLFunctions();
    setupDefaultShaderProgram();
}

void GLImageView::paintGL()
{
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawImage();
}

void GLImageView::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    vpPos = QSize( (width - side) / 2, (height - side) / 2 );
    viewSize = QSize(width, height);
}

QSize GLImageView::minimumSizeHint() const
{
    int min_h = (int)(320.0f * norm_h);
    return QSize(320, min_h);
}

QSize GLImageView::sizeHint() const
{
    return viewSize;
}

void GLImageView::drawImage() {
    if (image.get() == nullptr) return;
    int side = std::min(viewSize.width(), viewSize.height());
    glViewport(vpPos.width(), vpPos.height(), side, side);

    // setup vertex array object
    if (!vao.isCreated())
    {
        vao.create();
    }
    vao.bind();

    // setup vertex buffer object
    std::vector<GLfloat> coords;
    // bottom left;
    coords.push_back(-1.0f);
    coords.push_back(-1.0f * norm_h);
    coords.push_back(0.0f);
    // tex coordinate
    coords.push_back(0.0f);
    coords.push_back(0.0f);

    // bottom right
    coords.push_back(1.0f);
    coords.push_back(-1.0f * norm_h);
    coords.push_back(0.0f);
    // tex coordinate
    coords.push_back(1.0f);
    coords.push_back(0.0f);

    // top right
    coords.push_back(1.0f);
    coords.push_back(1.0f * norm_h);
    coords.push_back(0.0f);
    // tex coordinate
    coords.push_back(1.0f);
    coords.push_back(1.0f);

    // top left
    coords.push_back(-1.0f);
    coords.push_back(1.0f * norm_h);
    coords.push_back(0.0f);
    // tex coordinate
    coords.push_back(0.0f);
    coords.push_back(1.0f);

    if (!vbo.isCreated())
    {
        vbo.create();
    }
    vbo.bind();
    vbo.allocate(coords.data(), coords.size()*sizeof(GLfloat));

    // setup vertex element object
    // [bl, br, tr, tl]
    static const std::vector<GLuint> indices {
        0, 1, 2,
        2, 3, 0
    };
    if (!ebo.isCreated())
    {
        ebo.create();
    }
    ebo.bind();
    ebo.allocate(indices.data(), indices.size()*sizeof(GLuint));

    // setup texture
    if (texture.get() == nullptr || !isTextureSync) {
        QImage& img = *image.get();
        texture = std::unique_ptr<QOpenGLTexture>(new QOpenGLTexture(img));
        isTextureSync = true;
    }
    shaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    shaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    shaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    shaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    texture->bind();
    // glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void GLImageView::setupDefaultShaderProgram()
{
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    shaderProgram = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(this));
    shaderProgram->addShader(vshader);
    shaderProgram->addShader(fshader);
    shaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    shaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    shaderProgram->link();
    shaderProgram->bind();
    shaderProgram->setUniformValue("texture", 0);
}

void GLImageView::loadImage(QString& path) {
    QImage* p = new QImage(QImage(path).mirrored());
    image = std::unique_ptr<QImage>(p);
    isTextureSync = false;
    norm_h = (float)((float)image->height()/(float)image->width());
    int h = (int)((float)image->width()*norm_h);
    viewSize = QSize(image->width(), h);
    int side = qMin(viewSize.width(), viewSize.height());
    vpPos = QSize( (viewSize.width() - side) / 2,
                   (viewSize.height() - side) / 2 );
}

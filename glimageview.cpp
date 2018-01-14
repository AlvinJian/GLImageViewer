#include "glimageview.h"
#include <vector>
#include <iostream>
#include <QResizeEvent>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

#define DEFAULT_CAMERA_POS_X (0.0f)
#define DEFAULT_CAMERA_POS_Y (0.0f)
#define DEFAULT_CAMERA_POS_Z (-2.0f)

#define CLIP_NEAR (0.01f)
#define CLIP_FAR (100.0f)

#define MIN_FOCAL 1.0f
#define MAX_FOCAL 150.0f

GLImageView::GLImageView(QWidget *parent)
    : QOpenGLWidget(parent),
      shaderProgram(nullptr),
      texture(nullptr),
      image(nullptr),
      isTextureSync(false),
      clearColor(Qt::gray),
      norm_h(-1.0f),
      viewSize(640,640),
      ebo(QOpenGLBuffer::Type::IndexBuffer),
      focalLen(45.0f)
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

void GLImageView::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    float degree = (float)numDegrees.y();
    degree /= 2.0f;
    if (focalLen+degree > MIN_FOCAL && focalLen+degree < MAX_FOCAL) {
        focalLen += degree;
    }
    event->accept();
    update();
}

void GLImageView::drawImage() {
    if (image.get() == nullptr) return;
    glViewport(0, 0, viewSize.width(), viewSize.height());
    qDebug() << viewSize.width() << ", " << viewSize.height() << "\n";
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

    // associate vertex and buffer
    shaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    shaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    shaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    shaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    // assign transform matrices
    QMatrix4x4 projection; // projection matrxi must update everytime!
    float ratio = ((float)viewSize.width())/((float)viewSize.height());
    projection.perspective(focalLen, ratio, CLIP_NEAR, CLIP_FAR);
    QMatrix4x4 model = getModelMatrix();
    shaderProgram->setUniformValue("model", model);
    QMatrix4x4 viewMat = getViewMatrix();
    shaderProgram->setUniformValue("view", viewMat);
    shaderProgram->setUniformValue("projection", projection);

    // setup texture
    if (texture.get() == nullptr || !isTextureSync) {
        QImage& img = *image.get();
        texture = std::unique_ptr<QOpenGLTexture>(new QOpenGLTexture(img));
        isTextureSync = true;
    }
    texture->bind();
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void GLImageView::setupDefaultShaderProgram()
{
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec3 vertex;\n"
        "uniform mediump mat4 model;\n"
        "uniform mediump mat4 view;\n"
        "uniform mediump mat4 projection;\n"
        "\n"
        "attribute mediump vec2 texCoord;\n"
        "varying mediump vec2 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(vertex, 1.0f);\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec2 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    shaderProgram = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(this));
    shaderProgram->addShader(vshader);
    shaderProgram->addShader(fshader);
    // assign locations of vertex and texture coordinates
    shaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    shaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    shaderProgram->link();
    shaderProgram->bind();
    shaderProgram->setUniformValue("texture", 0);
}

void GLImageView::setupDefaultTransform() {
    cameraPos = QVector3D(DEFAULT_CAMERA_POS_X, DEFAULT_CAMERA_POS_Y, DEFAULT_CAMERA_POS_Z);
    imagePos = QVector3D();
}

void GLImageView::loadImage(QString& path) {
    QImage* p = new QImage(QImage(path).mirrored());
    image = std::unique_ptr<QImage>(p);
    isTextureSync = false;
    norm_h = (float)((float)image->height()/(float)image->width());
    int h = (int)((float)viewSize.width()*norm_h);
    viewSize = QSize(viewSize.width(), h);
    resize(viewSize);
    setupDefaultTransform();
}

QMatrix4x4 GLImageView::getViewMatrix() {
    QVector3D up(0.0f, 1.0f, 0.0f);
    QMatrix4x4 ret;
    ret.lookAt(cameraPos, imagePos, up);
    return ret;
}

QMatrix4x4 GLImageView::getModelMatrix() {
    QMatrix4x4 ret;
    ret.translate(imagePos);
    return ret;
}

#include "glwidget.h"
#include "source/misc/util.h"
#include <QOpenGLFunctions>

GLWidget::GLWidget()
{
}

GLWidget::GLWidget(QWidget *)
{
}


void GLWidget::initializeGL()
{

    //GLWidget::initializeGL();
    initializeOpenGLFunctions();
 //   initializeGL()
    QImage image(256,256,QImage::Format_RGB32);
    m_texture = new QOpenGLTexture( image );

    m_program = new QOpenGLShaderProgram;
 //   qDebug() <<Util::read_textfile(":/resources/shaders/screen.vert");
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, Util::loadTextFile(":/resources/shaders/screen.vert"));

    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, Util::loadTextFile(":/resources/shaders/screen.frag"));
    m_program->bindAttributeLocation("vPosition", 0);
    m_program->link();




}

//void GLWidget::paintEvent(QPaintEvent *event)
void GLWidget::paintGL()
{
    // celar the framebuffer
//    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind the texture
    uint texture_unit = 1;
    m_texture->bind( texture_unit );

    // activate the shader
    m_program->bind();
    m_program->setUniformValue( "screen", texture_unit );
    m_program->setUniformValue("CD", CD.x(), CD.y());
    m_program->setUniformValue("barrelScale", barrelScale.x(), barrelScale.y());
    m_program->setUniformValue("chromatic",chromatic);
    m_program->setUniformValue("lsca",lsca);
    m_program->setUniformValue("lamp",lamp);
    m_program->setUniformValue("saturation",saturation);
    m_program->setUniformValue("gamma",gamma);
    m_program->setUniformValue("time",time);

    time++;
    GLfloat vertices[]{ -1.0f, -1.0f,
                        1.0f, -1.0f,
                        -1.0f,  1.0f,
                        1.0f, 1.0f };

    m_program->enableAttributeArray(0);
    m_program->setAttributeArray(0, GL_FLOAT, vertices, 2);
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    m_program->disableAttributeArray(0);

    // release the shader
    m_program->release();


}

void GLWidget::Reset()
{
    time = 0;
}

void GLWidget::setTexture(QImage &img)
{
    m_texture = new QOpenGLTexture( img );
    update();
  //  update();
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{

    QOpenGLShaderProgram * m_program = nullptr;
    QOpenGLTexture       * m_texture = nullptr;
public:

    QVector3D CD = QVector3D(0.15, 0.3,0);
    // Barrel distortion shrinks the display area a bit, this will allow us to counteract that.
    QVector3D barrelScale = QVector3D(1,1,1) - (0.20 * CD);
    // Curvature distortion
    float chromatic = 0.015;
    float lsca = 482.12;
    float lamp = 0.4;
    float saturation = 1.5;
    float gamma = 1.2;


    GLWidget();
    GLWidget(QWidget* );
    void initializeGL() override;
    void paintGL() override ;
//    void paintEvent(QPaintEvent *event) override ;
    void setTexture(QImage& img);
    void resizeGL(int width, int height) override;
};

#endif // GLWIDGET_H

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QMimeData>
#include <QDropEvent>
#include <QString>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
Q_OBJECT

    QOpenGLShaderProgram * m_program = nullptr;
    QOpenGLTexture       * m_texture = nullptr;
public:

//    QVector3D CD = QVector3D(0.15, 0.3,0);
      QVector3D CD = QVector3D(15, 0.3,0);
    // Barrel distortion shrinks the display area a bit, this will allow us to counteract that.
    QVector3D barrelScale = QVector3D(1,1,1) - (0.20 * CD);
    // Curvature distortion
    float chromatic = 0.015;
    float lsca = 482.12;
    float lamp = 0.4;
    float saturation = 1.5;
    float gamma = 1.2;
    float time = 0;

    GLWidget();
    GLWidget(QWidget* );
    void initializeGL() override;
    void paintGL() override ;
    void Reset();
//    void paintEvent(QPaintEvent *event) override ;
    void setTexture(QImage& img);
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent *e) override
    {
//        e->acceptProposedAction();
        if (e->mimeData()->hasUrls()) {
            e->acceptProposedAction();
        }
    }

public slots:
signals:
    void emitOpenFile(QString);
//    void resizeGL(int width, int height) override;
};

#endif // GLWIDGET_H

#-------------------------------------------------
#
# Project created by QtCreator 2019-08-24T19:10:52
#
#-------------------------------------------------

QT       += core gui multimedia opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OK64
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        source/6502/impl.cpp \
        source/glwidget.cpp \
        source/okmemory.cpp \
        source/rvc.cpp \
        source/6502/mos6502.cpp \
        source/misc/util.cpp \
        source/rcomputer.cpp

HEADERS += \
        mainwindow.h \
        source/6502/impl.h \
        source/glwidget.h \
        source/okmemory.h \
        source/rvc.h \
        source/6502/mos6502.h \
        source/misc/util.h \
        source/rcomputer.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target


win32-msvc*{
    QMAKE_CXXFLAGS += -openmp
    LIBS += -openmp
    QMAKE_CXXFLAGS += -O3
    LIBS += -L$$PWD/libs/ -lresid
    LIBS   += -lopengl32 -lopengl
    #LIBS   += -lglu32
    #LIBS   += -lglut32
#   QMAKE $$PWD\..\..\TRSE\Publish\tutorials\OK64\KOS\kos.prg $$PWD/resources/rom/

}

linux*{
    LIBS += -lresid -fopenmp
    QMAKE_CXXFLAGS += -fopenmp -O2

}

macx*{
    LIBS +=  -L/usr/local/lib/ -lresid -openmp
    QMAKE_CXXFLAGS += -openmp -O2
    INCLUDEPATH+=/usr/local/include

}





DISTFILES += \
    resources/rom/font.bin \
    resources/text/opcodes.txt

RESOURCES += \
    resources.qrc

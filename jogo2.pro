#-------------------------------------------------
#
# Project created by QtCreator 2017-10-11T18:49:02
#
#-------------------------------------------------

QT += core gui opengl
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = jogo2
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    openglwidget.cpp \
    model.cpp \
    camera.cpp \
    light.cpp \
    trackball.cpp \
    material.cpp \
    camera.cpp \
    light.cpp \
    main.cpp \
    mainwindow.cpp \
    material.cpp \
    model.cpp \
    openglwidget.cpp \
    trackball.cpp \
    camera.cpp \
    light.cpp \
    main.cpp \
    mainwindow.cpp \
    material.cpp \
    model.cpp \
    openglwidget.cpp \
    trackball.cpp

HEADERS += \
        mainwindow.h \
    openglwidget.h \
    model.h \
    camera.h \
    light.h \
    trackball.h \
    material.h \
    camera.h \
    light.h \
    mainwindow.h \
    material.h \
    model.h \
    openglwidget.h \
    trackball.h \
    camera.h \
    light.h \
    mainwindow.h \
    material.h \
    model.h \
    openglwidget.h \
    trackball.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    formas/cube.off \
    formas/sphere.off \
    formas/cube.off \
    formas/sphere.off \
    texturas/lava.jpg \
    texturas/metal.jpg \
    formas/cube.off \
    formas/sphere.off

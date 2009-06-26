# -------------------------------------------------
# Project created by QtCreator 2009-04-15T09:46:54
# -------------------------------------------------
QT += core \
    gui
TARGET = qdatamatrix
CONFIG += gui
win32:LIBS += -L. \
    -ldmtx
unix:LIBS +=  -ldmtx
TEMPLATE = app
SOURCES += src/main.cpp \
    src/mainwindow.cpp
HEADERS += src/mainwindow.h
RESOURCES += data/qdatamatrix.qrc
FORMS += forms/mainwindow.ui

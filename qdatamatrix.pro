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
SOURCES += main.cpp \
    mainwindow.cpp
HEADERS += mainwindow.h
RESOURCES += data/qdatamatrix.qrc
FORMS += mainwindow.ui

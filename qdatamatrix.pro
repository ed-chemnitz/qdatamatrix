TARGET = qdatamatrix

!emscripten {
	QT += core gui widgets printsupport
}

win32:LIBS += -L. \
    -ldmtx
unix:LIBS += -ldmtx
TEMPLATE = app
SOURCES += src/main.cpp \
    src/mainwindow.cpp
HEADERS += src/mainwindow.h
RESOURCES += data/qdatamatrix.qrc
FORMS += forms/mainwindow.ui \
    forms/about.ui

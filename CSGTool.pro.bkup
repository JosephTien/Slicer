QT += core
QT -= gui

CONFIG += c++11 precompile_header

TARGET = CSGTool
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

HEADERS += \
    iglmachine.h \
    pch.h \
    tiny_obj_loader.h

SOURCES += \
    main.cpp \
    objloader.cpp

win32 {
    INCLUDEPATH += $$PWD/externals/include/eigen3/
    INCLUDEPATH += $$PWD/externals/include/glm/
    INCLUDEPATH += $$PWD/externals/include/CGAL-4_8_1/
    INCLUDEPATH += $$PWD/externals/include/boost-1_61/
    INCLUDEPATH += $$PWD/externals/include/gmp/
    INCLUDEPATH += $$PWD/externals/include/mpfr/
    LIBS += $$PWD/externals/lib/x64/CGAL-vc140-mt-4.8.1.lib
    LIBS += $$PWD/externals/lib/x64/libboost_thread-vc140-mt-1_61.lib
    LIBS += $$PWD/externals/lib/x64/libboost_system-vc140-mt-1_61.lib
    LIBS += $$PWD/externals/lib/x64/mpir.lib # replace gmp for x64?
    LIBS += $$PWD/externals/lib/x64/mpfr.lib # not sure whether being used or not
    LIBS += $$PWD/externals/lib/x64/tinyobjloader_140.lib
    #QMAKE_CXXFLAGS += -bigobj
    #QMAKE_LFLAGS_DEBUG = /NODEFAULTLIB:libcmt.lib
    #QMAKE_LFLAGS_RELEASE = /NODEFAULTLIB:libcmt.lib
}
macx {
    INCLUDEPATH += /usr/local/include
    INCLUDEPATH += /usr/local/include/eigen3
    LIBS += -L/usr/local/lib -lCGAL -lboost_thread-mt -lboost_system -lgmp
    LIBS += $$PWD/externals/lib/mac/libtinyobjloader.a
}
INCLUDEPATH += $$PWD/externals/include/igl


PRECOMPILED_HEADER = pch.h
#QMAKE_CXXFLAGS_DEBUG -= -g
#QMAKE_CXXFLAGS_RELEASE -= -O2


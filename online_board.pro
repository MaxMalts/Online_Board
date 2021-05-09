QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ServerApi/addlayerargs.cpp \
    ServerApi/confirmaddlayerargs.cpp \
    ServerApi/serverapi.cpp \
    Tools/ellipse.cpp \
    Tools/line.cpp \
    Tools/pencil.cpp \
    Tools/rectangle.cpp \
    Tools/tool.cpp \
    canvas.cpp \
    common.cpp \
    main.cpp \
    onlineboard.cpp \
    serializers.cpp

HEADERS += \
    ServerApi/addlayerargs.h \
    ServerApi/confirmaddlayerargs.h \
    ServerApi/serverapi.h \
    Tools/ellipse.h \
    Tools/line.h \
    Tools/pencil.h \
    Tools/rectangle.h \
    Tools/tool.h \
    canvas.h \
    common.h \
    onlineboard.h \
    serializers.h

FORMS += \
    line_props.ui \
    onlineboard.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DEFINES += JSON_SERIALIZER

DISTFILES += \
    Data/img/tools/line.png

TEMPLATE = lib
DESTDIR = $$PWD/libs

QT += qml quick widgets quickwidgets printsupport

CONFIG += c++11 no_keywords

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-unix-2.0 glib-2.0 gobject-2.0 poppler-qt5 cpdb-libs-frontend
}

LIBS += -lcups

DEFINES += QT_DEPRECATED_WARNINGS

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += /usr/local/include/

HEADERS += \
    headers/preview.h \
    headers/cpd.h

SOURCES += \
    sources/preview.cpp \
    sources/cpd.cpp

RESOURCES += qml.qrc

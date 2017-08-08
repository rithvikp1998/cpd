TEMPLATE = lib

QT += qml quick widgets quickwidgets printsupport

CONFIG += c++11 no_keywords

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-unix-2.0 glib-2.0 gobject-2.0 poppler-qt5
}

LIBS += -lcups

DEFINES += QT_DEPRECATED_WARNINGS

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/backends/cups/src/release/ -lCPDFrontend
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/backends/cups/src/debug/ -lCPDFrontend
else:unix: LIBS += -L$$PWD/backends/cups/src/ -lCPDFrontend

INCLUDEPATH += $$PWD/backends/cups/release/headers
DEPENDPATH += $$PWD/backends/cups/release/headers

HEADERS += \
    headers/preview.h \
    headers/cpd.h

SOURCES += \
    sources/preview.cpp \
    sources/cpd.cpp

RESOURCES += qml.qrc

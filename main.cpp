#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "preview.h"
#include <QQuickView>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickView view;
    view.engine()->addImageProvider(QLatin1String("preview"), new pdf_preview);
    view.setSource(QUrl("qrc:/main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    return app.exec();
}

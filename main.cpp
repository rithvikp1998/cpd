#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "preview.h"
#include <QQuickView>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickView view;
    view.engine()->addImageProvider(QLatin1String("preview"), new QPdfPreview);
    view.setSource(QUrl("qrc:/main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    view.show();

    return app.exec();
}

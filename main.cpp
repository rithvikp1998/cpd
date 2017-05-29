#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "preview.h"
#include <QQuickView>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    //QQmlApplicationEngine engine;
    //engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    QQuickView view;
    view.engine()->addImageProvider(QLatin1String("preview"), new pdf_preview);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}

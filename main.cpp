/* TODO:
 * 1. Make the preview_widget use poppler -- Done
 * 2. Integrate preview_widget into the qml_widget -- Done, for now
 * 3. Make the window responsive
 * 4. Make the preview_widget interactive
 */

#include <QtWidgets>
#include <QtQuickWidgets/QQuickWidget>
#include "preview.h"

extern "C" {
    #include "PrintDialog_Backend/print_frontend.h"
}

class QmlWidget : public QWidget
{
public:
    QmlWidget() {
        qml_widget = new QQuickWidget(QUrl("qrc:/main.qml"), this);
        qml_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    }
private:
    QQuickWidget *qml_widget;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow() {
        QHBoxLayout *layout = new QHBoxLayout();

        QmlWidget *qml_widget =  new QmlWidget();
        qml_widget->setFixedSize(640,480);

        PrintPreviewWidget *preview_widget =  new PrintPreviewWidget();
        preview_widget->setFixedSize(360,480);

        layout->addWidget(qml_widget);
        layout->addWidget(preview_widget);

        setCentralWidget(new QWidget());
        centralWidget()->setLayout(layout);
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    print_frontend_init(0, nullptr);
    return app.exec();
}

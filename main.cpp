/* TODO:
 * 1. Make the preview_widget use poppler -- Done
 * 2. Integrate preview_widget into the qml_widget -- in progress
 * 3. Make the window responsive
 */

#include <QtWidgets>
#include <QtQuickWidgets/QQuickWidget>
#include "preview.h"

class Widget : public QWidget
{
public:
    Widget() {
        mQQuickWidget = new QQuickWidget(QUrl("qrc:/main.qml"), this);
        mQQuickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        //this->show();
    }
private:
    QQuickWidget *mQQuickWidget;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow() {
        QHBoxLayout *layout = new QHBoxLayout();

        Widget *qml_widget =  new Widget();
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
    return app.exec();
}

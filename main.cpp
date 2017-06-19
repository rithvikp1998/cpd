/* TODO:
 * 1. Add image provider for preview (or replace it with existing QPrintPreviewDialog?)
 * 2. Make the window responsive
 */

#include <QtWidgets>
#include <QtQuickWidgets/QQuickWidget>

class Widget : public QWidget
{
public:
    Widget() {
        mQQuickWidget = new QQuickWidget(QUrl("qrc:/main.qml"), this);
        mQQuickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    }
private:
    QQuickWidget *mQQuickWidget;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Widget widget;
    widget.show();
    return app.exec();
}

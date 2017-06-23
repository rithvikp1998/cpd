/* TODO:
 * 1. Make the preview_widget use poppler -- Done
 * 2. Integrate preview_widget into the qml_widget
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
    }
private:
    QQuickWidget *mQQuickWidget;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Widget qml_widget;
    qml_widget.show();
    PrintPreviewWidget preview_widget;
    return app.exec();
}

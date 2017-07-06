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
    QmlWidget(QWidget* parent = Q_NULLPTR):
        QWidget(parent)
      , qml_widget(new QQuickWidget(QUrl("qrc:/main.qml"), this))
    {
        qml_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        qml_widget->setMinimumSize(480, 480);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        qml_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void resize(const QRect& rect)
    {
        QWidget::resize(rect.width(), rect.height());
        qml_widget->resize(rect.width(), rect.height());
    }

private:
    QQuickWidget *qml_widget;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow():
        qml_widget(new QmlWidget(this))
      , layout(new QHBoxLayout())
    {
        setCentralWidget(new QWidget(this));
        //qml_widget->setFixedSize(640,480);

        PrintPreviewWidget *preview_widget =  new PrintPreviewWidget(centralWidget());
        //preview_widget->setFixedSize(360,480);

        layout->addWidget(qml_widget, 2);
        layout->addWidget(preview_widget, 1);

        centralWidget()->setLayout(layout);
        setMinimumSize(640, 480);
        adjustSize();
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QMainWindow::resizeEvent(event);
        const auto itemRect = layout->itemAt(0)->geometry();
        qml_widget->resize(itemRect);
    }

private:
    QmlWidget* qml_widget;
    QHBoxLayout *layout;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    print_frontend_init(0, nullptr);
    return app.exec();
}

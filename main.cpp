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

class PreviewToolbarWidget : public QWidget
{
public:
    PreviewToolbarWidget(QWidget* parent = Q_NULLPTR):
        QWidget(parent)
      , preview_toolbar_widget(new QQuickWidget(QUrl("qrc:/preview_toolbar.qml"), this))
    {
        preview_toolbar_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        preview_toolbar_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void resize(const QRect& rect)
    {
        QWidget::resize(rect.width(), rect.height());
        preview_toolbar_widget->resize(rect.width(), rect.height());
    }

private:
    QQuickWidget *preview_toolbar_widget;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow():
        qml_widget(new QmlWidget(this))
      , preview_toolbar_widget(new PreviewToolbarWidget(this))
      , main_layout(new QHBoxLayout())
      , preview_layout(new QVBoxLayout())
    {
        setCentralWidget(new QWidget(this));

        preview_widget =  new PrintPreviewWidget(centralWidget());

        qml_widget->setMinimumSize(640, 480);
        preview_widget->setMinimumSize(360,440);
        preview_toolbar_widget->setMinimumSize(360,40);

        preview_layout->addWidget(preview_widget, 11);
        preview_layout->addWidget(preview_toolbar_widget, 1);
        preview_layout->setSpacing(0);

        main_layout->addWidget(qml_widget, 64);
        main_layout->addLayout(preview_layout, 36);
        main_layout->setSpacing(0);

        centralWidget()->setLayout(main_layout);
        adjustSize();
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QMainWindow::resizeEvent(event);
        qml_widget->resize(main_layout->itemAt(0)->geometry());
        preview_widget->resize(preview_layout->itemAt(0)->geometry());
        preview_toolbar_widget->resize(preview_layout->itemAt(1)->geometry());
    }

private:
    QmlWidget* qml_widget;
    PrintPreviewWidget *preview_widget;
    PreviewToolbarWidget *preview_toolbar_widget;
    QHBoxLayout *main_layout;
    QVBoxLayout *preview_layout;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    //print_frontend_init(0, nullptr); // To do: Dialog crashes when closed due to this
    return app.exec();
}

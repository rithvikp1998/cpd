#include <QtWidgets>
#include <QtQuickWidgets/QQuickWidget>
#include <qlogging.h>
#include <QQuickItem>

#include "preview.h"

extern "C" {
    #include "PrintDialog_Backend/print_frontend.h"
}

class QmlWidget : public QWidget
{
public:
    QmlWidget(QWidget* parent = Q_NULLPTR):
        QWidget(parent),
        qmlWidget(new QQuickWidget(QUrl("qrc:/main.qml"), this))
    {
        qmlWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        qmlWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void resize(const QRect& rect)
    {
        QWidget::resize(rect.width(), rect.height());
        qmlWidget->resize(rect.width(), rect.height());
    }

private:
    QQuickWidget *qmlWidget;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow():
        qmlWidget(new QmlWidget(this)),
        previewToolbarWidget(new PreviewToolbarWidget(this)),
        mainLayout(new QHBoxLayout()),
        previewLayout(new QVBoxLayout())
    {
        setCentralWidget(new QWidget(this));

        previewWidget =  new PrintPreviewWidget(centralWidget());

        qmlWidget->setMinimumSize(640, 480);
        previewWidget->setMinimumSize(360,440);
        previewToolbarWidget->setMinimumSize(360,40);

        previewLayout->addWidget(previewWidget, 11);
        previewLayout->addWidget(previewToolbarWidget, 1);
        previewLayout->setSpacing(0);

        mainLayout->addWidget(qmlWidget, 64);
        mainLayout->addLayout(previewLayout, 36);
        mainLayout->setSpacing(0);

        centralWidget()->setLayout(mainLayout);
        adjustSize();

        connect(previewToolbarWidget->toolbarRootObject, SIGNAL(nextPageButtonClicked()),
                previewWidget, SLOT(showNextPage()));
        connect(previewToolbarWidget->toolbarRootObject, SIGNAL(prevPageButtonClicked()),
                previewWidget, SLOT(showPrevPage()));
        connect(previewToolbarWidget->toolbarRootObject, SIGNAL(zoomSliderValueChanged()),
                previewWidget, SLOT(setZoom()));
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QMainWindow::resizeEvent(event);
        qmlWidget->resize(mainLayout->itemAt(0)->geometry());
        previewWidget->resize(previewLayout->itemAt(0)->geometry());
        previewToolbarWidget->resize(previewLayout->itemAt(1)->geometry());
    }

private:
    QmlWidget* qmlWidget;
    PrintPreviewWidget *previewWidget;
    PreviewToolbarWidget *previewToolbarWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *previewLayout;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    print_frontend_init(0, nullptr); // To do: Dialog crashes when closed due to this
    return app.exec();
}

#include <QtWidgets>
#include <QtQuickWidgets/QQuickWidget>
#include <qlogging.h>
#include <QQuickItem>
#include <QQmlContext>

#include "preview.h"

extern "C" {
    #include "PrintDialog_Backend/src/print_frontend.h"
    #include "PrintDialog_Backend/src/frontend_helper.h"
}

class QmlWidget : public QWidget
{
    Q_OBJECT
public:
    QmlWidget(QWidget* parent = Q_NULLPTR):
        QWidget(parent),
        qmlWidget(new QQuickWidget(QUrl("qrc:/main.qml"), this))
    {
        qmlWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        qmlWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        connect(qmlWidget->rootObject(), SIGNAL(printButtonClicked(QString)), this, SLOT(printDocument(QString)));
        connect(qmlWidget->rootObject(), SIGNAL(cancelButtonClicked()), this, SLOT(qmlQuit()));

        qmlWidget->rootContext()->setContextProperty("jobsList", jobsList);
    }

    void resize(const QRect& rect)
    {
        QWidget::resize(rect.width(), rect.height());
        qmlWidget->resize(rect.width(), rect.height());
    }

    QStringList jobsList;

public Q_SLOTS:
    void printDocument(QString printerName){
        const char *printer_name = printerName.toLatin1().data();
        PrinterObj *p = static_cast<PrinterObj*>(g_hash_table_lookup(f->printer, printer_name));
        if(!p){
            qCritical("Printer %s not found", printer_name);
            return;
        }

        QString printerLocation = p->location;
        QString jobStatus = "Running";
        QString jobsListEntry = printerName + "%" + printerLocation + "%" + jobStatus;
        jobsList.append(jobsListEntry);

        qmlWidget->rootContext()->setContextProperty("jobsList", jobsList);
        //To do: better way to update context property?

        //print_job(nullptr, nullptr); //There is no defintion of print_job in the backend yet
    }

    void qmlQuit(){
        disconnect_from_dbus(f);
        exit(0);
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
        previewWidget->widgetHeight = previewLayout->itemAt(0)->geometry().height();

        connect(previewToolbarWidget->toolbarRootObject, SIGNAL(nextPageButtonClicked()),
                previewWidget, SLOT(showNextPage()));
        connect(previewToolbarWidget->toolbarRootObject, SIGNAL(prevPageButtonClicked()),
                previewWidget, SLOT(showPrevPage()));
        connect(previewToolbarWidget->toolbarRootObject, SIGNAL(zoomSliderValueChanged(qreal)),
                previewWidget, SLOT(setZoom(qreal)));
    }

    ~MainWindow();

public Q_SLOT:
    void closeEvent(QCloseEvent *event) override;

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QMainWindow::resizeEvent(event);
        qmlWidget->resize(mainLayout->itemAt(0)->geometry());
        previewWidget->resize(previewLayout->itemAt(0)->geometry());
        previewToolbarWidget->resize(previewLayout->itemAt(1)->geometry());

        previewWidget->widgetHeight = previewLayout->itemAt(0)->geometry().height();
        previewWidget->setZoom(previewWidget->currentZoomFactor);
    }

private:
    QmlWidget* qmlWidget;
    PrintPreviewWidget *previewWidget;
    PreviewToolbarWidget *previewToolbarWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *previewLayout;
};

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent *event){
    disconnect_from_dbus(f);
    event->accept();
    exit(0);
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    print_frontend_init(0, nullptr);
    return app.exec();
}

#include "main.moc"

#include <QtWidgets>
#include <QQuickWidget>
#include <qlogging.h>
#include <QQuickItem>
#include <QQmlContext>

#include "../headers/preview.h"
#include "../headers/cpd.h"

extern "C" {
    #include "../backends/cups/src/print_frontend.h"
    #include "../backends/cups/src/frontend_helper.h"
}

QQmlWidget::QQmlWidget(QWidget* parent):
    QWidget(parent),
    qmlWidget(new QQuickWidget(QUrl("qrc:/pages/main.qml"), this))
{
    qmlWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qmlWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    qmlWidget->rootContext()->setContextProperty("jobsList", jobsList);

    connect(qmlWidget->rootObject(), SIGNAL(printButtonClicked(QString)),
            this, SLOT(printDocument(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(cancelButtonClicked()),
            this, SLOT(cpdQuit()));
    connect(qmlWidget->rootObject(), SIGNAL(setJobsList(bool)),
            this, SLOT(setJobsList(bool)));
    connect(qmlWidget->rootObject(), SIGNAL(setJobsHoldOptions(QString)),
            this, SLOT(setJobsHoldOptions(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(setAdvancedOptions(QString)),
            this, SLOT(setAdvancedOptions(QString)));
}

void QQmlWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    qmlWidget->resize(rect.width(), rect.height());
}

void QQmlWidget::printDocument(QString printerName)
{
    QByteArray printer_name_ba = printerName.toLocal8Bit();
    char *printer_name = printer_name_ba.data();
    PrinterObj *p = static_cast<PrinterObj*>(g_hash_table_lookup(f->printer, printer_name));
    if(!p){
        qCritical("Printer %s not found", printer_name);
        return;
    }

    QString filePath = ":/test.pdf";
    QByteArray file_path_ba = filePath.toLocal8Bit();
    char *file_path = file_path_ba.data();

    QString resolutionSetting = "resolution";
    QByteArray resolution_setting_ba = resolutionSetting.toLocal8Bit();
    char *resolution_setting = resolution_setting_ba.data();

    QString resolutionValue = "150dpi";
    QByteArray resolution_value_ba = resolutionValue.toLocal8Bit();
    char *resolution_value = resolution_value_ba.data();

    add_setting(p->settings, resolution_setting, resolution_value);

    print_file(f, file_path, printer_name, "CUPS");
}

void QQmlWidget::cpdQuit()
{
    disconnect_from_dbus(f);
    close();
}

void QQmlWidget::setJobsList(bool activeOnly)
{
    jobsList.clear();
    Job *j;
    int x = get_all_jobs(f, &j, activeOnly);
    for(int i=0; i<x; i++){
        QString printerName = j[i].printer_id;
        QString location = j[i].user;
        QString status = j[i].state;
        jobsList.append(printerName + "%" + location + "%" + status);
    }

    qmlWidget->rootContext()->setContextProperty("jobsList", jobsList);
}

void QQmlWidget::setJobsHoldOptions(QString printerName){
    QByteArray printer_name_ba = printerName.toLocal8Bit();
    char *printer_name = printer_name_ba.data();

    QString backendName = "CUPS";
    QByteArray backend_name_ba = backendName.toLocal8Bit();
    char *backend_name = backend_name_ba.data();

    PrinterObj *p = find_PrinterObj(f, printer_name, backend_name);
    if(!p){
        qCritical("Printer %s not found", printer_name);
        return;
    }

    QString optionName = "job-hold-until";
    QByteArray option_name_ba = optionName.toLocal8Bit();
    char *option_name = option_name_ba.data();

    Option *jobHoldOption = get_Option(p, option_name);
    for(int i=0; i<jobHoldOption->num_supported; i++)
        jobHoldOptionsList.append(jobHoldOption->supported_values[i]);

    qmlWidget->rootContext()->setContextProperty("jobHoldOptionsList", jobHoldOptionsList);
}

void QQmlWidget::setAdvancedOptions(QString printerName)
{
    QByteArray printer_name_ba = printerName.toLocal8Bit();
    char *printer_name = printer_name_ba.data();

    QString backendName = "CUPS";
    QByteArray backend_name_ba = backendName.toLocal8Bit();
    char *backend_name = backend_name_ba.data();

    PrinterObj *p = find_PrinterObj(f, printer_name, backend_name);
    if(!p){
        qCritical("Printer %s not found", printer_name);
        return;
    }

    QString optionName = "printer-resolution";
    QByteArray option_name_ba = optionName.toLocal8Bit();
    char *option_name = option_name_ba.data();

    Option *resolutionOption = get_Option(p, option_name);
    for(int i=0; i<resolutionOption->num_supported; i++)
        supportedResolutions.append(resolutionOption->supported_values[i]);

    qmlWidget->rootContext()->setContextProperty("supportedResolutions", supportedResolutions);
}

QCpdWindow::QCpdWindow():
        qmlWidget(new QQmlWidget(this)),
        previewToolbarWidget(new QPreviewToolbarWidget(this)),
        mainLayout(new QHBoxLayout()),
        previewLayout(new QVBoxLayout())
{
    setCentralWidget(new QWidget(this));

    previewWidget =  new QCpdPreviewWidget(centralWidget());

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

    this->show();
    print_frontend_init(0, nullptr);
}

void QCpdWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    qmlWidget->resize(mainLayout->itemAt(0)->geometry());
    previewWidget->resize(previewLayout->itemAt(0)->geometry());
    previewToolbarWidget->resize(previewLayout->itemAt(1)->geometry());

    previewWidget->widgetHeight = previewLayout->itemAt(0)->geometry().height();
    previewWidget->setZoom(previewWidget->currentZoomFactor);
}

QCpdWindow::~QCpdWindow() = default;

void QCpdWindow::closeEvent(QCloseEvent *event)
{
    disconnect_from_dbus(f);
    event->accept();
    close();
}

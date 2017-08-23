/****************************************************************************
**
**  $QT_BEGIN_LICENSE:GPL$
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>
**
**  $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QQuickWidget>
#include <qlogging.h>
#include <QQuickItem>
#include <QQmlContext>

#include "../headers/preview.h"
#include "../headers/cpd.h"

extern "C" {
    #include <CPDFrontend.h>
}

/*!
 *  \class QQmlWidget
 *  \inmodule CPD
 *
 *  \brief The QQmlWidget class creates a widget from the qml files
 *
 *  The dialog is comprised of three widgets - QQmlWidget, QCpdPreviewWidget and
 *  QPreviewToolbarWidget. QQmlWidget creates a QQuickWidget with pages/main.qml as the root and it
 *  handles the signals that are emitted from the pages/main.qml
 */

/*!
 *  \fn QQmlWidget::QQmlWidget(QWidget *parent)
 *
 *  Constructs QQmlWidget objects with \a parent. Connects signals from pages/main.qml with
 *  slots in QQmlWidget.
 */
QQmlWidget::QQmlWidget(QWidget* parent):
    QWidget(parent),
    qmlWidget(new QQuickWidget(QUrl("qrc:/pages/main.qml"), this))
{
    qmlWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qmlWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    qmlWidget->rootContext()->setContextProperty("jobsList", jobsList);

    connect(qmlWidget->rootObject(), SIGNAL(printButtonClicked(QString, QString)),
            this, SLOT(printDocument(QString, QString)));
    connect(qmlWidget->rootObject(), SIGNAL(cancelButtonClicked()),
            this, SLOT(cpdQuit()));
    connect(qmlWidget->rootObject(), SIGNAL(setJobsList(bool)),
            this, SLOT(setJobsList(bool)));
    connect(qmlWidget->rootObject(), SIGNAL(setJobsHoldOptions(QString)),
            this, SLOT(setJobsHoldOptions(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(setAdvancedOptions(QString)),
            this, SLOT(setAdvancedOptions(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(resolutionValueChanged(QString, QString)),
            this, SLOT(setResolutionSetting(QString, QString)));
    connect(qmlWidget->rootObject(), SIGNAL(cancelJob(int, bool)),
            this, SLOT(cancelJob(int, bool)));

    initBackend();
}

/*!
 *  \fn QQmlWidget::~QQmlWidget()
 *
 *  Destroys QQmlWidget objects
 */
QQmlWidget::~QQmlWidget() = default;

/*!
 *  \fn void QQmlWidget::resize(const QRect& rect)
 *
 *  resize() takes a QRect& \a rect as a parameter and uses it to resize the QQmlWidget objects
 *  to the same dimensions as the parameter \a rect
 */
void QQmlWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    qmlWidget->resize(rect.width(), rect.height());
}

/*!
 *  \fn void QQmlWidget::printDocument(QString printerName, QString backendName)
 *
 *  printDocument takes the name of the printer as a parameter \a printerName and \a backendName,
 *  looks up the PrinterObj whose name matches the parameter, sets printer settings to those chosen
 *  by the user and sends the file to printing.
 */
void QQmlWidget::printDocument(QString printerName, QString backendName)
{
    QByteArray printer_name_ba = printerName.toLocal8Bit();
    char *printer_name = printer_name_ba.data();

    QByteArray backend_name_ba = backendName.toLocal8Bit();
    char *backend_name = backend_name_ba.data();

    PrinterObj *p = find_PrinterObj(f, printer_name, backend_name);
    if(!p){
        qCritical("Printer %s not found", printer_name);
        return;
    }

    QString filePath = ":/test.pdf";
    QByteArray file_path_ba = filePath.toLocal8Bit();
    char *file_path = file_path_ba.data();

    print_file(p, file_path);
}

/*!
 *  \fn void QQmlWidget::cpdQuit()
 *
 *  This function acts as a slot for the signal cancelButtonClicked emitted from the
 *  pages/main.qml file. The signal is emitted when the user clicks on the "Cancel" Button
 *  in the sidebar.
 */
void QQmlWidget::cpdQuit()
{
    disconnect_from_dbus(f);
    this->window()->close();
}

/*!
 *  \fn void QQmlWidget::setJobsList(bool activeOnly)
 *
 *  The function gets the a list of all the jobs from all the backends. It then iterates over each
 *  job and concatenates the printer's name, job's user who started the job and the current state
 *  of the job and appends the string to a QStringList called jobsList.
 *
 *  When \a activeOnly is set to 1, only jobs are that are currently running are shown whereas
 *  when it is set to 0, all jobs are shown.
 *
 *  The "%" parameter is used a separator to split the three strings: printer's name, job's user,
 *  job's state. The list shows jobs in the newest-first order.
 */
void QQmlWidget::setJobsList(bool activeOnly)
{
    jobsList.clear();
    Job *j;
    numJobs = get_all_jobs(f, &j, activeOnly);
    for(int i=0; i<numJobs; i++){
        QString printerName = j[numJobs-i-1].printer_id;
        QString user = j[numJobs-i-1].user;
        QString status = j[numJobs-i-1].state;
        jobsList.append(printerName + "%" + user + "%" + status);
    }

    jobStructArray = j;

    qmlWidget->rootContext()->setContextProperty("jobsList", jobsList);
}

/*!
 *  \fn void QQmlWidget::setJobsHoldOptions(QString printerName)
 *
 *  Many printers come with a hold option which specifies when the job should start after a user
 *  clicked the "Print" button in the UI. This function gets all the available options for a printer
 *  with the given name \a printerName and puts them in a QStringList called jobHoldOptionList.
 */
void QQmlWidget::setJobsHoldOptions(QString printerName)
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

    QString optionName = "job-hold-until";
    QByteArray option_name_ba = optionName.toLocal8Bit();
    char *option_name = option_name_ba.data();

    jobHoldOptionsList.clear();
    Option *jobHoldOption = get_Option(p, option_name);
    for(int i=0; i<jobHoldOption->num_supported; i++)
        jobHoldOptionsList.append(jobHoldOption->supported_values[i]);

    qmlWidget->rootContext()->setContextProperty("jobHoldOptionsList", jobHoldOptionsList);
}

/*!
 *  \fn void QQmlWidget::setAdvancedOptions(QString printerName)
 *
 *  This function finds the more advanced options like "Resolution", "Contrast", "Brightness" etc.
 *  for a given printer with name \a printerName and puts them in their corresponding QStringLists.
 *
 *  Note: Due to some limitations with the CUPS API, only "Resolution" options are being set. This
 *  function will be updated as soon as we find a way to overcome the limitations in the API
 */
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

    supportedResolutions.clear();
    Option *resolutionOption = get_Option(p, option_name);
    for(int i=0; i<resolutionOption->num_supported; i++)
        supportedResolutions.append(resolutionOption->supported_values[i]);

    qmlWidget->rootContext()->setContextProperty("supportedResolutions", supportedResolutions);
}

/*!
 *  \fn void QQmlWidget::setResolutionSetting(QString resolutionValue, QString printerName)
 *
 *  This function acts a slot for the resolutionValueChanged signal emitted from the main.qml.
 *  It takes the name of the printer \a printerName and the resolution value selected by the user
 *  as \a resolutionValue and add it to the printerObj's settings struct.
 */
void QQmlWidget::setResolutionSetting(QString resolutionValue, QString printerName)
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

    QString resolutionSetting = "printer-resolution";
    QByteArray resolution_setting_ba = resolutionSetting.toLocal8Bit();
    char *resolution_setting = resolution_setting_ba.data();

    QByteArray resolution_value_ba = resolutionValue.toLocal8Bit();
    char *resolution_value = resolution_value_ba.data();

    add_setting(p->settings, resolution_setting, resolution_value);
}

/*!
 *  \fn void QQmlWidget::cancelJob(int jobIndex, bool activeOnly)
 *
 *  This function acts as a slot for the cancelJob signal emitted when the user selects "Cancel"
 *  menu item in the right click menu of the Jobs tab. The signal comes with \a jobIndex which is
 *  used to cancel the chosen job. The parameter \a activeOnly is used to refresh the jobsList
 *  after the job is cancelled. The jobs list shows jobs in newest-first order whereas the
 *  jobStructArray stores them in oldest-first order, hence the jobIndex is modified before further
 *  use.
 */
void QQmlWidget::cancelJob(int jobIndex, bool activeOnly)
{
    jobIndex = numJobs - jobIndex - 1;
    PrinterObj *p = find_PrinterObj(f, jobStructArray[jobIndex].printer_id,
                                    jobStructArray[jobIndex].backend_name);
    if(!p){
        qCritical("Printer %s not found", jobStructArray[jobIndex].printer_id);
        return;
    }

    cancel_job(p, jobStructArray[jobIndex].job_id);
    setJobsList(activeOnly);
}

/*!
 *  \fn void QQmlWidget::initBackend()
 *
 *  This function is called whenever a new instance of the dialog is created. This function helps
 *  create a new FrontendObj for a new instance of the dialog and connects the FrontendObj to the
 *  dbus for further communication.
 */
void QQmlWidget::initBackend()
{
    event_callback add_cb = static_cast<event_callback>(add_printer_callback);
    event_callback rem_cb = static_cast<event_callback>(remove_printer_callback);
    f = get_new_FrontendObj(NULL, add_cb, rem_cb);
    connect_to_dbus(f);
}

/*!
 *  \class QCpdWindow
 *  \inmodule CPD
 *
 *  This class acts as the main window for the print dialog.
 *
 *  QCpdWindow
 *      |
 *      |
 *  mainLayout
 *      |
 *      |--- qmlWidget
 *      |
 *      |--- previewLayout
 *              |
 *              |--- previewWidget
 *              |--- previewToolbarWidget
 *
 *  \code
 *
 *      printDialog = new QCpdWindow();
 *      // print...
 *
 *  \endcode
 */

/*!
 *  \fn QCpdWindow::QCpdWindow()
 *
 *  Constructs QCpdWindow objects
 */
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
}

/*!
 *  \fn void QCpdWindow::resizeEvent(QResizeEvent* event)
 *
 *  This function overrides the resizeEvent function from QMainWindow and resizes all the child
 *  widgets to match the size of the resized layout items when \a event occurs.
 */
void QCpdWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    qmlWidget->resize(mainLayout->itemAt(0)->geometry());
    previewWidget->resize(previewLayout->itemAt(0)->geometry());
    previewToolbarWidget->resize(previewLayout->itemAt(1)->geometry());

    previewWidget->widgetHeight = previewLayout->itemAt(0)->geometry().height();
    previewWidget->setZoom(previewWidget->currentZoomFactor);
}

/*!
 *  \fn QCpdWindow::~QCpdWindow()
 *
 *  Destroys QCpdWindow objects
 */
QCpdWindow::~QCpdWindow() = default;

/*!
 *  \fn void QCpdWindow::closeEvent(QCloseEvent *event)
 *
 *  This function overrides the closeEvent function from QMainWindow and is called when \a event
 *  occurs i.e. the user clicks on the window close (cross) icon in the window.
 */
void QCpdWindow::closeEvent(QCloseEvent *event)
{
    disconnect_from_dbus(qmlWidget->f);
    event->accept();
    close();
}

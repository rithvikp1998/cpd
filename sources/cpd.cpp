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
 *  This is the default constructor for QQmlWidget. Connects signals from pages/main.qml with
 *  slots in QQmlWidget.
 */
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

    initBackend();
}

/*!
 * \fn void QQmlWidget::resize()
 * \param rect
 *
 *  resize() takes a QRect& as a parameter and uses it to resize the QQmlWidget objects
 *  to the same dimensions as the parameter rect
 */
void QQmlWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    qmlWidget->resize(rect.width(), rect.height());
}

/*!
 *  \fn void QQmlWidget::printDocument()
 *  \param printerName
 *
 *  printDocument takes the name of the printer as a parameter, looks up the PrinterObj whose name
 *  matches the parameter, sets printer settings to those chosen by the user and sends the file
 *  to printing.
 */
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

    print_file(p, file_path);
}

/*!
 * \fn void QQmlWidget::cpdQuit()
 *
 *  This function acts as a slot for the signal cancelButtonClicked emitted from the
 *  pages/main.qml file. The signal is emitted when the user clicks on the "Cancel" Button
 *  in the sidebar.
 */
void QQmlWidget::cpdQuit()
{
    disconnect_from_dbus(f);
    close();
}

/*!
 * \fn void QQmlWidget::setJobsList()
 * \param activeOnly
 *
 *  The function gets the a list of all the jobs from all the backends. It then iterates over each
 *  job and concatenates the printer's name, job's user who started the job and the current state
 *  of the job and appends the string to a QStringList called jobsList
 *
 *  The "%" parameter is used a separator to split the three strings: printer's name, job's user,
 *  job's state.
 */
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

/*!
 * \fn void QQmlWidget::setJobsHoldOptions()
 * \param printerName
 *
 *  Many printers come with a hold option which specifies when the job should start after a user
 *  clicked the "Print" button in the UI. This function gets all the available options for a printer
 *  with the given name and puts them in a QStringList called jobHoldOptionList
 */
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

    jobHoldOptionsList.clear();
    Option *jobHoldOption = get_Option(p, option_name);
    for(int i=0; i<jobHoldOption->num_supported; i++)
        jobHoldOptionsList.append(jobHoldOption->supported_values[i]);

    qmlWidget->rootContext()->setContextProperty("jobHoldOptionsList", jobHoldOptionsList);
}

/*!
 * \fn void QQmlWidget::setAdvancedOptions()
 * \param printerName
 *
 *  This function finds the more advanced options like "Resolution", "Contrast", "Brightness" etc.
 *  and puts them in their corresponding QStringLists.
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
 * \fn void QQmlWidget::initBackend
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
 * \fn void QCpdWindow::resizeEvent()
 * \param event
 *
 *  This function overrides the resizeEvent function from QMainWindow and resizes all the child
 *  widgets to match the size of the resized layout items.
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
 *  \fn void QCpdWindow::~QCpdWindow()
 *
 *  Default destructor for the QCpdWindow class
 */
QCpdWindow::~QCpdWindow() = default;

/*!
 * \fn void QCpdWindow::closeEvent()
 * \param event
 *
 *  This function overrides the closeEvent function from QMainWindow and is called when the user
 *  clicks on the window close (cross) icon in the window.
 */
void QCpdWindow::closeEvent(QCloseEvent *event)
{
    disconnect_from_dbus(qmlWidget->f);
    event->accept();
    close();
}

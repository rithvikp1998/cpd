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
#include <QObject>

#include "../headers/preview.h"
#include "../headers/cpd.h"

extern "C" {
    #include <cpdb-libs-frontend.h>
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

    /* Signals from main.qml are connected to slots here */
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

    /* Signals from general.qml, first forwarded to main.qml and then connected to slots here */
    connect(qmlWidget->rootObject(), SIGNAL(newPrinterSelected(QString)),
            this, SLOT(newPrinterSelected(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(remotePrintersToggled(QString)),
            this, SLOT(remotePrintersToggled(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(orientationChanged(QString)),
            this, SLOT(orientationChanged(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(newPageSizeSelected(QString)),
            this, SLOT(newPageSizeSelected(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(collateToggled(QString)),
            this, SLOT(collateToggled(QString)));
    connect(qmlWidget->rootObject(), SIGNAL(newPageRangeSet(QString)),
            this, SLOT(newPageRangeSet(QString)));
    
    /* Signals from page_setup.qml, first forwarded to main.qml and then connected to slots here */
    connect(qmlWidget->rootObject(), SIGNAL(setDuplexOption(QString)),
            this, SLOT(setDuplexOption(QString)));
    
    /* Signals from jobs.qml, first forwarded to main.qml and then connected to slots here */
    connect(qmlWidget->rootObject(), SIGNAL(cancelJob(int, bool)),
            this, SLOT(cancelJob(int, bool)));

    /* Signals from advanced.qml, first forwarded to main.qml and then connected to slots here */
    connect(qmlWidget->rootObject(), SIGNAL(resolutionValueChanged(QString, QString)),
            this, SLOT(setResolutionSetting(QString, QString)));

    /* Callback function signals */
    connect(cbf::Instance(), SIGNAL(addPrinterSignal(char *, char *, char *)),
            this, SLOT(addPrinter(char *, char *, char *)));
    connect(cbf::Instance(), SIGNAL(removePrinterSignal(char *)),
            this, SLOT(removePrinter(char *)));

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
 *  \fn void QQmlWidget::addPrinter(char *printer_name, char *printer_id, char *backend_name)
 *
 *  Whenever a new printer with name \a printer_name, id \a printer_id and backend \a backend_name
 *  is discovered in the backends, this function is called which adds that printer to the UI.
 */
void QQmlWidget::addPrinter(char *printer_name, char *printer_id, char *backend_name)
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject*>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj,
                                  "addToDestinationModel",
                                  Q_ARG(QVariant, printer_name),
                                  Q_ARG(QVariant, printer_id),
                                  Q_ARG(QVariant, backend_name));
    else
        qDebug() << "generalObject Not Found";
}

/*!
 *  \fn void QQmlWidget::removePrinter(char *printer_name)
 *
 *  Whenever a printer with name \a printer_name is removed,
 *  this function is called which removes that printer to the UI.
 */
void QQmlWidget::removePrinter(char *printer_name)
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject*>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj,
                                  "removeFromDestinationModel",
                                  Q_ARG(QVariant, printer_name));
    else
        qDebug() << "generalObject Not Found";
}

/*!
 *  \fn void QQmlWidget::clearPrinters()
 *
 *  This function clears the list of printers shown in the UI.
 */
void QQmlWidget::clearPrinters()
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject*>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj, "clearDestinationModel");
    else
        qDebug() << "generalObject Not Found";
}

/*!
 *  \fn void QQmlWidget::newPrinterSelected(const QString &printer)
 *
 *  Whenever a new printer with name \a printer is selected by the User, this function updates all
 *  the options shown in the dialog with the options and values supported by the selected printer.
 */
void QQmlWidget::newPrinterSelected(const QString &printer)
{
    QStringList list = printer.split('#');  // printer is in the format: <printer_id>#<backend_name>
    p = find_PrinterObj(f, list[0].toLatin1().data(), list[1].toLatin1().data());

    Options *options = get_all_options(p);

    GHashTableIter iter;
    g_hash_table_iter_init(&iter, options->table);
    gpointer _key, _value;

    while (g_hash_table_iter_next(&iter, &_key, &_value)) {
        char *key = static_cast<char *>(_key);
        Option *value = static_cast<Option *>(_value);
        if (strncmp(key, "copies", 6) == 0) {
            for (int i = 0; i < value->num_supported; i++)
                addMaximumPrintCopies(value->supported_values[i]);
        } else if (strncmp(key, "finishings", 10) == 0) {

        } else if (strncmp(key, "ipp-attribute-fidelity", 22) == 0) {

        } else if (strncmp(key, "job-hold-until", 14) == 0) {
            clearStartJobsModel();
            for (int i = 0; i < value->num_supported; i++)
                updateStartJobsModel(value->supported_values[i]);
        } else if (strncmp(key, "job-name", 8) == 0) {

        } else if (strncmp(key, "job-priority", 12) == 0) {

        } else if (strncmp(key, "job-sheets", 10) == 0) {

        } else if (strncmp(key, "media-col", 9) == 0) {

        } else if (strncmp(key, "media", 5) == 0) {
            clearPaperSizeModel();
            for (int i = 0; i < value->num_supported; i++)
                updatePaperSizeModel(pwg_to_readable(value->supported_values[i]),
                                     value->supported_values[i],
                                     strcmp(value->supported_values[i], value->default_value));
        } else if (strncmp(key, "multiple-document-handling", 26) == 0) {

        } else if (strncmp(key, "number-up", 9) == 0) {
            clearPagesPerSideModel();
            for (int i = 0; i < value->num_supported; i++)
                updatePagesPerSideModel(value->supported_values[i],
                                        strcmp(value->supported_values[i], value->default_value));

        } else if (strncmp(key, "output-bin", 10) == 0) {

        } else if (strncmp(key, "orientation-requested", 21) == 0) {

        } else if (strncmp(key, "page-ranges", 11) == 0) {

        } else if (strncmp(key, "print-color-mode", 16) == 0) {

        } else if (strncmp(key, "print-quality", 13) == 0) {

        } else if (strncmp(key, "printer-resolution", 18) == 0) {
            /*clearResolutionModel();
            for (int i = 0; i < value->num_supported; i++)
                updateResolutionModel(value->supported_values[i],
                                      strcmp(value->supported_values[i], value->default_value));*/
        } else if (strncmp(key, "sides", 5) == 0) {
            clearTwoSidedSwitch();
            for (int i = 0; i < value->num_supported; i++)
                enableTwoSided(value->supported_values[i]);
        } else {
            qDebug() << "Unhandled Option:" << key;
        }
    }
}

/*!
 *  \fn void QQmlWidget::remotePrintersToggled(const QString &enabled)
 *
 *  When \a enabled is set to "true", the dialog shows all printers including remote printers
 *  whereas when it is not set to "true", the dialog hides all the remote printers and shows
 *  only the local printers.
 */
void QQmlWidget::remotePrintersToggled(const QString &enabled)
{
    enabled.compare("true") == 0 ? unhide_remote_cups_printers(f) : hide_remote_cups_printers(f);
}

void QQmlWidget::orientationChanged(const QString &orientation)
{
    //preview->setOrientation(orientation);
    add_setting_to_printer(p,
                           QString("orientation-requested").toLatin1().data(),
                           orientation.toLatin1().data());
}

void QQmlWidget::newPageSizeSelected(const QString &pageSize)
{
    QStringList pageSizeSplitList = pageSize.split("_");
    QString size = pageSizeSplitList[2];
    QStringList sizeSplitList = size.split("x");

    qreal width = sizeSplitList[0].toDouble();

    QString unit = sizeSplitList[1].right(2);
    sizeSplitList[1].remove(unit);

    qreal height = sizeSplitList[1].toDouble();

    //preview->setPageSize(pageSizeSplitList[1], width, height, unit);
    add_setting_to_printer(p,
                           QString("media").toLatin1().data(),
                           pageSize.toLatin1().data());
}

void QQmlWidget::numCopiesChanged(const int copies)
{
    //preview->setNumCopies(copies);
    add_setting_to_printer(p,
                           QString("copies").toLatin1().data(),
                           QString::number(copies).toLatin1().data());
}

void QQmlWidget::collateToggled(const QString &enabled)
{
    //preview->setCollateCopies(enabled.compare("true") == 0);
}

void QQmlWidget::newPageRangeSet(const QString &pageRange)
{
    QString page(pageRange);
    add_setting_to_printer(p,
                           QString("page-ranges").toLatin1().data(),
                           page.remove('[').remove(']').toLatin1().data());
}

void QQmlWidget::setDuplexOption(const QString &duplexOption)
{
    add_setting_to_printer(p,
                           QString("sides").toLatin1().data(),
                           duplexOption.toLatin1().data());
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
    event_callback add_cb = (event_callback)(CallbackFunctions::add_printer_callback);
    event_callback rem_cb = (event_callback)(CallbackFunctions::remove_printer_callback);
    f = get_new_FrontendObj(NULL, add_cb, rem_cb);
    connect_to_dbus(f);
}

/*!
 *  \fn void QQmlWidget::addMaximumPrintCopies(char *_copies)
 *
 *  This function takes the range \a _copies for the number of copies and sets the maximum value
 *  allowed in the dialog.
 */
void QQmlWidget::addMaximumPrintCopies(char *_copies)
{
    QString copies(_copies);
    QStringList list = copies.split('-'); // copies is in this format: 1-9999

    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        obj->setProperty("maximumCopies", list[1].toInt());
    else
        qDebug() << "generalObject Not Found";
}

/*!
 *  \fn void QQmlWidget::updateStartJobsModel(char *startJobOption)
 *
 *  Many printers come with a hold option which specifies when the job should start after a user
 *  clicked the "Print" button in the UI. This function adds a new \a startJobOption to the
 *  existing model.
 */
void QQmlWidget::updateStartJobsModel(char *startJobOption)
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj, "updateStartJobsModel", Q_ARG(QVariant, startJobOption));
    else
        qDebug() << "jobsObject Not Found";
}

/*!
 *  \fn void QQmlWidget::clearStartJobsModel()
 *
 *  This function clears the model startJobModel which holds a list of printer-supported job hold
 *  options.
 */
void QQmlWidget::clearStartJobsModel()
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj, "clearStartJobsModel");
    else
        qDebug() << "jobsObject Not Found";
}

/*!
 *  \fn void QQmlWidget::updatePaperSizeModel(char *media, int isDefault)
 *
 *  Adds a new paper defined by \a media to the existing model paperSizeModel. The \a isDefault
 *  parameter checks if the given media is to be set as the default for the printer.
 */
void QQmlWidget::updatePaperSizeModel(const char *name, char *pwg_name, int isDefault)
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj,
                                  "updatePaperSizeModel",
                                  Q_ARG(QVariant, name),
                                  Q_ARG(QVariant, pwg_name),
                                  Q_ARG(QVariant, isDefault));
    else
        qDebug() << "generalObject Not Found";
}

/*!
 *  \fn void QQmlWidget::clearPaperSizeModel()
 *
 *  This function clears the model paperSizeModel which holds a list of printer-supported media
 *  options.
 */
void QQmlWidget::clearPaperSizeModel()
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj, "clearPaperSizeModel");
    else
        qDebug() << "generalObject Not Found";
}

void QQmlWidget::updatePagesPerSideModel(char *pages, int isDefault)
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj,
                                  "updatePagesPerSideModel",
                                  Q_ARG(QVariant, pages),
                                  Q_ARG(QVariant, isDefault));
    else
        qDebug() << "pageSetupObject Not Found";
}

void QQmlWidget::clearPagesPerSideModel()
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj, "clearPagesPerSideModel");
    else
        qDebug() << "pageSetupObject Not Found";
}

void QQmlWidget::enableTwoSided(char *option)
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj,
                                  "enableTwoSided",
                                  Q_ARG(QVariant, option));
    else
        qDebug() << "pageSetupObject Not Found";
}

void QQmlWidget::clearTwoSidedSwitch()
{
    QObject *obj = qvariant_cast<QObject *>(qmlWidget->rootObject()->findChild<QObject *>("pageLoader")->property("item"));
    if (obj)
        QMetaObject::invokeMethod(obj, "clearTwoSidedSwitch");
    else
        qDebug() << "pageSetupObject Not Found";
}

/*!
 *  \fn static void CallbackFunctions::CallbackFunctions(QObject *parent)
 *
 *  A singleton class which acts as a mediator between a static callback
 *  function and QCPDialog class
 */
CallbackFunctions::CallbackFunctions(QObject *parent):
    QObject (parent)
{
}

/*!
 *  \fn static void CallbackFunctions::add_printer_callback(PrinterObj *p)
 *
 *  Acts as a callback function whenever a new PrinterObj \a p is added.
 */
void CallbackFunctions::add_printer_callback(PrinterObj *p)
{
    cbf::Instance()->addPrinterSignal(p->name, p->id, p->backend_name);
}

/*!
 *  \fn static void CallbackFunctions::remove_printer_callback(PrinterObj *p)
 *
 *  Acts as a callback function whenever a PrinterObj \a p is removed.
 */
void CallbackFunctions::remove_printer_callback(PrinterObj *p)
{
    cbf::Instance()->removePrinterSignal(p->name);
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

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

#ifndef CPD_H
#define CPD_H

extern "C" {
    #include <cpdb-libs-frontend.h>
}

#include <QtWidgets>
#include <QQuickWidget>

#include "../headers/preview.h"
#include "../headers/singleton.h"

static inline int add_printer_callback(PrinterObj *p)
{
    printf("print_frontend.c : Printer %s added!\n", p->name);
    return 0;
}

static inline int remove_printer_callback(PrinterObj *p)
{
    printf("print_frontend.c : Printer %s removed!\n", p->name);
    return 0;
}

class QQmlWidget;
class QCpdWindow;

class QQmlWidget : public QWidget
{
    Q_OBJECT
public:
    QStringList jobsList;
    QStringList supportedResolutions;
    QStringList jobHoldOptionsList;
    FrontendObj *f;

    QQmlWidget(QWidget* parent = Q_NULLPTR);
    ~QQmlWidget();
    void resize(const QRect& rect);
    void clearPrinters();
    void addMaximumPrintCopies(char *_copies);
    void clearStartJobsModel();
    void clearPaperSizeModel();
    void clearPagesPerSideModel();
    void updatePagesPerSideModel(char *pages, int isDefault);
    void enableTwoSided(char *option);
    void clearTwoSidedSwitch();
    void updateStartJobsModel(char *startJobOption);
    void updatePaperSizeModel(const char *name, char *pwg_name, int isDefault);

private Q_SLOTS:
    void printDocument(QString printerName, QString backendName);
    void cpdQuit();
    void setJobsList(bool activeOnly);
    void setJobsHoldOptions(QString printerName);
    void setAdvancedOptions(QString printerName);
    void setResolutionSetting(QString resolutionValue, QString printerName);
    void cancelJob(int jobIndex, bool activeOnly);
    void addPrinter(char *printer_name, char *printer_id, char *backend_name);
    void removePrinter(char *printer_name);
    void newPrinterSelected(const QString &printer);
    void remotePrintersToggled(const QString &enabled);
    void orientationChanged(const QString &orientation);
    void newPageSizeSelected(const QString &pageSize);
    void numCopiesChanged(const int copies);
    void collateToggled(const QString &enabled);
    void newPageRangeSet(const QString &pageRange);
    void setDuplexOption(const QString &duplexOption);

private:
    QQuickWidget *qmlWidget;
    Job *jobStructArray;
    int numJobs;
    PrinterObj* p;

    void initBackend();
};

class QCpdWindow : public QMainWindow
{
public:
    QCpdWindow();
    ~QCpdWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QQmlWidget* qmlWidget;
    QCpdPreviewWidget *previewWidget;
    QPreviewToolbarWidget *previewToolbarWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *previewLayout;
};

class CallbackFunctions : public QObject
{
    Q_OBJECT
public:
    explicit CallbackFunctions(QObject *parent = 0);
    static void add_printer_callback(PrinterObj *p);
    static void remove_printer_callback(PrinterObj *p);

Q_SIGNALS:
    void addPrinterSignal(char *printer_name, char *printer_id, char *backend_name);
    void removePrinterSignal(char *printer_name);

};

typedef Singleton<CallbackFunctions> cbf;

#endif // CPD_H

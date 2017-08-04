#ifndef CPD_H
#define CPD_H

#include <QtWidgets>
#include <QQuickWidget>

#include "../headers/preview.h"

extern "C" {
    #include <CPDFrontend.h>
}

static inline int add_printer_callback(PrinterObj *p)
{
    printf("print_frontend.c : Printer %s added!\n", p->name);
}

static inline int remove_printer_callback(PrinterObj *p)
{
    printf("print_frontend.c : Printer %s removed!\n", p->name);
}

class QQmlWidget : public QWidget
{
    Q_OBJECT
public:
    QStringList jobsList;
    QStringList supportedResolutions;
    QStringList jobHoldOptionsList;
    FrontendObj *f;

    QQmlWidget(QWidget* parent = Q_NULLPTR);
    void resize(const QRect& rect);

public Q_SLOTS:
    void printDocument(QString printerName);
    void cpdQuit();
    void setJobsList(bool activeOnly);
    void setJobsHoldOptions(QString printerName);
    void setAdvancedOptions(QString printerName);

private:
    QQuickWidget *qmlWidget;

    void initBackend();
};

class QCpdWindow : public QMainWindow
{
public:
    QCpdWindow();
    ~QCpdWindow();

public Q_SLOT:
    void closeEvent(QCloseEvent *event) override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QQmlWidget* qmlWidget;
    QCpdPreviewWidget *previewWidget;
    QPreviewToolbarWidget *previewToolbarWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *previewLayout;
};

#endif // CPD_H

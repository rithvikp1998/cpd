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
    ~QQmlWidget();
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

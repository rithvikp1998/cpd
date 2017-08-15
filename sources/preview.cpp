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

#include <QPainter>
#include <QPrintPreviewWidget>
#include <QMessageLogger>
#include <QFile>
#include <QToolBar>
#include <QQuickWidget>
#include <QQuickItem>

#include <poppler/qt5/poppler-qt5.h>

#include "../headers/preview.h"

#define PREVIEW_SHRINK_FACTOR 0.6

/*!
 *  \class QCpdPreviewWidget
 *  \inmodule CPD
 *
 *  The dialog is comprised of three widgets - QQmlWidget, QCpdPreviewWidget and
 *  QPreviewToolbarWidget. This class acts as the preview window to the dialog. This widget derives
 *  from the QPrintPreviewWidget class by Qt. The class uses poppler to generate the image using
 *  the document and QPrinter's settings.
 */

/*!
 *  \fn QCpdPreviewWidget::QCpdPreviewWidget(QWidget* parent)
 *
 *  Constructs QCpdPreviewWidget objects with \a parent. The paintRequested signal is emitted
 *  when the QPrintPreviewWidget is constructed.
 */
QCpdPreviewWidget::QCpdPreviewWidget(QWidget* parent):
    QWidget(parent),
    printer(new QPrinter{}),
    previewWidget(new QPrintPreviewWidget(printer.get(), this))
{
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Portrait);
    printer->setFullPage(true);

    connect(previewWidget, SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));
}

/*!
 *  \fn QCpdPreviewWidget::~QCpdPreviewWidget()
 *
 *  Destroys QCpdPreviewWidget objects
 */
QCpdPreviewWidget::~QCpdPreviewWidget() = default;

/*!
 *  \fn void QCpdPreviewWidget::resize(const QRect& rect)
 *
 *  resize() takes a QRect& \a rect as a parameter and uses it to resize the QQmlWidget objects
 *  to the same dimensions as the parameter rect
 */
void QCpdPreviewWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    previewWidget->resize(rect.width(), rect.height());
}

/*!
 *  \fn void QCpdPreviewWidget::print(QPrinter *printer)
 *
 *  The function acts as a slot for the paintRequested signal emitted by QPrintPreviewWidget.
 *  The function uses poppler and QPrinter \a printer settings to generated and paint the image.
 */
void QCpdPreviewWidget::print(QPrinter *printer)
{
    QPainter painter(printer);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform, true);

    QFile f;
    f.setFileName(":/test.pdf");
    f.open(QIODevice::ReadOnly);
    QByteArray pdf = f.readAll();

    Poppler::Document *document = Poppler::Document::loadFromData(pdf);
    if (!document)
        qCritical("File '%s' does not exist!", qUtf8Printable(":/test.pdf"));
    if (document->isLocked())
        qCritical("File %s is locked!", qUtf8Printable(":/test.pdf"));

    pageCount = document->numPages();

    Poppler::Page *page = document->page(pageNumber);
    if (page == nullptr)
        qCritical("File '%s' is empty?", qUtf8Printable(":/test.pdf"));

    QImage image = page->renderToImage(72.0,72.0,0,0,page->pageSize().width(),page->pageSize().height());
    if (image.isNull())
        qCritical("Error!");

    paperHeight = page->pageSize().height();
    previewPainted = 1;

    painter.drawImage(0,0,image,0,0,0,0,0);
    painter.end();
}

/*!
 *  \fn void QCpdPreviewWidget::showNextPage()
 *
 *  This function acts as a slot for the nextPageButtonClicked signal emitted from
 *  pages/preview_toolbar.qml and shows the next page in the document in the preview window.
 */
void QCpdPreviewWidget::showNextPage()
{
    pageNumber = pageNumber < (pageCount-1) ? pageNumber+1 : pageNumber;
    previewWidget->updatePreview();
}

/*!
 *  \fn void QCpdPreviewWidget::showPrevPage()
 *
 *  This function acts as a slot for the prevPageButtonClicked signal emitted from
 *  pages/preview_toolbar.qml and shows the previous page in the document in the preview window.
 */
void QCpdPreviewWidget::showPrevPage()
{
    pageNumber = pageNumber > 0 ? pageNumber-1 : pageNumber;
    previewWidget->updatePreview();
}

/*!
 *  \fn void QCpdPreviewWidget::setZoom(qreal zoomFactor)
 *
 *  This function acts as a slot for the zoomSliderValueChanged signal emitted from
 *  pages/preview_toolbar.qml and sets the size of the page shown in the preview according to
 *  \a zoomFactor
 */
void QCpdPreviewWidget::setZoom(qreal zoomFactor)
{
    if(previewPainted)
        previewWidget->setZoomFactor(zoomFactor * PREVIEW_SHRINK_FACTOR * (widgetHeight/paperHeight));
    previewWidget->updatePreview();
    currentZoomFactor = zoomFactor;
}

/*!
 *  \class QPreviewToolbarWidget
 *  \inmodule CPD
 *
 *  The dialog is comprised of three widgets - QQmlWidget, QCpdPreviewWidget and
 *  QPreviewToolbarWidget. QPreviewToolbarWidget creates a QQuickWidget with
 *  pages/preview_toolbar.qml as the root and handles all the signals from the qml file.
 */

/*!
 *  \fn QPreviewToolbarWidget::QPreviewToolbarWidget(QWidget* parent)
 *
 *  Constructs QPreviewToolbarWidget objects with \a parent
 */
QPreviewToolbarWidget::QPreviewToolbarWidget(QWidget* parent):
        QWidget(parent),
        previewToolbarWidget(new QQuickWidget(QUrl("qrc:/pages/preview_toolbar.qml"), this))
{
    previewToolbarWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    previewToolbarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbarRootObject = previewToolbarWidget->rootObject();
}

/*!
 *  \fn QPreviewToolbarWidget::~QPreviewToolbarWidget()
 *
 *  Destroys QPreviewToolbarWidget objects
 */
QPreviewToolbarWidget::~QPreviewToolbarWidget() = default;

/*!
 *  \fn void QPreviewToolbarWidget::resize(const QRect& rect)
 *
 *  resize() takes a QRect& \a rect as a parameter and uses it to resize the QQmlWidget objects
 *  to the same dimensions as the parameter rect
 */
void QPreviewToolbarWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    previewToolbarWidget->resize(rect.width(), rect.height());
}

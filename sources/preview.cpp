#include <QPainter>
#include <QPrintPreviewWidget>
#include <QMessageLogger>
#include <QFile>
#include <QToolBar>
#include <QQuickWidget>
#include <QQuickItem>

#include <poppler/qt5/poppler-qt5.h>

#include "../headers/preview.h"

/* This method implements the existing QPrintPreviewWidget class from Qt */

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

QCpdPreviewWidget::~QCpdPreviewWidget() = default;

void QCpdPreviewWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    previewWidget->resize(rect.width(), rect.height());
}

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

void QCpdPreviewWidget::showNextPage()
{
    pageNumber = pageNumber < (pageCount-1) ? pageNumber+1 : pageNumber;
    previewWidget->updatePreview();
}

void QCpdPreviewWidget::showPrevPage()
{
    pageNumber = pageNumber > 0 ? pageNumber-1 : pageNumber;
    previewWidget->updatePreview();
}

void QCpdPreviewWidget::setZoom(qreal zoomFactor)
{
    if(previewPainted)
        previewWidget->setZoomFactor(zoomFactor * 0.6 * (widgetHeight/paperHeight));
    previewWidget->updatePreview();
    currentZoomFactor = zoomFactor;
}

QPreviewToolbarWidget::QPreviewToolbarWidget(QWidget* parent):
        QWidget(parent),
        previewToolbarWidget(new QQuickWidget(QUrl("qrc:/pages/preview_toolbar.qml"), this))
{
    previewToolbarWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    previewToolbarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbarRootObject = previewToolbarWidget->rootObject();
}

QPreviewToolbarWidget::~QPreviewToolbarWidget() = default;

void QPreviewToolbarWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    previewToolbarWidget->resize(rect.width(), rect.height());
}

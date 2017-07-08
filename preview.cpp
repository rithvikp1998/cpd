#include <QPainter>
#include <QPrintPreviewWidget>
#include <QMessageLogger>
#include <QFile>
#include <QToolBar>
#include <QQuickWidget>
#include <QQuickItem>

#include <poppler/qt5/poppler-qt5.h>

#include "preview.h"

/* This method implements the existing QPrintPreviewWidget class from Qt */

PrintPreviewWidget::PrintPreviewWidget(QWidget* parent):
    QWidget(parent),
    printer(new QPrinter{}),
    previewWidget(new QPrintPreviewWidget(printer.get(), this))
{
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Portrait);
    printer->setFullPage(true);

    connect(previewWidget, SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));
}

PrintPreviewWidget::~PrintPreviewWidget() = default;

void PrintPreviewWidget::print(QPrinter *printer){
    QPainter painter(printer);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform, true);

    QFile f;
    f.setFileName(":/test.pdf");
    f.open(QIODevice::ReadOnly);
    QByteArray pdf=f.readAll();

    Poppler::Document *document = Poppler::Document::loadFromData(pdf);
    if (!document)
        qCritical("File '%s' does not exist!", qUtf8Printable(":/test.pdf"));
    if (document->isLocked())
        qCritical("File %s is locked!", qUtf8Printable(":/test.pdf"));

    Poppler::Page *page = document->page(0);
    if (page == nullptr)
        qCritical("File '%s' is empty?", qUtf8Printable(":/test.pdf"));

    QImage image = page->renderToImage(72.0,72.0,0,0,page->pageSize().width(),page->pageSize().height());
    if (image.isNull())
        qCritical("Error!");

    painter.drawImage(0,0,image,0,0,0,0,0);
    painter.end();
}


void PrintPreviewWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    previewWidget->resize(rect.width(), rect.height());
}


PreviewToolbarWidget::PreviewToolbarWidget(QWidget* parent):
        QWidget(parent),
        previewToolbarWidget(new QQuickWidget(QUrl("qrc:/preview_toolbar.qml"), this))
{
    previewToolbarWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    previewToolbarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(previewToolbarWidget->rootObject(), SIGNAL(nextPageButtonClicked()), this, SLOT(showNextPage()));
    connect(previewToolbarWidget->rootObject(), SIGNAL(prevPageButtonClicked()), this, SLOT(showPrevPage()));
    connect(previewToolbarWidget->rootObject(), SIGNAL(zoomSliderValueChanged()), this, SLOT(setZoom()));
}

void PreviewToolbarWidget::resize(const QRect& rect)
{
    QWidget::resize(rect.width(), rect.height());
    previewToolbarWidget->resize(rect.width(), rect.height());
}

PreviewToolbarWidget::~PreviewToolbarWidget() = default;

void PreviewToolbarWidget::showNextPage()
{
    qCritical("Showing next page");
}

void PreviewToolbarWidget::showPrevPage()
{
    qCritical("Showing previous page");
}

void PreviewToolbarWidget::setZoom()
{
    qCritical("Setting zoom");
}

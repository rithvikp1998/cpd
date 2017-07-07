#include "preview.h"
#include <QPainter>
#include <QPrintPreviewWidget>
#include <poppler/qt5/poppler-qt5.h>
#include <QMessageLogger>
#include <QFile>

/* This method implements the existing QPrintPreviewWidget class from Qt */

PrintPreviewWidget::PrintPreviewWidget(QWidget* parent):
    QWidget(parent)
  , printer(new QPrinter{})
  , preview_widget(new QPrintPreviewWidget(printer.get(), this))
{
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Portrait);
    printer->setFullPage(true);

    connect(preview_widget, SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));
}

PrintPreviewWidget::~PrintPreviewWidget()
{
}

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
    preview_widget->resize(rect.width(), rect.height());
}

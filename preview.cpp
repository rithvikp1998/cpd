#include "preview.h"
#include <poppler/qt5/poppler-qt5.h>
#include <QMessageLogger>

/* The preview works by using poppler to convert a page of pdf to QImage */

QImage QPdfPreview::requestImage(const QString &id, QSize *size, const QSize &requested_size) {
    Poppler::Document *document = Poppler::Document::load("/tmp/test.pdf");
    if (!document || document->isLocked())
        qCritical("File '%s' does not exist or is locked!", qUtf8Printable("/tmp/test.pdf"));

    Poppler::Page *page = document->page(0);
    if (page == nullptr)
        qCritical("File '%s' is empty?", qUtf8Printable("/tmp/test.pdf"));

    QImage image = page->renderToImage(72.0,72.0,0,0,page->pageSize().width(),page->pageSize().height());
    if (image.isNull())
        qCritical("Error!");

    return image;
}

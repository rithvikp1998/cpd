#include "preview.h"
#include <poppler/qt5/poppler-qt5.h>
#include <QMessageLogger>
#include <QFile>

/* The preview works by using poppler to convert a page of pdf to QImage */

QImage QPdfPreview::requestImage(const QString &id, QSize *size, const QSize &requested_size) {
    int fileNameLength = id.lastIndexOf("/");
    QString fileName = ":/" + id.mid(0, fileNameLength);
    int pageNumber = id.mid(fileNameLength + 1, id.size()).toInt();
    QImage image;

    QFile f;
    f.setFileName(fileName);
    f.open(QIODevice::ReadOnly);
    QByteArray pdf=f.readAll();

    Poppler::Document *document = Poppler::Document::loadFromData(pdf);

    //Poppler::Document *document = Poppler::Document::load(fileName);
    if (!document){
        qCritical("File '%s' does not exist!", qUtf8Printable(fileName));
        return image;
    }

    if (document->isLocked()) {
        qCritical("File %s is locked!", qUtf8Printable(fileName));
    }

    Poppler::Page *page = document->page(pageNumber);
    if (page == nullptr){
        qCritical("File '%s' is empty?", qUtf8Printable(fileName));
        return image;
    }\

    image = page->renderToImage(72.0,72.0,0,0,page->pageSize().width(),page->pageSize().height());
    if (image.isNull())
        qCritical("Error!");

    return image;
}

int QPreviewData::get_number_of_pages(QString fileName){
    fileName = ":" + fileName;
    QFile f;
    f.setFileName(fileName);
    f.open(QIODevice::ReadOnly);
    QByteArray pdf=f.readAll();

    Poppler::Document *document = Poppler::Document::loadFromData(pdf);
    //Poppler::Document *document = Poppler::Document::load(fileName);
    if (!document || document->isLocked()){
        qCritical("File '%s' does not exist or is locked!", qUtf8Printable(fileName));
        return 0;
    }

    return document->numPages();
}

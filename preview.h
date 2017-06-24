#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>
#include <QQuickImageProvider>
#include <QtPrintSupport/QPrinter>

class QPdfPreview : public QQuickImageProvider
{
public:
    QPdfPreview() : QQuickImageProvider(QQuickImageProvider::Image) {}
    QImage requestImage(const QString &id, QSize *size, const QSize &requested_size) override;
};

class QPreviewData : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE int get_number_of_pages(QString fileName);
};

class PrintPreviewWidget : public QWidget
{
    Q_OBJECT
public slots:
    void print(QPrinter* printer);
public:
    PrintPreviewWidget();
};

#endif // PREVIEW_H

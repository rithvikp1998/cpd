#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>
#include <QQuickWidget>
#include <QQuickItem>

#include <memory>

class QPrinter;
class QPrintPreviewWidget;

class PrintPreviewWidget : public QWidget
{
    Q_OBJECT
public:
    PrintPreviewWidget(QWidget* parent = Q_NULLPTR);
    void resize(const QRect& rect);
    ~PrintPreviewWidget();
public Q_SLOTS:
    void print(QPrinter *printer);
    void showNextPage();
    void showPrevPage();
    void setZoom();
private:
    std::unique_ptr<QPrinter> printer;
    QPrintPreviewWidget *previewWidget;
    int pageNumber = 0;
    int pageCount = 0;
};

class PreviewToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    PreviewToolbarWidget(QWidget* parent = Q_NULLPTR);
    void resize(const QRect& rect);
    ~PreviewToolbarWidget();
    QQuickItem *toolbarRootObject;
private:
    QQuickWidget *previewToolbarWidget;
};

#endif // PREVIEW_H

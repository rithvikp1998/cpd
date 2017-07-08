#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>
#include <QQuickWidget>

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
public slots:
    void print(QPrinter *printer);
private:
    std::unique_ptr<QPrinter> printer;
    QPrintPreviewWidget *previewWidget;
};

class PreviewToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    PreviewToolbarWidget(QWidget* parent = Q_NULLPTR);
    void resize(const QRect& rect);
    ~PreviewToolbarWidget();
public slots:
    void showNextPage();
    void showPrevPage();
    void setZoom();
private:
    QQuickWidget *previewToolbarWidget;
};

#endif // PREVIEW_H

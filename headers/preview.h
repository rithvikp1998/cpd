#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>
#include <QQuickWidget>
#include <QQuickItem>

#include <memory>

class QPrinter;
class QPrintPreviewWidget;

class QCpdPreviewWidget : public QWidget
{
    Q_OBJECT
public:
    qreal widgetHeight = 0;
    qreal currentZoomFactor = 1;

    QCpdPreviewWidget(QWidget* parent = Q_NULLPTR);
    ~QCpdPreviewWidget();
    void resize(const QRect& rect);

public Q_SLOTS:
    void print(QPrinter *printer);
    void showNextPage();
    void showPrevPage();
    void setZoom(qreal zoomFactor);

private:
    std::unique_ptr<QPrinter> printer;
    QPrintPreviewWidget *previewWidget;
    int pageNumber = 0;
    int pageCount = 0;
    qreal paperHeight = 0;
    bool previewPainted = 0;
};

class QPreviewToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    QQuickItem *toolbarRootObject;

    QPreviewToolbarWidget(QWidget* parent = Q_NULLPTR);
    ~QPreviewToolbarWidget();
    void resize(const QRect& rect);

private:
    QQuickWidget *previewToolbarWidget;
};

#endif // PREVIEW_H

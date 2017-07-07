#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>

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

#endif // PREVIEW_H

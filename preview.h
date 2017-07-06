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
    ~PrintPreviewWidget();

public slots:
    void print(QPrinter *printer);
private:
    std::unique_ptr<QPrinter> printer;
    QPrintPreviewWidget *preview_widget;
};

#endif // PREVIEW_H

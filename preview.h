#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>
#include <QPrinter>
#include <QPrintPreviewWidget>

class PrintPreviewWidget : public QWidget
{
    Q_OBJECT
public:
    PrintPreviewWidget();
public slots:
    void print(QPrinter *printer);
private:
    QPrinter *printer;
    QPrintPreviewWidget *preview_widget;
};

#endif // PREVIEW_H

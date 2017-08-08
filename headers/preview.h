/****************************************************************************
**
**  $QT_BEGIN_LICENSE:GPL$
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>
**
**  $QT_END_LICENSE$
**
****************************************************************************/

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

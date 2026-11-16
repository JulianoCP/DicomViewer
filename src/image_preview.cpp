#include "image_preview.h"

#include <QPainter>
#include <QPixmap>

void ImagePreview::paintEvent(QPaintEvent* event)
{
    const QPixmap image = pixmap();

    if (image.isNull())
    {
        QLabel::paintEvent(event);
        return;
    }

    const QRect area = contentsRect();
    const QSize displaySize = image.size().scaled(area.size(), Qt::KeepAspectRatio);

    QRect target(QPoint(0, 0), displaySize);
    target.moveCenter(area.center());

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(target, image, image.rect());
}
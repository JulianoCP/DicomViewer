#include "image_preview.h"
#include <QPainter>
#include <QPixmap>
#include <QSizePolicy>

ImagePreview::ImagePreview(const QString& text, QWidget* parent): QLabel(text, parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setAlignment(Qt::AlignCenter);
    setTextFormat(Qt::PlainText);
}

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
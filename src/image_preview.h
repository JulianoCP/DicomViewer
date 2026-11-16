#pragma once

#include <QLabel>

class ImagePreview : public QLabel
{

protected:

    void paintEvent(QPaintEvent* event) override;

public:

    explicit ImagePreview(const QString& text, QWidget* parent = nullptr);

};
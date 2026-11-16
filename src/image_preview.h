#pragma once

#include <QLabel>

class ImagePreview : public QLabel
{

public:

    using QLabel::QLabel;

protected:

    void paintEvent(QPaintEvent* event) override;

};
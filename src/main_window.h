#pragma once

#include <QWidget>
#include "image_preview.h"

class MainWindow : public QWidget
{

protected:

    ImagePreview* PreviewReference = nullptr;

public:

    explicit MainWindow(QWidget* parent = nullptr);

private:

    void OpenDicomFile();

};
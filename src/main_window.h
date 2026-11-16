#pragma once

#include "image_preview.h"
#include <QWidget>

class DicomMetadataLabel;

class MainWindow : public QWidget
{

protected:

    ImagePreview* PreviewReference = nullptr;
    DicomMetadataLabel* MetadataLabel = nullptr;

private:

    void OpenDicomFile();

public:

    explicit MainWindow(QWidget* parent = nullptr);

};
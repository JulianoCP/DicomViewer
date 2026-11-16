#pragma once

#include "image_preview.h"
#include <QWidget>
#include <memory>

class DicomImage;
class DcmFileFormat;
class DicomMetadataLabel;

class MainWindow : public QWidget
{

protected:

    ImagePreview* PreviewReference = nullptr;
    DicomMetadataLabel* MetadataLabel = nullptr;

private:

    std::unique_ptr<DcmFileFormat> CurrentFile;
    std::unique_ptr<DicomImage> CurrentImage;

private:

    void OpenDicomFile();
    bool RenderImage(DicomImage& image);

public:

    explicit MainWindow(QWidget* parent = nullptr); ~MainWindow() override;

};
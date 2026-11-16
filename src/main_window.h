#pragma once

#include "image_preview.h"
#include <QWidget>
#include <memory>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>

class QHBoxLayout;
class QDoubleSpinBox;
class DicomMetadataLabel;

class MainWindow : public QWidget
{

protected:

    QWidget* WindowControls = nullptr;
    QDoubleSpinBox* LevelInput = nullptr;
    QDoubleSpinBox* WidthInput = nullptr;
    QHBoxLayout* ControlsLayout = nullptr;

    ImagePreview* PreviewReference = nullptr;
    DicomMetadataLabel* MetadataLabel = nullptr;

private:

    double WindowWidth = 0.0;
    double WindowCenter = 0.0;

    std::unique_ptr<DcmFileFormat> CurrentFile;
    std::unique_ptr<DicomImage> CurrentImage;

private:

    void OpenDicomFile();
    void ApplyWindowSettings();
    bool RenderImage(DicomImage& image);

public:

    explicit MainWindow(QWidget* parent = nullptr); ~MainWindow() override = default;

};
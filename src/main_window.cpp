#include "main_window.h"
#include "image_preview.h"
#include "dicom_metadata_label.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QByteArray>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent)
{
    setWindowTitle("Visualizador DICOM");
    resize(1280, 720);

    auto* layout = new QVBoxLayout(this);
    auto* openButton = new QPushButton("Abrir DICOM");

    MetadataLabel = new DicomMetadataLabel(this);
    PreviewReference = new ImagePreview("Selecione uma imagem DICOM");

    WindowControls = new QWidget(this);
    ControlsLayout = new QHBoxLayout(WindowControls);
    LevelInput = new QDoubleSpinBox(WindowControls);

    LevelInput->setRange(-1000000.0, 1000000.0);
    LevelInput->setDecimals(2);
    LevelInput->setKeyboardTracking(false);

    WidthInput = new QDoubleSpinBox(WindowControls);
    WidthInput->setRange(1.0, 1000000.0);
    WidthInput->setDecimals(2);
    WidthInput->setKeyboardTracking(false);

    ControlsLayout->addStretch();
    ControlsLayout->setContentsMargins(0, 0, 0, 0);
    ControlsLayout->addWidget(new QLabel("Window Level", WindowControls));
    ControlsLayout->addWidget(LevelInput);
    ControlsLayout->addWidget(new QLabel("Window Width", WindowControls));
    ControlsLayout->addWidget(WidthInput);
    ControlsLayout->addStretch();

    WindowControls->hide();

    connect(LevelInput, &QDoubleSpinBox::valueChanged, this, &MainWindow::ApplyWindowSettings);
    connect(WidthInput, &QDoubleSpinBox::valueChanged, this, &MainWindow::ApplyWindowSettings);

    layout->addWidget(MetadataLabel);
    layout->addWidget(PreviewReference, 1);
    layout->addWidget(WindowControls);
    layout->addWidget(openButton);

    connect(openButton, &QPushButton::clicked, this, &MainWindow::OpenDicomFile);
}

void MainWindow::OpenDicomFile()
{
    const QString path = QFileDialog::getOpenFileName(this, "Abrir DICOM", {}, "DICOM (*.dcm);;Todos os arquivos (*)");

    if (path.isEmpty())
    {
        return;
    }

    MetadataLabel->hide();
    WindowControls->hide();

    CurrentImage.reset();
    CurrentFile = std::make_unique<DcmFileFormat>();

    const QByteArray filePath = path.toLocal8Bit();
    const OFCondition loadStatus = CurrentFile->loadFile(filePath.constData());

    if (loadStatus.bad())
    {
        PreviewReference->setText(QString("Erro ao ler o DICOM: %1").arg(loadStatus.text()));
        CurrentFile.reset();
        return;
    }

    DcmDataset* dataset = CurrentFile->getDataset();
    CurrentImage = std::make_unique<DicomImage>(dataset, dataset->getOriginalXfer());

    if (CurrentImage->getStatus() != EIS_Normal)
    {
        PreviewReference->setText(QString("Erro ao ler o DICOM: %1").arg(DicomImage::getString(CurrentImage->getStatus())));
        CurrentImage.reset();
        CurrentFile.reset();
        return;
    }

    const bool hasWindowPreset = CurrentImage->getWindowCount() > 0;
    hasWindowPreset ? CurrentImage->setWindow(0) : CurrentImage->setMinMaxWindow();

    if (!RenderImage(*CurrentImage))
    {
        CurrentImage.reset();
        CurrentFile.reset();
        return;
    }

    MetadataLabel->ShowMetadata(*dataset,static_cast<int>(CurrentImage->getWidth()), static_cast<int>(CurrentImage->getHeight()));

    if (CurrentImage->getWindow(WindowCenter, WindowWidth))
    {
        const QSignalBlocker blockLevel(LevelInput);
        const QSignalBlocker blockWidth(WidthInput);

        LevelInput->setValue(WindowCenter);
        WidthInput->setValue(WindowWidth);

        WindowControls->show();
    }
}

void MainWindow::ApplyWindowSettings()
{
    if (!CurrentImage)
    {
        return;
    }

    if (CurrentImage->setWindow(LevelInput->value(), WidthInput->value()))
    {
        RenderImage(*CurrentImage);
    }
}

bool MainWindow::RenderImage(DicomImage& image)
{
    const auto* pixels = static_cast<const uchar*>(image.getOutputData(8));

    if (!pixels)
    {
        PreviewReference->setText("Não foi possível gerar a imagem para exibição.");
        return false;
    }

    const int imageWidth = static_cast<int>(image.getWidth());
    const int imageHeight = static_cast<int>(image.getHeight());
    const QImage grayscale(pixels, imageWidth, imageHeight, imageWidth, QImage::Format_Grayscale8);

    PreviewReference->setPixmap(QPixmap::fromImage(grayscale.copy()));
    return true;
}
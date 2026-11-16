#include "main_window.h"
#include "image_preview.h"
#include "dicom_metadata_label.h"
#include <QByteArray>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent)
{
    setWindowTitle("Visualizador DICOM");
    resize(800, 600);

    auto* layout = new QVBoxLayout(this);
    auto* openButton = new QPushButton("Abrir DICOM");

    PreviewReference = new ImagePreview("Selecione uma imagem DICOM");
    MetadataLabel = new DicomMetadataLabel(this);

    layout->addWidget(PreviewReference, 1);
    layout->addWidget(MetadataLabel);
    layout->addWidget(openButton);

    connect(openButton, &QPushButton::clicked, this, &MainWindow::OpenDicomFile);
}

MainWindow::~MainWindow() = default;

void MainWindow::OpenDicomFile()
{
    const QString path = QFileDialog::getOpenFileName(this, "Abrir DICOM", {}, "DICOM (*.dcm);;Todos os arquivos (*)");

    if (path.isEmpty())
    {
        return;
    }

    MetadataLabel->hide();

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
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

void MainWindow::OpenDicomFile()
{
    const QString path = QFileDialog::getOpenFileName(this, "Abrir DICOM", {}, "DICOM (*.dcm);;Todos os arquivos (*)");

    if (path.isEmpty())
    {
        return;
    }

    MetadataLabel->hide();
    DcmFileFormat dicomFile;

    const QByteArray filePath = path.toLocal8Bit();
    const OFCondition loadStatus = dicomFile.loadFile(filePath.constData());

    if (loadStatus.bad())
    {
        PreviewReference->setText(QString("Erro ao ler o DICOM: %1").arg(loadStatus.text()));
        return;
    }

    DcmDataset* dataset = dicomFile.getDataset();
    DicomImage image(dataset, dataset->getOriginalXfer());

    if (image.getStatus() != EIS_Normal)
    {
        PreviewReference->setText(QString("Erro ao ler o DICOM: %1").arg(DicomImage::getString(image.getStatus())));
        return;
    }

    const bool hasWindowPreset = image.getWindowCount() > 0;
    hasWindowPreset ? image.setWindow(0) : image.setMinMaxWindow();

    const auto* pixels = static_cast<const uchar*>(image.getOutputData(8));

    if (!pixels)
    {
        PreviewReference->setText("Não foi possível gerar a imagem para exibição.");
        return;
    }

    const int imageWidth = static_cast<int>(image.getWidth());
    const int imageHeight = static_cast<int>(image.getHeight());
    const QImage grayscale(pixels, imageWidth, imageHeight, imageWidth, QImage::Format_Grayscale8);

    PreviewReference->setPixmap(QPixmap::fromImage(grayscale.copy()));
    MetadataLabel->ShowMetadata(*dataset, imageWidth, imageHeight);
}
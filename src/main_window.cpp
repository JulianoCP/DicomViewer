#include "main_window.h"
#include "image_preview.h"

#include <QByteArray>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <dcmtk/dcmimgle/dcmimage.h>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent)
{
    setWindowTitle("Visualizador DICOM");
    resize(1280, 720);

    auto* layout = new QVBoxLayout(this);
    auto* openButton = new QPushButton("Abrir DICOM");

    PreviewReference = new ImagePreview("Selecione uma imagem DICOM");
    PreviewReference->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    PreviewReference->setAlignment(Qt::AlignCenter);
    PreviewReference->setTextFormat(Qt::PlainText);

    layout->addWidget(PreviewReference, 1);
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

    const QByteArray filePath = path.toLocal8Bit();
    DicomImage image(filePath.constData());

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
}
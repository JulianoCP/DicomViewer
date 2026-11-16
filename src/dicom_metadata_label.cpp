#include "dicom_metadata_label.h"
#include <dcmtk/dcmdata/dctk.h>

DicomMetadataLabel::DicomMetadataLabel(QWidget* parent) : QLabel(parent)
{
    setAlignment(Qt::AlignCenter);
    setTextFormat(Qt::PlainText);
    setWordWrap(true);
    hide();
}

void DicomMetadataLabel::ShowMetadata(DcmDataset& dataset, int imageWidth, int imageHeight)
{
    const auto readTag = [&dataset](const DcmTagKey& tag) -> QString
    {
        OFString value;

        if (dataset.findAndGetOFString(tag, value).good() && !value.empty())
        {
            return QString::fromLatin1(value.c_str());
        }

        return QStringLiteral("—");
    };

    setText(QStringLiteral("Modalidade: %1 | Dimensões: %2 × %3 | Fotometria: %4 | Bits armazenados: %5").arg(readTag(DCM_Modality), QString::number(imageWidth), QString::number(imageHeight), readTag(DCM_PhotometricInterpretation), readTag(DCM_BitsStored)));
    show();
}
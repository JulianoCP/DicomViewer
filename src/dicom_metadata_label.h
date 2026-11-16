#pragma once

#include <QLabel>

class DcmDataset;

class DicomMetadataLabel : public QLabel
{

public:

    void ShowMetadata(DcmDataset& dataset, int imageWidth, int imageHeight);

public:

    explicit DicomMetadataLabel(QWidget* parent = nullptr);

};
#include "main_window.h"
#include <QApplication>
#include <dcmtk/dcmjpeg/djdecode.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    DJDecoderRegistration::registerCodecs();

    MainWindow window;
    window.show();

    const int result = app.exec();
    DJDecoderRegistration::cleanup();

    return result;
}
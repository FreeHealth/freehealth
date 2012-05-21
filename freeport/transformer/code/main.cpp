#include <QtGui>
#include <QFile>
#include <QApplication>
#include "window.h"

int main(int argc, char *argv[])
{

    QApplication app( argc, argv );
    Window w;
    w.show();

//    QFile file(sourcepath);

//    //QFile file("C:\\Qt-Book\\Test5.xml");
//    file.open(QIODevice::ReadWrite );
//    XmlWriter xw( &file );
//    xw.setAutoNewLine( true );
//    xw.writeRaw("<?xml version=\"1.0\" encoding =\"UTF-8\"?>");
//    xw.newLine();
//    xw.writeRaw( "<!DOCTYPE FreeMedForms>" );
//    xw.newLine();
//    xw.writeOpenTag("DataIntegrator");

//    //Patient Block :
//    Patient patienttest;
//    SaxHandler handler(&patienttest, &file);
//    handler.readFile(destpath+"\\FMF_FPatient.xml");
//    //handler.readFile("C:\\Qt-Book\\SAX HANDLER\\OMNIS_FPatient2.xml");


//    xw.writeCloseTag("DataIntegrator");
    return app.exec();
}

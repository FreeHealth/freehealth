#include "tkPrinterTester.h"

#include <tkPrinter.h>
//#include "../../libs/toolkit/widgets/tkPrinterPreviewer_p.h"

#include <QDir>
#include <tkGlobal.h>
#include <QDebug>
#include <QApplication>


tkPrinterTester::tkPrinterTester( QMainWindow *win ) :
        w(win)
{
    if (!w)
        w = new QMainWindow();
//    previewerTest();
     test1();
}


void tkPrinterTester::test1()
{
    QDir dir(qApp->applicationDirPath());
    if (tkGlobal::isRunningOnMac())
        dir.cd("../../../");
    QString header = tkGlobal::readTextFile(":/header.html");
    QString header2 = tkGlobal::readTextFile(":/header_2.html");
    QString footer = tkGlobal::readTextFile(":/footer.html");
    QString footer2 = tkGlobal::readTextFile(":/footer_2.html");
    QString watermark = tkGlobal::readTextFile(":/watermark.html");
    QPixmap pixWatermark;
    pixWatermark.load(":/pixmapWatermark.png");
    QString document = tkGlobal::readTextFile(":/document.html");

//    QTextDocument td(document);
//    qWarning() << "document blockCount" << td.blockCount();
//
//    QTextFrame::iterator it;
//    int i = 0;
//    QTextFrame *child=0;
//    for (it = td.rootFrame()->begin(); !(it.atEnd()); ++it) {
//        ++i;
//    }
//    qWarning() << "document frameCount" << i;

    tkPrinter pe;
    pe.askForPrinter();
    pe.printer()->setFromTo(3,3);
    pe.addTextWatermark( "Adding a plain text\nWATERMARK", tkPrinter::EvenPages, Qt::AlignCenter, Qt::AlignCenter, QFont(), QColor("lightgrey") );
    pe.addTextWatermark( "Printed with DrugsInteractions", tkPrinter::EvenPages, Qt::AlignBottom, Qt::AlignCenter, QFont(), QColor("lightgrey") );
    pe.addPixmapWatermark( pixWatermark, tkPrinter::EachPages, Qt::AlignTop );
//    pe.setHeader( header , tkPrinter::FirstPageOnly );
//    pe.setHeader( header );
//    pe.setHeader( header2, tkPrinter::EachPages );
    pe.setHeader( header2 );
    pe.setFooter( footer );
//    pe.setFooter( footer2 );
//    pe.setFooter( footer );
    pe.setContent( document );
    pe.setOrientation(QPrinter::Portrait);
    pe.setPaperSize(QPrinter::A4);
    pe.previewDialog();
}

void tkPrinterTester::previewerTest()
{
    tkPrinter printer;
    previewer = printer.previewer(w);
    qWarning() << previewer;
    previewer->initialize();
    previewer->setHeader( "" );
    previewer->setFooter( "" );
    previewer->setWatermark("");
    w->setCentralWidget(previewer);
    w->show();
}

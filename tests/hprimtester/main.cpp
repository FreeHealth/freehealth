#include "../../libs/utils/hprimparser.h"
#include "../../libs/utils/global.h"

#include <QCoreApplication>
#include <QString>
#include <QRegExp>
#include <QHash>

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

                         qWarning() << "Testing wrong encoding correction";

            QString file = app.applicationDirPath();
    if (Utils::isRunningOnMac())
        file += "/../../..";
    file += "/tests/wrong_encoding.hpr";


    // Read the test file with a wrong encoding
    QString content = Utils::readTextFile(file, "MacRoman");
//    for(int i=368; i < 380; ++i)
//        qWarning() << i << content.at(i).unicode() << content.at(i);

//    QString s;
//    s.append(QChar(195));
//    s.append(QChar(160));
//    QChar c(195, 160);
//    qWarning() << c << s;

    qWarning() << Utils::correctTextAccentEncoding(content);

            //    content = QString::fromUtf8(ba);
            //    qWarning() << content;

            // frÃ¨rdÃ©n Ã  prolactine traitÃ© mÃ©dicalement de 2006 Ã  2010 ainsi qu'un nodule thyroÃ¯dien de 14mm de dÃ©couverte fortuite.


//            Utils::HPRIM::HprimMessage msg = Utils::HPRIM::parseHprimRawSource(content);

    //    qWarning() << msg.rawContent().rawSource();

    return 0;
}

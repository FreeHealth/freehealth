#include "../../libs/utils/widgets/qbuttonlineedit.h"

#include <QApplication>
#include <QIcon>

#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Utils::QButtonLineEdit *line = new Utils::QButtonLineEdit;
    line->setRightIcon(QIcon("/Volumes/RamDisk/eric/freemedforms/global_resources/pixmap/16x16/ok.png"));
    line->show();

    return a.exec();
}

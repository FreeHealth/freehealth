#include "../../libs/utils/widgets/qbuttonlineedit.h"

#include <QApplication>
#include <QIcon>
#include <QtGui>

#include <QDebug>

const char * const ICONOK = "/Volumes/RamDisk/eric/freemedforms/global_resources/pixmap/16x16/ok.png";
const char * const ICONWARNING = "/Volumes/RamDisk/eric/freemedforms/global_resources/pixmap/16x16/warning.png";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget *w = new QWidget;
    QVBoxLayout *l = new QVBoxLayout(w);
    w->setLayout(l);

    Utils::QButtonLineEdit *line1 = new Utils::QButtonLineEdit;
    line1->setRightIcon(QIcon(ICONOK));
    line1->setPlaceholderText("Line 1 (Right icon test: one icon set)");

    Utils::QButtonLineEdit *line2 = new Utils::QButtonLineEdit;
    line2->setRightIcon(QIcon(ICONOK));
    line2->setRightIcon(QIcon(ICONWARNING));
    line2->setPlaceholderText("Line 2 (Right icon test: replacing ok icon with warning icon)");

    Utils::QButtonLineEdit *line3 = new Utils::QButtonLineEdit;
    line3->setLeftIcon(QIcon(ICONOK));
    line3->setPlaceholderText("Line 3 (Left icon test: one icon set)");

    Utils::QButtonLineEdit *line4 = new Utils::QButtonLineEdit;
    line4->setLeftIcon(QIcon(ICONOK));
    line4->setLeftIcon(QIcon(ICONWARNING));
    line4->setPlaceholderText("Line 4 (Left icon test: replacing ok icon with warning icon)");

    Utils::QButtonLineEdit *line5 = new Utils::QButtonLineEdit;
    line5->setLeftIcon(QIcon(ICONOK));
    line5->setRightIcon(QIcon(ICONOK));
    line5->setPlaceholderText("Line 5 (both ok)");

    Utils::QButtonLineEdit *line6 = new Utils::QButtonLineEdit;
    line6->setLeftIcon(QIcon(ICONOK));
    line6->setLeftIcon(QIcon(ICONWARNING));
    line6->setRightIcon(QIcon(ICONOK));
    line6->setRightIcon(QIcon(ICONWARNING));
    line6->setPlaceholderText("Line 6 (both warning)");

    l->addWidget(line1);
    l->addWidget(line2);
    l->addWidget(line3);
    l->addWidget(line4);
    l->addWidget(line5);
    l->addWidget(line6);
    w->setFocus();
    w->show();

    return a.exec();
}

#include "freetext.h"

FreeText::FreeText(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

FreeText::~FreeText(){}

QString FreeText::getFreeText()
{
    QString freeText;
    freeText = freeTextEdit->text();
    return freeText;
}

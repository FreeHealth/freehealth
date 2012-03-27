#include "freetext.h"

FreeText::FreeText(QWidget *parent)
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

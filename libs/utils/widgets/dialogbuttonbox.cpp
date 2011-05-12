#include "dialogbuttonbox.h"

#include <QToolButton>
#include <QCoreApplication>


DialogButtonBox::DialogButtonBox(QWidget *parent) :
    QDialogButtonBox(parent)
{
}

QAbstractButton *DialogButtonBox::addButton(const QString &trContext, const QString &unTrText, const QIcon &icon, ButtonRole role)
{
    QToolButton *but = new QToolButton(this);
    but->setText(QCoreApplication::translate(trContext.toUtf8(), unTrText.toUtf8()));
    but->setIcon(icon);
    return but;
}

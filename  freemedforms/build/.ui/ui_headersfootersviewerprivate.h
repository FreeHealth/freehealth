/********************************************************************************
** Form generated from reading ui file 'headersfootersviewerprivate.ui'
**
** Created: Mon Feb 1 11:54:10 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_HEADERSFOOTERSVIEWERPRIVATE_H
#define UI_HEADERSFOOTERSVIEWERPRIVATE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <tkRichTextEditor.h>

QT_BEGIN_NAMESPACE

class Ui_HeadersFootersViewerPrivate
{
public:
    QGridLayout *gridLayout;
    tkRichTextEditor *headerWidget;
    tkRichTextEditor *footerWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *HeadersFootersViewerPrivate)
    {
        if (HeadersFootersViewerPrivate->objectName().isEmpty())
            HeadersFootersViewerPrivate->setObjectName(QString::fromUtf8("HeadersFootersViewerPrivate"));
        HeadersFootersViewerPrivate->resize(600, 300);
        gridLayout = new QGridLayout(HeadersFootersViewerPrivate);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        headerWidget = new tkRichTextEditor(HeadersFootersViewerPrivate);
        headerWidget->setObjectName(QString::fromUtf8("headerWidget"));

        gridLayout->addWidget(headerWidget, 0, 0, 1, 1);

        footerWidget = new tkRichTextEditor(HeadersFootersViewerPrivate);
        footerWidget->setObjectName(QString::fromUtf8("footerWidget"));

        gridLayout->addWidget(footerWidget, 1, 0, 1, 1);

        buttonBox = new QDialogButtonBox(HeadersFootersViewerPrivate);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 1);


        retranslateUi(HeadersFootersViewerPrivate);
        QObject::connect(buttonBox, SIGNAL(accepted()), HeadersFootersViewerPrivate, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), HeadersFootersViewerPrivate, SLOT(reject()));

        QMetaObject::connectSlotsByName(HeadersFootersViewerPrivate);
    } // setupUi

    void retranslateUi(QDialog *HeadersFootersViewerPrivate)
    {
        HeadersFootersViewerPrivate->setWindowTitle(QApplication::translate("HeadersFootersViewerPrivate", "Headers and Footers Editor", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(HeadersFootersViewerPrivate);
    } // retranslateUi

};

namespace Ui {
    class HeadersFootersViewerPrivate: public Ui_HeadersFootersViewerPrivate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HEADERSFOOTERSVIEWERPRIVATE_H

/********************************************************************************
** Form generated from reading ui file 'mfInteractionDialog.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MFINTERACTIONDIALOG_H
#define UI_MFINTERACTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>

namespace DrugsWidget {
namespace Internal {

class Ui_InteractionDialog
{
public:
    QGridLayout *gridLayout;
    QFrame *line;
    QTextBrowser *textBrowser;
    QHBoxLayout *horizontalLayout;
    QPushButton *helpButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *printButton;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QPushButton *zoomIn;
    QPushButton *zoomOut;

    void setupUi(QDialog *DrugsWidget__Internal__InteractionDialog)
    {
        if (DrugsWidget__Internal__InteractionDialog->objectName().isEmpty())
            DrugsWidget__Internal__InteractionDialog->setObjectName(QString::fromUtf8("DrugsWidget__Internal__InteractionDialog"));
        DrugsWidget__Internal__InteractionDialog->resize(400, 300);
        gridLayout = new QGridLayout(DrugsWidget__Internal__InteractionDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        line = new QFrame(DrugsWidget__Internal__InteractionDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 3, 0, 1, 1);

        textBrowser = new QTextBrowser(DrugsWidget__Internal__InteractionDialog);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

        gridLayout->addWidget(textBrowser, 4, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        helpButton = new QPushButton(DrugsWidget__Internal__InteractionDialog);
        helpButton->setObjectName(QString::fromUtf8("helpButton"));

        horizontalLayout->addWidget(helpButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        printButton = new QPushButton(DrugsWidget__Internal__InteractionDialog);
        printButton->setObjectName(QString::fromUtf8("printButton"));

        horizontalLayout->addWidget(printButton);

        pushButton = new QPushButton(DrugsWidget__Internal__InteractionDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);


        gridLayout->addLayout(horizontalLayout, 5, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(DrugsWidget__Internal__InteractionDialog);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label);

        zoomIn = new QPushButton(DrugsWidget__Internal__InteractionDialog);
        zoomIn->setObjectName(QString::fromUtf8("zoomIn"));
        zoomIn->setMinimumSize(QSize(20, 20));
        zoomIn->setMaximumSize(QSize(20, 20));

        horizontalLayout_2->addWidget(zoomIn);

        zoomOut = new QPushButton(DrugsWidget__Internal__InteractionDialog);
        zoomOut->setObjectName(QString::fromUtf8("zoomOut"));
        zoomOut->setMinimumSize(QSize(20, 20));
        zoomOut->setMaximumSize(QSize(20, 20));

        horizontalLayout_2->addWidget(zoomOut);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__InteractionDialog);
        QObject::connect(pushButton, SIGNAL(clicked()), DrugsWidget__Internal__InteractionDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__InteractionDialog);
    } // setupUi

    void retranslateUi(QDialog *DrugsWidget__Internal__InteractionDialog)
    {
        DrugsWidget__Internal__InteractionDialog->setWindowTitle(QApplication::translate("DrugsWidget::Internal::InteractionDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        helpButton->setText(QApplication::translate("DrugsWidget::Internal::InteractionDialog", "Help", 0, QApplication::UnicodeUTF8));
        printButton->setText(QApplication::translate("DrugsWidget::Internal::InteractionDialog", "Print", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("DrugsWidget::Internal::InteractionDialog", "Close", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DrugsWidget::Internal::InteractionDialog", "Synthetic interactions", 0, QApplication::UnicodeUTF8));
        zoomIn->setText(QString());
        zoomOut->setText(QString());
        Q_UNUSED(DrugsWidget__Internal__InteractionDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class InteractionDialog: public Ui_InteractionDialog {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_MFINTERACTIONDIALOG_H

/********************************************************************************
** Form generated from reading ui file 'drugsuseroptionspage.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DRUGSUSEROPTIONSPAGE_H
#define UI_DRUGSUSEROPTIONSPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace DrugsWidget {
namespace Internal {

class Ui_DrugsUserWidget
{
public:
    QGridLayout *gridLayout_4;
    QLabel *label;
    QFrame *line;
    QVBoxLayout *userLayout;

    void setupUi(QWidget *DrugsWidget__Internal__DrugsUserWidget)
    {
        if (DrugsWidget__Internal__DrugsUserWidget->objectName().isEmpty())
            DrugsWidget__Internal__DrugsUserWidget->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DrugsUserWidget"));
        DrugsWidget__Internal__DrugsUserWidget->resize(540, 502);
        gridLayout_4 = new QGridLayout(DrugsWidget__Internal__DrugsUserWidget);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        label = new QLabel(DrugsWidget__Internal__DrugsUserWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label, 0, 0, 1, 1);

        line = new QFrame(DrugsWidget__Internal__DrugsUserWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_4->addWidget(line, 1, 0, 1, 1);

        userLayout = new QVBoxLayout();
        userLayout->setObjectName(QString::fromUtf8("userLayout"));

        gridLayout_4->addLayout(userLayout, 2, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__DrugsUserWidget);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DrugsUserWidget);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DrugsUserWidget)
    {
        DrugsWidget__Internal__DrugsUserWidget->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DrugsUserWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DrugsWidget::Internal::DrugsUserWidget", "Drugs User Preferences", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DrugsUserWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DrugsUserWidget: public Ui_DrugsUserWidget {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_DRUGSUSEROPTIONSPAGE_H

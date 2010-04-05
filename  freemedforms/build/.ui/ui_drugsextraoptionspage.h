/********************************************************************************
** Form generated from reading ui file 'drugsextraoptionspage.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DRUGSEXTRAOPTIONSPAGE_H
#define UI_DRUGSEXTRAOPTIONSPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <texteditorplugin/texteditor.h>

namespace DrugsWidget {
namespace Internal {

class Ui_DrugsExtraWidget
{
public:
    QGridLayout *gridLayout_4;
    QLabel *label;
    QFrame *line;
    Editor::TextEditor *ALDAfter;
    Editor::TextEditor *ALDBefore;
    QCheckBox *hideLabCheck;
    QLabel *label_15;
    QLabel *label_16;

    void setupUi(QWidget *DrugsWidget__Internal__DrugsExtraWidget)
    {
        if (DrugsWidget__Internal__DrugsExtraWidget->objectName().isEmpty())
            DrugsWidget__Internal__DrugsExtraWidget->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DrugsExtraWidget"));
        DrugsWidget__Internal__DrugsExtraWidget->resize(540, 502);
        gridLayout_4 = new QGridLayout(DrugsWidget__Internal__DrugsExtraWidget);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        label = new QLabel(DrugsWidget__Internal__DrugsExtraWidget);
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

        line = new QFrame(DrugsWidget__Internal__DrugsExtraWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_4->addWidget(line, 1, 0, 1, 1);

        ALDAfter = new Editor::TextEditor(DrugsWidget__Internal__DrugsExtraWidget);
        ALDAfter->setObjectName(QString::fromUtf8("ALDAfter"));

        gridLayout_4->addWidget(ALDAfter, 7, 0, 1, 1);

        ALDBefore = new Editor::TextEditor(DrugsWidget__Internal__DrugsExtraWidget);
        ALDBefore->setObjectName(QString::fromUtf8("ALDBefore"));

        gridLayout_4->addWidget(ALDBefore, 5, 0, 1, 1);

        hideLabCheck = new QCheckBox(DrugsWidget__Internal__DrugsExtraWidget);
        hideLabCheck->setObjectName(QString::fromUtf8("hideLabCheck"));

        gridLayout_4->addWidget(hideLabCheck, 2, 0, 1, 1);

        label_15 = new QLabel(DrugsWidget__Internal__DrugsExtraWidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_15->sizePolicy().hasHeightForWidth());
        label_15->setSizePolicy(sizePolicy1);
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label_15->setFont(font1);

        gridLayout_4->addWidget(label_15, 4, 0, 1, 1);

        label_16 = new QLabel(DrugsWidget__Internal__DrugsExtraWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        sizePolicy1.setHeightForWidth(label_16->sizePolicy().hasHeightForWidth());
        label_16->setSizePolicy(sizePolicy1);
        label_16->setFont(font1);

        gridLayout_4->addWidget(label_16, 6, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__DrugsExtraWidget);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DrugsExtraWidget);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DrugsExtraWidget)
    {
        DrugsWidget__Internal__DrugsExtraWidget->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DrugsExtraWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DrugsWidget::Internal::DrugsExtraWidget", "Drugs Extra Preferences", 0, QApplication::UnicodeUTF8));
        hideLabCheck->setText(QApplication::translate("DrugsWidget::Internal::DrugsExtraWidget", "Hide laboratory name", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("DrugsWidget::Internal::DrugsExtraWidget", "Text before 100% therapeutic", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("DrugsWidget::Internal::DrugsExtraWidget", "Text after 100% therapeutic", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DrugsExtraWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DrugsExtraWidget: public Ui_DrugsExtraWidget {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_DRUGSEXTRAOPTIONSPAGE_H

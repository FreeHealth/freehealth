/********************************************************************************
** Form generated from reading ui file 'drugsviewoptionspage.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DRUGSVIEWOPTIONSPAGE_H
#define UI_DRUGSVIEWOPTIONSPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFontComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <utils/widgets/colorbuttonchooser.h>

namespace DrugsWidget {
namespace Internal {

class Ui_DrugsViewWidget
{
public:
    QGridLayout *gridLayout_4;
    QLabel *label;
    QFrame *line;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *label_5;
    QSpinBox *viewFontSizeSpin;
    QFontComboBox *viewFontCombo;
    QLabel *label_4;
    QLabel *label_6;
    QSpinBox *historicSizeSpin;
    QGroupBox *testerBox;
    QGridLayout *gridLayout_3;
    QLabel *label_2;
    QLineEdit *userNameEdit;
    QLabel *label_3;
    QLineEdit *userPasswordEdit;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label_7;
    QComboBox *levelOfWarningCombo;
    QCheckBox *showIconsCheck;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_5;
    Utils::ColorButtonChooser *backgroundDosagesAvailableButton;
    QCheckBox *useBackgroundForDosages;

    void setupUi(QWidget *DrugsWidget__Internal__DrugsViewWidget)
    {
        if (DrugsWidget__Internal__DrugsViewWidget->objectName().isEmpty())
            DrugsWidget__Internal__DrugsViewWidget->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DrugsViewWidget"));
        DrugsWidget__Internal__DrugsViewWidget->resize(467, 391);
        gridLayout_4 = new QGridLayout(DrugsWidget__Internal__DrugsViewWidget);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        label = new QLabel(DrugsWidget__Internal__DrugsViewWidget);
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

        line = new QFrame(DrugsWidget__Internal__DrugsViewWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_4->addWidget(line, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(DrugsWidget__Internal__DrugsViewWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy2);
        label_5->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(label_5, 1, 0, 1, 1);

        viewFontSizeSpin = new QSpinBox(groupBox_2);
        viewFontSizeSpin->setObjectName(QString::fromUtf8("viewFontSizeSpin"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(viewFontSizeSpin->sizePolicy().hasHeightForWidth());
        viewFontSizeSpin->setSizePolicy(sizePolicy3);
        viewFontSizeSpin->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(viewFontSizeSpin, 2, 1, 1, 1);

        viewFontCombo = new QFontComboBox(groupBox_2);
        viewFontCombo->setObjectName(QString::fromUtf8("viewFontCombo"));
        sizePolicy3.setHeightForWidth(viewFontCombo->sizePolicy().hasHeightForWidth());
        viewFontCombo->setSizePolicy(sizePolicy3);
        viewFontCombo->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(viewFontCombo, 1, 1, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy2.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy2);
        label_4->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(label_4, 2, 0, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 3, 0, 1, 1);

        historicSizeSpin = new QSpinBox(groupBox_2);
        historicSizeSpin->setObjectName(QString::fromUtf8("historicSizeSpin"));
        sizePolicy3.setHeightForWidth(historicSizeSpin->sizePolicy().hasHeightForWidth());
        historicSizeSpin->setSizePolicy(sizePolicy3);
        historicSizeSpin->setValue(20);

        gridLayout_2->addWidget(historicSizeSpin, 3, 1, 1, 1);


        gridLayout_4->addWidget(groupBox_2, 2, 0, 1, 1);

        testerBox = new QGroupBox(DrugsWidget__Internal__DrugsViewWidget);
        testerBox->setObjectName(QString::fromUtf8("testerBox"));
        testerBox->setCheckable(true);
        testerBox->setChecked(false);
        gridLayout_3 = new QGridLayout(testerBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_2 = new QLabel(testerBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy2);

        gridLayout_3->addWidget(label_2, 0, 0, 1, 1);

        userNameEdit = new QLineEdit(testerBox);
        userNameEdit->setObjectName(QString::fromUtf8("userNameEdit"));

        gridLayout_3->addWidget(userNameEdit, 0, 1, 1, 1);

        label_3 = new QLabel(testerBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy2.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy2);

        gridLayout_3->addWidget(label_3, 0, 2, 1, 1);

        userPasswordEdit = new QLineEdit(testerBox);
        userPasswordEdit->setObjectName(QString::fromUtf8("userPasswordEdit"));
        userPasswordEdit->setEnabled(false);

        gridLayout_3->addWidget(userPasswordEdit, 0, 3, 1, 1);


        gridLayout_4->addWidget(testerBox, 5, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer, 6, 0, 1, 1);

        groupBox = new QGroupBox(DrugsWidget__Internal__DrugsViewWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 0, 0, 1, 1);

        levelOfWarningCombo = new QComboBox(groupBox);
        levelOfWarningCombo->setObjectName(QString::fromUtf8("levelOfWarningCombo"));
        sizePolicy1.setHeightForWidth(levelOfWarningCombo->sizePolicy().hasHeightForWidth());
        levelOfWarningCombo->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(levelOfWarningCombo, 0, 1, 1, 1);

        showIconsCheck = new QCheckBox(groupBox);
        showIconsCheck->setObjectName(QString::fromUtf8("showIconsCheck"));

        gridLayout->addWidget(showIconsCheck, 1, 0, 1, 2);


        gridLayout_4->addWidget(groupBox, 3, 0, 1, 1);

        groupBox_3 = new QGroupBox(DrugsWidget__Internal__DrugsViewWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_5 = new QGridLayout(groupBox_3);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        backgroundDosagesAvailableButton = new Utils::ColorButtonChooser(groupBox_3);
        backgroundDosagesAvailableButton->setObjectName(QString::fromUtf8("backgroundDosagesAvailableButton"));
        backgroundDosagesAvailableButton->setMinimumSize(QSize(20, 20));
        backgroundDosagesAvailableButton->setMaximumSize(QSize(20, 20));

        gridLayout_5->addWidget(backgroundDosagesAvailableButton, 0, 1, 1, 1);

        useBackgroundForDosages = new QCheckBox(groupBox_3);
        useBackgroundForDosages->setObjectName(QString::fromUtf8("useBackgroundForDosages"));

        gridLayout_5->addWidget(useBackgroundForDosages, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_3, 4, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__DrugsViewWidget);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DrugsViewWidget);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DrugsViewWidget)
    {
        DrugsWidget__Internal__DrugsViewWidget->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Drugs View Preferences", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "View", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Font", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Font size", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Size of historic", 0, QApplication::UnicodeUTF8));
        testerBox->setTitle(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Testers", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Name", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Password", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Interactions warnings", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Level of warning", 0, QApplication::UnicodeUTF8));
        levelOfWarningCombo->clear();
        levelOfWarningCombo->insertItems(0, QStringList()
         << QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Maximal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Moderate", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Minimal", 0, QApplication::UnicodeUTF8)
        );
        showIconsCheck->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Show interactions icons in the prescription", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Available dosage", 0, QApplication::UnicodeUTF8));
        backgroundDosagesAvailableButton->setText(QString());
        useBackgroundForDosages->setText(QApplication::translate("DrugsWidget::Internal::DrugsViewWidget", "Background color for drugs with available dosages", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DrugsViewWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DrugsViewWidget: public Ui_DrugsViewWidget {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_DRUGSVIEWOPTIONSPAGE_H

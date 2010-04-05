/********************************************************************************
** Form generated from reading ui file 'druginfo.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DRUGINFO_H
#define UI_DRUGINFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace DrugsWidget {
namespace Internal {

class Ui_DrugInfo
{
public:
    QGridLayout *gridLayout_2;
    QLabel *drugName;
    QFrame *line;
    QDialogButtonBox *buttonBox;
    QTabWidget *chkAllIAM;
    QWidget *tab;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QListWidget *knownMols;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_3;
    QListWidget *DCI;
    QGroupBox *groupTestINN;
    QVBoxLayout *verticalLayout_4;
    QRadioButton *rbINNOk;
    QRadioButton *rbINNWrong;
    QLabel *label;
    QTextEdit *INNMessage;
    QPushButton *butSendINN;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_4;
    QListWidget *interactClass;
    QWidget *tab_2;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_3;
    QLabel *label_5;
    QListWidget *listWidgetInteractions;
    QTextBrowser *Info_textBrowser;
    QTextBrowser *CAT_textBrower;
    QLabel *label_6;
    QLabel *label_7;
    QGroupBox *groupTestIAM;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *chkAllInteractionsFound;
    QCheckBox *chkAllInteractionsOK;
    QCheckBox *chkAllIAMTextsOK;
    QCheckBox *chkAllCATTextsOK;
    QLabel *label_8;
    QTextEdit *InteractMessage;
    QPushButton *butIAMSend;

    void setupUi(QDialog *DrugsWidget__Internal__DrugInfo)
    {
        if (DrugsWidget__Internal__DrugInfo->objectName().isEmpty())
            DrugsWidget__Internal__DrugInfo->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DrugInfo"));
        DrugsWidget__Internal__DrugInfo->resize(657, 455);
        gridLayout_2 = new QGridLayout(DrugsWidget__Internal__DrugInfo);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        drugName = new QLabel(DrugsWidget__Internal__DrugInfo);
        drugName->setObjectName(QString::fromUtf8("drugName"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(drugName->sizePolicy().hasHeightForWidth());
        drugName->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        drugName->setFont(font);
        drugName->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(drugName, 0, 0, 1, 1);

        line = new QFrame(DrugsWidget__Internal__DrugInfo);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(line, 1, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DrugsWidget__Internal__DrugInfo);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_2->addWidget(buttonBox, 4, 0, 1, 1);

        chkAllIAM = new QTabWidget(DrugsWidget__Internal__DrugInfo);
        chkAllIAM->setObjectName(QString::fromUtf8("chkAllIAM"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout = new QGridLayout(tab);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label_2->setFont(font1);
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_2);

        knownMols = new QListWidget(tab);
        knownMols->setObjectName(QString::fromUtf8("knownMols"));

        verticalLayout->addWidget(knownMols);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font1);
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_3);

        DCI = new QListWidget(tab);
        DCI->setObjectName(QString::fromUtf8("DCI"));

        verticalLayout_2->addWidget(DCI);


        gridLayout->addLayout(verticalLayout_2, 0, 1, 1, 1);

        groupTestINN = new QGroupBox(tab);
        groupTestINN->setObjectName(QString::fromUtf8("groupTestINN"));
        verticalLayout_4 = new QVBoxLayout(groupTestINN);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        rbINNOk = new QRadioButton(groupTestINN);
        rbINNOk->setObjectName(QString::fromUtf8("rbINNOk"));

        verticalLayout_4->addWidget(rbINNOk);

        rbINNWrong = new QRadioButton(groupTestINN);
        rbINNWrong->setObjectName(QString::fromUtf8("rbINNWrong"));

        verticalLayout_4->addWidget(rbINNWrong);

        label = new QLabel(groupTestINN);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font1);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(label);

        INNMessage = new QTextEdit(groupTestINN);
        INNMessage->setObjectName(QString::fromUtf8("INNMessage"));

        verticalLayout_4->addWidget(INNMessage);

        butSendINN = new QPushButton(groupTestINN);
        butSendINN->setObjectName(QString::fromUtf8("butSendINN"));

        verticalLayout_4->addWidget(butSendINN);


        gridLayout->addWidget(groupTestINN, 0, 2, 2, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);
        label_4->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_4);

        interactClass = new QListWidget(tab);
        interactClass->setObjectName(QString::fromUtf8("interactClass"));

        verticalLayout_3->addWidget(interactClass);


        gridLayout->addLayout(verticalLayout_3, 1, 0, 1, 2);

        chkAllIAM->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_4 = new QGridLayout(tab_2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_5 = new QLabel(tab_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font1);
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_5, 0, 0, 1, 2);

        listWidgetInteractions = new QListWidget(tab_2);
        listWidgetInteractions->setObjectName(QString::fromUtf8("listWidgetInteractions"));

        gridLayout_3->addWidget(listWidgetInteractions, 1, 0, 1, 2);

        Info_textBrowser = new QTextBrowser(tab_2);
        Info_textBrowser->setObjectName(QString::fromUtf8("Info_textBrowser"));

        gridLayout_3->addWidget(Info_textBrowser, 3, 0, 1, 1);

        CAT_textBrower = new QTextBrowser(tab_2);
        CAT_textBrower->setObjectName(QString::fromUtf8("CAT_textBrower"));

        gridLayout_3->addWidget(CAT_textBrower, 3, 1, 1, 1);

        label_6 = new QLabel(tab_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font1);
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_6, 2, 0, 1, 1);

        label_7 = new QLabel(tab_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setFont(font1);
        label_7->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_7, 2, 1, 1, 1);


        gridLayout_4->addLayout(gridLayout_3, 0, 0, 1, 1);

        groupTestIAM = new QGroupBox(tab_2);
        groupTestIAM->setObjectName(QString::fromUtf8("groupTestIAM"));
        verticalLayout_6 = new QVBoxLayout(groupTestIAM);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        chkAllInteractionsFound = new QCheckBox(groupTestIAM);
        chkAllInteractionsFound->setObjectName(QString::fromUtf8("chkAllInteractionsFound"));

        verticalLayout_6->addWidget(chkAllInteractionsFound);

        chkAllInteractionsOK = new QCheckBox(groupTestIAM);
        chkAllInteractionsOK->setObjectName(QString::fromUtf8("chkAllInteractionsOK"));

        verticalLayout_6->addWidget(chkAllInteractionsOK);

        chkAllIAMTextsOK = new QCheckBox(groupTestIAM);
        chkAllIAMTextsOK->setObjectName(QString::fromUtf8("chkAllIAMTextsOK"));

        verticalLayout_6->addWidget(chkAllIAMTextsOK);

        chkAllCATTextsOK = new QCheckBox(groupTestIAM);
        chkAllCATTextsOK->setObjectName(QString::fromUtf8("chkAllCATTextsOK"));

        verticalLayout_6->addWidget(chkAllCATTextsOK);

        label_8 = new QLabel(groupTestIAM);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setFont(font1);
        label_8->setAlignment(Qt::AlignCenter);

        verticalLayout_6->addWidget(label_8);

        InteractMessage = new QTextEdit(groupTestIAM);
        InteractMessage->setObjectName(QString::fromUtf8("InteractMessage"));

        verticalLayout_6->addWidget(InteractMessage);

        butIAMSend = new QPushButton(groupTestIAM);
        butIAMSend->setObjectName(QString::fromUtf8("butIAMSend"));

        verticalLayout_6->addWidget(butIAMSend);


        gridLayout_4->addWidget(groupTestIAM, 0, 1, 1, 1);

        chkAllIAM->addTab(tab_2, QString());

        gridLayout_2->addWidget(chkAllIAM, 3, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__DrugInfo);
        QObject::connect(buttonBox, SIGNAL(accepted()), DrugsWidget__Internal__DrugInfo, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DrugsWidget__Internal__DrugInfo, SLOT(reject()));

        chkAllIAM->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DrugInfo);
    } // setupUi

    void retranslateUi(QDialog *DrugsWidget__Internal__DrugInfo)
    {
        DrugsWidget__Internal__DrugInfo->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Dialog", 0, QApplication::UnicodeUTF8));
        drugName->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "drug name", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Known molecule", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "associated INN", 0, QApplication::UnicodeUTF8));
        groupTestINN->setTitle(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Testers", 0, QApplication::UnicodeUTF8));
        rbINNOk->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "All informations are exact", 0, QApplication::UnicodeUTF8));
        rbINNWrong->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Something is wrong", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Explain what is wrong", 0, QApplication::UnicodeUTF8));
        butSendINN->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Send this informations", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Drug interaction class", 0, QApplication::UnicodeUTF8));
        chkAllIAM->setTabText(chkAllIAM->indexOf(tab), QApplication::translate("DrugsWidget::Internal::DrugInfo", "Drug information", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Interactions from the list", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Nature of the risk", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "What to do", 0, QApplication::UnicodeUTF8));
        groupTestIAM->setTitle(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Testers", 0, QApplication::UnicodeUTF8));
        chkAllInteractionsFound->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "All interactions are found", 0, QApplication::UnicodeUTF8));
        chkAllInteractionsOK->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "All interactions are exact", 0, QApplication::UnicodeUTF8));
        chkAllIAMTextsOK->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "All Interaction text is right", 0, QApplication::UnicodeUTF8));
        chkAllCATTextsOK->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "All todos texts are right", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Explain what is wrong", 0, QApplication::UnicodeUTF8));
        butIAMSend->setText(QApplication::translate("DrugsWidget::Internal::DrugInfo", "Send this informations", 0, QApplication::UnicodeUTF8));
        chkAllIAM->setTabText(chkAllIAM->indexOf(tab_2), QApplication::translate("DrugsWidget::Internal::DrugInfo", "Found drug interactions", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DrugInfo);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DrugInfo: public Ui_DrugInfo {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_DRUGINFO_H

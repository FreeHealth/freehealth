/********************************************************************************
** Form generated from reading ui file 'userviewer.ui'
**
** Created: Mon Feb 1 11:54:10 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_USERVIEWER_H
#define UI_USERVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDateTimeEdit>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>
#include <listviewplugin/stringlistview.h>
#include <usermanagerplugin/widgets/userrightswidget.h>

namespace UserPlugin {
namespace Internal {

class Ui_UserViewer
{
public:
    QGridLayout *gridLayout_4;
    QSplitter *splitter;
    QTabWidget *tabWidget;
    QWidget *tabIdentity;
    QGridLayout *gridLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *loginLineEdit;
    QPushButton *but_viewHistory;
    QPushButton *but_changePassword;
    QLabel *label_7;
    QDateTimeEdit *lastLoginDateTimeEdit;
    QLabel *label_2;
    QLineEdit *uuidLineEdit;
    QLabel *label_4;
    QLineEdit *nameLineEdit;
    QLabel *label_5;
    QLineEdit *secNameLineEdit;
    QLabel *label_6;
    QLineEdit *surnameLineEdit;
    QLabel *label_3;
    QComboBox *titleCombo;
    QLabel *label_8;
    QComboBox *genderCombo;
    QSpacerItem *verticalSpacer;
    QWidget *tabContact;
    QGridLayout *gridLayout_2;
    QFormLayout *formLayout_2;
    QLabel *label_9;
    QHBoxLayout *horizontalLayout;
    QLineEdit *countryLineEdit;
    QLabel *label_21;
    QComboBox *languageCombo;
    QLabel *label_10;
    QTextEdit *adressTextEdit;
    QLabel *label_11;
    QLineEdit *zipcodeLineEdit;
    QLabel *label_12;
    QLineEdit *cityLineEdit;
    QLabel *label_13;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *tel1LineEdit;
    QLabel *label_14;
    QLineEdit *tel2LineEdit;
    QLabel *label_15;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *tel3LineEdit;
    QLabel *label_16;
    QLineEdit *faxLineEdit;
    QLabel *label_17;
    QLineEdit *mailLineEdit;
    QSpacerItem *verticalSpacer_2;
    QWidget *tabDatas;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_20;
    QFrame *line_2;
    QLabel *label_22;
    QLabel *label_23;
    Views::StringListView *specialtyListView;
    Views::StringListView *qualificationsListView;
    QLabel *label_18;
    Views::StringListView *practIdsListView;
    QWidget *rightsTab;
    QGridLayout *gridLayout_10;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_24;
    QFrame *line_3;
    QLabel *label_25;
    QLabel *label_19;
    UserPlugin::Internal::UserRightsWidget *userManagerRightsListWidget;
    UserPlugin::Internal::UserRightsWidget *medicalRightsListWidget;
    QLabel *label_28;
    UserPlugin::Internal::UserRightsWidget *drugsRightsListWidget;
    QLabel *label_26;
    QLabel *label_27;
    UserPlugin::Internal::UserRightsWidget *paramedicalRightsWidget;
    UserPlugin::Internal::UserRightsWidget *administrativeRightsWidget;
    QWidget *tabHeadersFooters_2;
    QGridLayout *gridLayout_9;
    QGridLayout *gridLayout_5;
    QTabWidget *tabHeadersFooters;
    QWidget *genericTab;
    QGridLayout *editorsGenericLayout;
    QWidget *adminTab;
    QGridLayout *editorsAdminLayout;
    QWidget *prescriptionTab;
    QGridLayout *editorsPrescriptionLayout;
    QWidget *tabAlerts;
    QGridLayout *gridLayout_6;
    QLabel *label_29;

    void setupUi(QWidget *UserPlugin__Internal__UserViewer)
    {
        if (UserPlugin__Internal__UserViewer->objectName().isEmpty())
            UserPlugin__Internal__UserViewer->setObjectName(QString::fromUtf8("UserPlugin__Internal__UserViewer"));
        UserPlugin__Internal__UserViewer->resize(651, 587);
        gridLayout_4 = new QGridLayout(UserPlugin__Internal__UserViewer);
        gridLayout_4->setMargin(0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        splitter = new QSplitter(UserPlugin__Internal__UserViewer);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        splitter->setChildrenCollapsible(false);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabIdentity = new QWidget();
        tabIdentity->setObjectName(QString::fromUtf8("tabIdentity"));
        gridLayout = new QGridLayout(tabIdentity);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
        formLayout->setLabelAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);
        label = new QLabel(tabIdentity);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        loginLineEdit = new QLineEdit(tabIdentity);
        loginLineEdit->setObjectName(QString::fromUtf8("loginLineEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, loginLineEdit);

        but_viewHistory = new QPushButton(tabIdentity);
        but_viewHistory->setObjectName(QString::fromUtf8("but_viewHistory"));

        formLayout->setWidget(2, QFormLayout::FieldRole, but_viewHistory);

        but_changePassword = new QPushButton(tabIdentity);
        but_changePassword->setObjectName(QString::fromUtf8("but_changePassword"));

        formLayout->setWidget(3, QFormLayout::FieldRole, but_changePassword);

        label_7 = new QLabel(tabIdentity);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_7);

        lastLoginDateTimeEdit = new QDateTimeEdit(tabIdentity);
        lastLoginDateTimeEdit->setObjectName(QString::fromUtf8("lastLoginDateTimeEdit"));
        lastLoginDateTimeEdit->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lastLoginDateTimeEdit->sizePolicy().hasHeightForWidth());
        lastLoginDateTimeEdit->setSizePolicy(sizePolicy1);
        lastLoginDateTimeEdit->setFrame(true);
        lastLoginDateTimeEdit->setReadOnly(true);
        lastLoginDateTimeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);

        formLayout->setWidget(4, QFormLayout::FieldRole, lastLoginDateTimeEdit);

        label_2 = new QLabel(tabIdentity);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_2);

        uuidLineEdit = new QLineEdit(tabIdentity);
        uuidLineEdit->setObjectName(QString::fromUtf8("uuidLineEdit"));
        uuidLineEdit->setEnabled(false);
        uuidLineEdit->setReadOnly(true);

        formLayout->setWidget(5, QFormLayout::FieldRole, uuidLineEdit);

        label_4 = new QLabel(tabIdentity);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_4);

        nameLineEdit = new QLineEdit(tabIdentity);
        nameLineEdit->setObjectName(QString::fromUtf8("nameLineEdit"));

        formLayout->setWidget(6, QFormLayout::FieldRole, nameLineEdit);

        label_5 = new QLabel(tabIdentity);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_5);

        secNameLineEdit = new QLineEdit(tabIdentity);
        secNameLineEdit->setObjectName(QString::fromUtf8("secNameLineEdit"));

        formLayout->setWidget(7, QFormLayout::FieldRole, secNameLineEdit);

        label_6 = new QLabel(tabIdentity);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_6);

        surnameLineEdit = new QLineEdit(tabIdentity);
        surnameLineEdit->setObjectName(QString::fromUtf8("surnameLineEdit"));

        formLayout->setWidget(8, QFormLayout::FieldRole, surnameLineEdit);

        label_3 = new QLabel(tabIdentity);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_3);

        titleCombo = new QComboBox(tabIdentity);
        titleCombo->setObjectName(QString::fromUtf8("titleCombo"));

        formLayout->setWidget(9, QFormLayout::FieldRole, titleCombo);

        label_8 = new QLabel(tabIdentity);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout->setWidget(10, QFormLayout::LabelRole, label_8);

        genderCombo = new QComboBox(tabIdentity);
        genderCombo->setObjectName(QString::fromUtf8("genderCombo"));

        formLayout->setWidget(10, QFormLayout::FieldRole, genderCombo);


        gridLayout->addLayout(formLayout, 2, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 0, 1, 1);

        tabWidget->addTab(tabIdentity, QString());
        tabContact = new QWidget();
        tabContact->setObjectName(QString::fromUtf8("tabContact"));
        gridLayout_2 = new QGridLayout(tabContact);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label_9 = new QLabel(tabContact);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_9);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        countryLineEdit = new QLineEdit(tabContact);
        countryLineEdit->setObjectName(QString::fromUtf8("countryLineEdit"));

        horizontalLayout->addWidget(countryLineEdit);

        label_21 = new QLabel(tabContact);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        horizontalLayout->addWidget(label_21);

        languageCombo = new QComboBox(tabContact);
        languageCombo->setObjectName(QString::fromUtf8("languageCombo"));

        horizontalLayout->addWidget(languageCombo);


        formLayout_2->setLayout(0, QFormLayout::FieldRole, horizontalLayout);

        label_10 = new QLabel(tabContact);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_10);

        adressTextEdit = new QTextEdit(tabContact);
        adressTextEdit->setObjectName(QString::fromUtf8("adressTextEdit"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, adressTextEdit);

        label_11 = new QLabel(tabContact);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_11);

        zipcodeLineEdit = new QLineEdit(tabContact);
        zipcodeLineEdit->setObjectName(QString::fromUtf8("zipcodeLineEdit"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, zipcodeLineEdit);

        label_12 = new QLabel(tabContact);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_12);

        cityLineEdit = new QLineEdit(tabContact);
        cityLineEdit->setObjectName(QString::fromUtf8("cityLineEdit"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, cityLineEdit);

        label_13 = new QLabel(tabContact);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_13);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        tel1LineEdit = new QLineEdit(tabContact);
        tel1LineEdit->setObjectName(QString::fromUtf8("tel1LineEdit"));

        horizontalLayout_2->addWidget(tel1LineEdit);

        label_14 = new QLabel(tabContact);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_2->addWidget(label_14);

        tel2LineEdit = new QLineEdit(tabContact);
        tel2LineEdit->setObjectName(QString::fromUtf8("tel2LineEdit"));

        horizontalLayout_2->addWidget(tel2LineEdit);


        formLayout_2->setLayout(4, QFormLayout::FieldRole, horizontalLayout_2);

        label_15 = new QLabel(tabContact);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_15);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        tel3LineEdit = new QLineEdit(tabContact);
        tel3LineEdit->setObjectName(QString::fromUtf8("tel3LineEdit"));

        horizontalLayout_3->addWidget(tel3LineEdit);

        label_16 = new QLabel(tabContact);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        horizontalLayout_3->addWidget(label_16);

        faxLineEdit = new QLineEdit(tabContact);
        faxLineEdit->setObjectName(QString::fromUtf8("faxLineEdit"));

        horizontalLayout_3->addWidget(faxLineEdit);


        formLayout_2->setLayout(5, QFormLayout::FieldRole, horizontalLayout_3);

        label_17 = new QLabel(tabContact);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        formLayout_2->setWidget(6, QFormLayout::LabelRole, label_17);

        mailLineEdit = new QLineEdit(tabContact);
        mailLineEdit->setObjectName(QString::fromUtf8("mailLineEdit"));

        formLayout_2->setWidget(6, QFormLayout::FieldRole, mailLineEdit);


        gridLayout_2->addLayout(formLayout_2, 1, 0, 2, 2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 3, 0, 1, 2);

        tabWidget->addTab(tabContact, QString());
        tabDatas = new QWidget();
        tabDatas->setObjectName(QString::fromUtf8("tabDatas"));
        gridLayout_7 = new QGridLayout(tabDatas);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_20 = new QLabel(tabDatas);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_20->sizePolicy().hasHeightForWidth());
        label_20->setSizePolicy(sizePolicy2);

        horizontalLayout_5->addWidget(label_20);

        line_2 = new QFrame(tabDatas);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        horizontalLayout_5->addWidget(line_2);


        gridLayout_3->addLayout(horizontalLayout_5, 0, 0, 1, 2);

        label_22 = new QLabel(tabDatas);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_22, 1, 0, 1, 1);

        label_23 = new QLabel(tabDatas);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_23, 1, 1, 1, 1);

        specialtyListView = new Views::StringListView(tabDatas);
        specialtyListView->setObjectName(QString::fromUtf8("specialtyListView"));

        gridLayout_3->addWidget(specialtyListView, 2, 0, 1, 1);

        qualificationsListView = new Views::StringListView(tabDatas);
        qualificationsListView->setObjectName(QString::fromUtf8("qualificationsListView"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(qualificationsListView->sizePolicy().hasHeightForWidth());
        qualificationsListView->setSizePolicy(sizePolicy3);

        gridLayout_3->addWidget(qualificationsListView, 2, 1, 3, 1);

        label_18 = new QLabel(tabDatas);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_18, 3, 0, 1, 1);

        practIdsListView = new Views::StringListView(tabDatas);
        practIdsListView->setObjectName(QString::fromUtf8("practIdsListView"));
        sizePolicy3.setHeightForWidth(practIdsListView->sizePolicy().hasHeightForWidth());
        practIdsListView->setSizePolicy(sizePolicy3);

        gridLayout_3->addWidget(practIdsListView, 4, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_3, 0, 0, 1, 1);

        tabWidget->addTab(tabDatas, QString());
        rightsTab = new QWidget();
        rightsTab->setObjectName(QString::fromUtf8("rightsTab"));
        gridLayout_10 = new QGridLayout(rightsTab);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_24 = new QLabel(rightsTab);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        sizePolicy2.setHeightForWidth(label_24->sizePolicy().hasHeightForWidth());
        label_24->setSizePolicy(sizePolicy2);

        horizontalLayout_6->addWidget(label_24);

        line_3 = new QFrame(rightsTab);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        horizontalLayout_6->addWidget(line_3);


        gridLayout_10->addLayout(horizontalLayout_6, 0, 0, 1, 2);

        label_25 = new QLabel(rightsTab);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_25->setFont(font);
        label_25->setAlignment(Qt::AlignCenter);

        gridLayout_10->addWidget(label_25, 1, 0, 1, 1);

        label_19 = new QLabel(rightsTab);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setFont(font);
        label_19->setAlignment(Qt::AlignCenter);

        gridLayout_10->addWidget(label_19, 1, 1, 1, 1);

        userManagerRightsListWidget = new UserPlugin::Internal::UserRightsWidget(rightsTab);
        userManagerRightsListWidget->setObjectName(QString::fromUtf8("userManagerRightsListWidget"));

        gridLayout_10->addWidget(userManagerRightsListWidget, 2, 0, 1, 1);

        medicalRightsListWidget = new UserPlugin::Internal::UserRightsWidget(rightsTab);
        medicalRightsListWidget->setObjectName(QString::fromUtf8("medicalRightsListWidget"));

        gridLayout_10->addWidget(medicalRightsListWidget, 2, 1, 1, 1);

        label_28 = new QLabel(rightsTab);
        label_28->setObjectName(QString::fromUtf8("label_28"));
        label_28->setFont(font);
        label_28->setAlignment(Qt::AlignCenter);

        gridLayout_10->addWidget(label_28, 3, 0, 1, 1);

        drugsRightsListWidget = new UserPlugin::Internal::UserRightsWidget(rightsTab);
        drugsRightsListWidget->setObjectName(QString::fromUtf8("drugsRightsListWidget"));

        gridLayout_10->addWidget(drugsRightsListWidget, 4, 0, 1, 1);

        label_26 = new QLabel(rightsTab);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        label_26->setFont(font);
        label_26->setAlignment(Qt::AlignCenter);

        gridLayout_10->addWidget(label_26, 5, 0, 1, 1);

        label_27 = new QLabel(rightsTab);
        label_27->setObjectName(QString::fromUtf8("label_27"));
        label_27->setFont(font);
        label_27->setAlignment(Qt::AlignCenter);

        gridLayout_10->addWidget(label_27, 5, 1, 1, 1);

        paramedicalRightsWidget = new UserPlugin::Internal::UserRightsWidget(rightsTab);
        paramedicalRightsWidget->setObjectName(QString::fromUtf8("paramedicalRightsWidget"));

        gridLayout_10->addWidget(paramedicalRightsWidget, 6, 0, 1, 1);

        administrativeRightsWidget = new UserPlugin::Internal::UserRightsWidget(rightsTab);
        administrativeRightsWidget->setObjectName(QString::fromUtf8("administrativeRightsWidget"));

        gridLayout_10->addWidget(administrativeRightsWidget, 6, 1, 1, 1);

        tabWidget->addTab(rightsTab, QString());
        tabHeadersFooters_2 = new QWidget();
        tabHeadersFooters_2->setObjectName(QString::fromUtf8("tabHeadersFooters_2"));
        gridLayout_9 = new QGridLayout(tabHeadersFooters_2);
        gridLayout_9->setMargin(0);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        tabHeadersFooters = new QTabWidget(tabHeadersFooters_2);
        tabHeadersFooters->setObjectName(QString::fromUtf8("tabHeadersFooters"));
        genericTab = new QWidget();
        genericTab->setObjectName(QString::fromUtf8("genericTab"));
        editorsGenericLayout = new QGridLayout(genericTab);
        editorsGenericLayout->setMargin(0);
        editorsGenericLayout->setObjectName(QString::fromUtf8("editorsGenericLayout"));
        tabHeadersFooters->addTab(genericTab, QString());
        adminTab = new QWidget();
        adminTab->setObjectName(QString::fromUtf8("adminTab"));
        editorsAdminLayout = new QGridLayout(adminTab);
        editorsAdminLayout->setMargin(0);
        editorsAdminLayout->setObjectName(QString::fromUtf8("editorsAdminLayout"));
        tabHeadersFooters->addTab(adminTab, QString());
        prescriptionTab = new QWidget();
        prescriptionTab->setObjectName(QString::fromUtf8("prescriptionTab"));
        editorsPrescriptionLayout = new QGridLayout(prescriptionTab);
        editorsPrescriptionLayout->setMargin(0);
        editorsPrescriptionLayout->setObjectName(QString::fromUtf8("editorsPrescriptionLayout"));
        tabHeadersFooters->addTab(prescriptionTab, QString());

        gridLayout_5->addWidget(tabHeadersFooters, 0, 0, 1, 1);


        gridLayout_9->addLayout(gridLayout_5, 0, 0, 1, 1);

        tabWidget->addTab(tabHeadersFooters_2, QString());
        tabAlerts = new QWidget();
        tabAlerts->setObjectName(QString::fromUtf8("tabAlerts"));
        gridLayout_6 = new QGridLayout(tabAlerts);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        label_29 = new QLabel(tabAlerts);
        label_29->setObjectName(QString::fromUtf8("label_29"));
        sizePolicy1.setHeightForWidth(label_29->sizePolicy().hasHeightForWidth());
        label_29->setSizePolicy(sizePolicy1);
        label_29->setFont(font);
        label_29->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(label_29, 0, 0, 1, 1);

        tabWidget->addTab(tabAlerts, QString());
        splitter->addWidget(tabWidget);

        gridLayout_4->addWidget(splitter, 0, 0, 1, 1);


        retranslateUi(UserPlugin__Internal__UserViewer);

        tabWidget->setCurrentIndex(5);
        tabHeadersFooters->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(UserPlugin__Internal__UserViewer);
    } // setupUi

    void retranslateUi(QWidget *UserPlugin__Internal__UserViewer)
    {
        UserPlugin__Internal__UserViewer->setWindowTitle(QApplication::translate("UserPlugin::Internal::UserViewer", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Login", 0, QApplication::UnicodeUTF8));
        but_viewHistory->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "View login history", 0, QApplication::UnicodeUTF8));
        but_changePassword->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Change password", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Last Login", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lastLoginDateTimeEdit->setToolTip(QApplication::translate("UserPlugin::Internal::UserViewer", "Date and time of the last connection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Unique Identifant", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Name", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Second Name", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Surname", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Title", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Gender", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabIdentity), QApplication::translate("UserPlugin::Internal::UserViewer", "Identity", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Country", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Language", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Address", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "ZipCode", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "City", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Tel 1", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Tel 2", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Tel 3", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Fax", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Mail", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabContact), QApplication::translate("UserPlugin::Internal::UserViewer", "Contact", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Specialties and qualifications", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Specialty", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Qualifications", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Practitioner identifiant", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabDatas), QApplication::translate("UserPlugin::Internal::UserViewer", "Datas", 0, QApplication::UnicodeUTF8));
        label_24->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Rights", 0, QApplication::UnicodeUTF8));
        label_25->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "User manager rights", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Medical rights", 0, QApplication::UnicodeUTF8));
        label_28->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Drugs dosage rights", 0, QApplication::UnicodeUTF8));
        label_26->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Paramedical rights", 0, QApplication::UnicodeUTF8));
        label_27->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Administrative rights", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(rightsTab), QApplication::translate("UserPlugin::Internal::UserViewer", "Rights", 0, QApplication::UnicodeUTF8));
        tabHeadersFooters->setTabText(tabHeadersFooters->indexOf(genericTab), QApplication::translate("UserPlugin::Internal::UserViewer", "Generic", 0, QApplication::UnicodeUTF8));
        tabHeadersFooters->setTabText(tabHeadersFooters->indexOf(adminTab), QApplication::translate("UserPlugin::Internal::UserViewer", "Administrative", 0, QApplication::UnicodeUTF8));
        tabHeadersFooters->setTabText(tabHeadersFooters->indexOf(prescriptionTab), QApplication::translate("UserPlugin::Internal::UserViewer", "Prescriptions", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabHeadersFooters_2), QApplication::translate("UserPlugin::Internal::UserViewer", "Headers and Footers", 0, QApplication::UnicodeUTF8));
        label_29->setText(QApplication::translate("UserPlugin::Internal::UserViewer", "Only super-administrator can leave alerts to users.", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabAlerts), QApplication::translate("UserPlugin::Internal::UserViewer", "Alerts", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(UserPlugin__Internal__UserViewer);
    } // retranslateUi

};

} // namespace Internal
} // namespace UserPlugin

namespace UserPlugin {
namespace Internal {
namespace Ui {
    class UserViewer: public Ui_UserViewer {};
} // namespace Ui
} // namespace Internal
} // namespace UserPlugin

#endif // UI_USERVIEWER_H

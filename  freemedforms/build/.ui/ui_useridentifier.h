/********************************************************************************
** Form generated from reading ui file 'useridentifier.ui'
**
** Created: Mon Feb 1 11:54:10 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_USERIDENTIFIER_H
#define UI_USERIDENTIFIER_H

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
#include <utils/widgets/lineeditechoswitcher.h>

namespace UserPlugin {
namespace Internal {

class Ui_UserIdentifier
{
public:
    QGridLayout *gridLayout_3;
    QLabel *lblAppName;
    QLabel *lblAppName_2;
    QFrame *line;
    QGroupBox *groupInformations;
    QGridLayout *gridLayout_2;
    QListWidget *informationsWidget;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QCheckBox *chkRemember;
    Utils::LineEditEchoSwitcher *login;
    Utils::LineEditEchoSwitcher *password;
    QDialogButtonBox *buttonBox;
    QLabel *newlyMessage;

    void setupUi(QDialog *UserPlugin__Internal__UserIdentifier)
    {
        if (UserPlugin__Internal__UserIdentifier->objectName().isEmpty())
            UserPlugin__Internal__UserIdentifier->setObjectName(QString::fromUtf8("UserPlugin__Internal__UserIdentifier"));
        UserPlugin__Internal__UserIdentifier->resize(469, 289);
        gridLayout_3 = new QGridLayout(UserPlugin__Internal__UserIdentifier);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        lblAppName = new QLabel(UserPlugin__Internal__UserIdentifier);
        lblAppName->setObjectName(QString::fromUtf8("lblAppName"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        lblAppName->setFont(font);
        lblAppName->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(lblAppName, 0, 0, 1, 2);

        lblAppName_2 = new QLabel(UserPlugin__Internal__UserIdentifier);
        lblAppName_2->setObjectName(QString::fromUtf8("lblAppName_2"));
        lblAppName_2->setFont(font);
        lblAppName_2->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(lblAppName_2, 1, 0, 1, 2);

        line = new QFrame(UserPlugin__Internal__UserIdentifier);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(line, 3, 0, 1, 2);

        groupInformations = new QGroupBox(UserPlugin__Internal__UserIdentifier);
        groupInformations->setObjectName(QString::fromUtf8("groupInformations"));
        gridLayout_2 = new QGridLayout(groupInformations);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        informationsWidget = new QListWidget(groupInformations);
        informationsWidget->setObjectName(QString::fromUtf8("informationsWidget"));

        gridLayout_2->addWidget(informationsWidget, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupInformations, 5, 0, 1, 1);

        groupBox = new QGroupBox(UserPlugin__Internal__UserIdentifier);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        chkRemember = new QCheckBox(groupBox);
        chkRemember->setObjectName(QString::fromUtf8("chkRemember"));
        chkRemember->setEnabled(false);
        chkRemember->setLayoutDirection(Qt::RightToLeft);
        chkRemember->setAutoFillBackground(false);
        chkRemember->setTristate(false);

        gridLayout->addWidget(chkRemember, 2, 0, 1, 2);

        login = new Utils::LineEditEchoSwitcher(groupBox);
        login->setObjectName(QString::fromUtf8("login"));

        gridLayout->addWidget(login, 0, 1, 1, 1);

        password = new Utils::LineEditEchoSwitcher(groupBox);
        password->setObjectName(QString::fromUtf8("password"));

        gridLayout->addWidget(password, 1, 1, 1, 1);


        gridLayout_3->addWidget(groupBox, 5, 1, 1, 1);

        buttonBox = new QDialogButtonBox(UserPlugin__Internal__UserIdentifier);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_3->addWidget(buttonBox, 6, 0, 1, 2);

        newlyMessage = new QLabel(UserPlugin__Internal__UserIdentifier);
        newlyMessage->setObjectName(QString::fromUtf8("newlyMessage"));
        newlyMessage->setFont(font);
        newlyMessage->setAlignment(Qt::AlignCenter);
        newlyMessage->setWordWrap(true);

        gridLayout_3->addWidget(newlyMessage, 4, 0, 1, 2);

        QWidget::setTabOrder(chkRemember, buttonBox);
        QWidget::setTabOrder(buttonBox, informationsWidget);

        retranslateUi(UserPlugin__Internal__UserIdentifier);
        QObject::connect(buttonBox, SIGNAL(accepted()), UserPlugin__Internal__UserIdentifier, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UserPlugin__Internal__UserIdentifier, SLOT(reject()));

        QMetaObject::connectSlotsByName(UserPlugin__Internal__UserIdentifier);
    } // setupUi

    void retranslateUi(QDialog *UserPlugin__Internal__UserIdentifier)
    {
        UserPlugin__Internal__UserIdentifier->setWindowTitle(QApplication::translate("UserPlugin::Internal::UserIdentifier", "Dialog", 0, QApplication::UnicodeUTF8));
        lblAppName->setText(QApplication::translate("UserPlugin::Internal::UserIdentifier", "appName", 0, QApplication::UnicodeUTF8));
        lblAppName_2->setText(QApplication::translate("UserPlugin::Internal::UserIdentifier", "User identification", 0, QApplication::UnicodeUTF8));
        groupInformations->setTitle(QApplication::translate("UserPlugin::Internal::UserIdentifier", "Informations", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("UserPlugin::Internal::UserIdentifier", "Identifiers", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("UserPlugin::Internal::UserIdentifier", "Login", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("UserPlugin::Internal::UserIdentifier", "Password", 0, QApplication::UnicodeUTF8));
        chkRemember->setText(QApplication::translate("UserPlugin::Internal::UserIdentifier", "Remember me for a while", 0, QApplication::UnicodeUTF8));
        newlyMessage->setText(QApplication::translate("UserPlugin::Internal::UserIdentifier", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Lucida Grande'; font-size:13pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">The users' database has been created.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Use <span style=\" font-style:italic;\">admin</span> for the login and password.</p></body></html>", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(UserPlugin__Internal__UserIdentifier);
    } // retranslateUi

};

} // namespace Internal
} // namespace UserPlugin

namespace UserPlugin {
namespace Internal {
namespace Ui {
    class UserIdentifier: public Ui_UserIdentifier {};
} // namespace Ui
} // namespace Internal
} // namespace UserPlugin

#endif // UI_USERIDENTIFIER_H

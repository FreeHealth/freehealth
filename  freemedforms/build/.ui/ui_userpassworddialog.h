/********************************************************************************
** Form generated from reading ui file 'userpassworddialog.ui'
**
** Created: Mon Feb 1 11:54:10 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_USERPASSWORDDIALOG_H
#define UI_USERPASSWORDDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <utils/widgets/lineeditechoswitcher.h>

namespace UserPlugin {
namespace Internal {

class Ui_UserPasswordDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QFrame *line;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label_3;
    QDialogButtonBox *buttonBox;
    Utils::LineEditEchoSwitcher *oldPass;
    Utils::LineEditEchoSwitcher *newPass;
    Utils::LineEditEchoSwitcher *newControl;

    void setupUi(QDialog *UserPlugin__Internal__UserPasswordDialog)
    {
        if (UserPlugin__Internal__UserPasswordDialog->objectName().isEmpty())
            UserPlugin__Internal__UserPasswordDialog->setObjectName(QString::fromUtf8("UserPlugin__Internal__UserPasswordDialog"));
        UserPlugin__Internal__UserPasswordDialog->resize(519, 190);
        gridLayout = new QGridLayout(UserPlugin__Internal__UserPasswordDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(UserPlugin__Internal__UserPasswordDialog);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 3);

        line = new QFrame(UserPlugin__Internal__UserPasswordDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 3);

        label_4 = new QLabel(UserPlugin__Internal__UserPasswordDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 2, 0, 1, 1);

        label_2 = new QLabel(UserPlugin__Internal__UserPasswordDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        label_3 = new QLabel(UserPlugin__Internal__UserPasswordDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        buttonBox = new QDialogButtonBox(UserPlugin__Internal__UserPasswordDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 5, 0, 1, 3);

        oldPass = new Utils::LineEditEchoSwitcher(UserPlugin__Internal__UserPasswordDialog);
        oldPass->setObjectName(QString::fromUtf8("oldPass"));

        gridLayout->addWidget(oldPass, 2, 1, 1, 2);

        newPass = new Utils::LineEditEchoSwitcher(UserPlugin__Internal__UserPasswordDialog);
        newPass->setObjectName(QString::fromUtf8("newPass"));

        gridLayout->addWidget(newPass, 3, 1, 1, 2);

        newControl = new Utils::LineEditEchoSwitcher(UserPlugin__Internal__UserPasswordDialog);
        newControl->setObjectName(QString::fromUtf8("newControl"));

        gridLayout->addWidget(newControl, 4, 1, 1, 2);


        retranslateUi(UserPlugin__Internal__UserPasswordDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), UserPlugin__Internal__UserPasswordDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UserPlugin__Internal__UserPasswordDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(UserPlugin__Internal__UserPasswordDialog);
    } // setupUi

    void retranslateUi(QDialog *UserPlugin__Internal__UserPasswordDialog)
    {
        UserPlugin__Internal__UserPasswordDialog->setWindowTitle(QApplication::translate("UserPlugin::Internal::UserPasswordDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("UserPlugin::Internal::UserPasswordDialog", "Change your password", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("UserPlugin::Internal::UserPasswordDialog", "Type old password", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("UserPlugin::Internal::UserPasswordDialog", "Type new password", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("UserPlugin::Internal::UserPasswordDialog", "Confirm new password", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(UserPlugin__Internal__UserPasswordDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace UserPlugin

namespace UserPlugin {
namespace Internal {
namespace Ui {
    class UserPasswordDialog: public Ui_UserPasswordDialog {};
} // namespace Ui
} // namespace Internal
} // namespace UserPlugin

#endif // UI_USERPASSWORDDIALOG_H

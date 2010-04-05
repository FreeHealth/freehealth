/********************************************************************************
** Form generated from reading ui file 'debugdialog.ui'
**
** Created: Sun Sep 13 10:28:07 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DEBUGDIALOG_H
#define UI_DEBUGDIALOG_H

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
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

namespace Core {
namespace Internal {

class Ui_DebugDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QPushButton *fullScreenButton;
    QFrame *line;
    QHBoxLayout *horizontalLayout_3;
    QTreeWidget *tree;
    QWidget *forStack;
    QHBoxLayout *horizontalLayout;
    QPushButton *butSend;
    QPushButton *butSave;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_2;
    QPushButton *pushButton;

    void setupUi(QDialog *Core__Internal__DebugDialog)
    {
        if (Core__Internal__DebugDialog->objectName().isEmpty())
            Core__Internal__DebugDialog->setObjectName(QString::fromUtf8("Core__Internal__DebugDialog"));
        Core__Internal__DebugDialog->resize(457, 404);
        gridLayout = new QGridLayout(Core__Internal__DebugDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(Core__Internal__DebugDialog);
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

        horizontalLayout_2->addWidget(label);

        fullScreenButton = new QPushButton(Core__Internal__DebugDialog);
        fullScreenButton->setObjectName(QString::fromUtf8("fullScreenButton"));
        fullScreenButton->setMinimumSize(QSize(20, 20));
        fullScreenButton->setMaximumSize(QSize(20, 20));

        horizontalLayout_2->addWidget(fullScreenButton);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        line = new QFrame(Core__Internal__DebugDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        tree = new QTreeWidget(Core__Internal__DebugDialog);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        tree->setHeaderItem(__qtreewidgetitem);
        tree->setObjectName(QString::fromUtf8("tree"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tree->sizePolicy().hasHeightForWidth());
        tree->setSizePolicy(sizePolicy1);
        tree->header()->setVisible(false);

        horizontalLayout_3->addWidget(tree);

        forStack = new QWidget(Core__Internal__DebugDialog);
        forStack->setObjectName(QString::fromUtf8("forStack"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(forStack->sizePolicy().hasHeightForWidth());
        forStack->setSizePolicy(sizePolicy2);
        forStack->setMinimumSize(QSize(100, 100));

        horizontalLayout_3->addWidget(forStack);


        gridLayout->addLayout(horizontalLayout_3, 2, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        butSend = new QPushButton(Core__Internal__DebugDialog);
        butSend->setObjectName(QString::fromUtf8("butSend"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(butSend->sizePolicy().hasHeightForWidth());
        butSend->setSizePolicy(sizePolicy3);

        horizontalLayout->addWidget(butSend);

        butSave = new QPushButton(Core__Internal__DebugDialog);
        butSave->setObjectName(QString::fromUtf8("butSave"));

        horizontalLayout->addWidget(butSave);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_2 = new QPushButton(Core__Internal__DebugDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        sizePolicy3.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy3);

        horizontalLayout->addWidget(pushButton_2);

        pushButton = new QPushButton(Core__Internal__DebugDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        sizePolicy3.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy3);

        horizontalLayout->addWidget(pushButton);


        gridLayout->addLayout(horizontalLayout, 3, 0, 1, 1);


        retranslateUi(Core__Internal__DebugDialog);
        QObject::connect(pushButton, SIGNAL(clicked()), Core__Internal__DebugDialog, SLOT(accept()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), Core__Internal__DebugDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Core__Internal__DebugDialog);
    } // setupUi

    void retranslateUi(QDialog *Core__Internal__DebugDialog)
    {
        Core__Internal__DebugDialog->setWindowTitle(QApplication::translate("Core::Internal::DebugDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Core::Internal::DebugDialog", "Show debugging informations", 0, QApplication::UnicodeUTF8));
        fullScreenButton->setText(QString());
        butSend->setText(QApplication::translate("Core::Internal::DebugDialog", "Send to dev team", 0, QApplication::UnicodeUTF8));
        butSave->setText(QApplication::translate("Core::Internal::DebugDialog", "Save log", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("Core::Internal::DebugDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Core::Internal::DebugDialog", "Ok", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Core__Internal__DebugDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace Core

namespace Core {
namespace Internal {
namespace Ui {
    class DebugDialog: public Ui_DebugDialog {};
} // namespace Ui
} // namespace Internal
} // namespace Core

#endif // UI_DEBUGDIALOG_H

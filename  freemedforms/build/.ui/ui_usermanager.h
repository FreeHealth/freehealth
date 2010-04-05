/********************************************************************************
** Form generated from reading ui file 'usermanager.ui'
**
** Created: Mon Feb 1 11:54:10 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_USERMANAGER_H
#define UI_USERMANAGER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTableView>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <usermanagerplugin/widgets/userviewer.h>
#include <utils/widgets/QButtonLineEdit.h>

namespace UserPlugin {
namespace Internal {

class Ui_UserManager
{
public:
    QAction *createNewUserAct;
    QAction *modifyUserAct;
    QAction *saveAct;
    QAction *clearModificationsAct;
    QAction *deleteUserAct;
    QAction *quitUserManagerAct;
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    Utils::QButtonLineEdit *searchLineEdit;
    QTableView *userTableView;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *memoryUsageLabel;
    UserPlugin::UserViewer *userViewer;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *UserPlugin__Internal__UserManager)
    {
        if (UserPlugin__Internal__UserManager->objectName().isEmpty())
            UserPlugin__Internal__UserManager->setObjectName(QString::fromUtf8("UserPlugin__Internal__UserManager"));
        UserPlugin__Internal__UserManager->resize(800, 600);
        createNewUserAct = new QAction(UserPlugin__Internal__UserManager);
        createNewUserAct->setObjectName(QString::fromUtf8("createNewUserAct"));
        modifyUserAct = new QAction(UserPlugin__Internal__UserManager);
        modifyUserAct->setObjectName(QString::fromUtf8("modifyUserAct"));
        saveAct = new QAction(UserPlugin__Internal__UserManager);
        saveAct->setObjectName(QString::fromUtf8("saveAct"));
        clearModificationsAct = new QAction(UserPlugin__Internal__UserManager);
        clearModificationsAct->setObjectName(QString::fromUtf8("clearModificationsAct"));
        deleteUserAct = new QAction(UserPlugin__Internal__UserManager);
        deleteUserAct->setObjectName(QString::fromUtf8("deleteUserAct"));
        quitUserManagerAct = new QAction(UserPlugin__Internal__UserManager);
        quitUserManagerAct->setObjectName(QString::fromUtf8("quitUserManagerAct"));
        centralwidget = new QWidget(UserPlugin__Internal__UserManager);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        searchLineEdit = new Utils::QButtonLineEdit(layoutWidget);
        searchLineEdit->setObjectName(QString::fromUtf8("searchLineEdit"));

        verticalLayout->addWidget(searchLineEdit);

        userTableView = new QTableView(layoutWidget);
        userTableView->setObjectName(QString::fromUtf8("userTableView"));

        verticalLayout->addWidget(userTableView);

        groupBox = new QGroupBox(layoutWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        memoryUsageLabel = new QLabel(groupBox);
        memoryUsageLabel->setObjectName(QString::fromUtf8("memoryUsageLabel"));

        gridLayout->addWidget(memoryUsageLabel, 0, 0, 1, 1);


        verticalLayout->addWidget(groupBox);

        splitter->addWidget(layoutWidget);
        userViewer = new UserPlugin::UserViewer(splitter);
        userViewer->setObjectName(QString::fromUtf8("userViewer"));
        splitter->addWidget(userViewer);

        gridLayout_2->addWidget(splitter, 0, 0, 1, 1);

        UserPlugin__Internal__UserManager->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(UserPlugin__Internal__UserManager);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        UserPlugin__Internal__UserManager->setStatusBar(statusbar);
        toolBar = new QToolBar(UserPlugin__Internal__UserManager);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMovable(false);
        toolBar->setAllowedAreas(Qt::LeftToolBarArea);
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        UserPlugin__Internal__UserManager->addToolBar(Qt::TopToolBarArea, toolBar);

        retranslateUi(UserPlugin__Internal__UserManager);

        QMetaObject::connectSlotsByName(UserPlugin__Internal__UserManager);
    } // setupUi

    void retranslateUi(QMainWindow *UserPlugin__Internal__UserManager)
    {
        UserPlugin__Internal__UserManager->setWindowTitle(QApplication::translate("UserPlugin::Internal::UserManager", "MainWindow", 0, QApplication::UnicodeUTF8));
        createNewUserAct->setText(QApplication::translate("UserPlugin::Internal::UserManager", "Create new user", 0, QApplication::UnicodeUTF8));
        modifyUserAct->setText(QApplication::translate("UserPlugin::Internal::UserManager", "Modify User", 0, QApplication::UnicodeUTF8));
        saveAct->setText(QApplication::translate("UserPlugin::Internal::UserManager", "Save changes", 0, QApplication::UnicodeUTF8));
        clearModificationsAct->setText(QApplication::translate("UserPlugin::Internal::UserManager", "Clear modifications", 0, QApplication::UnicodeUTF8));
        deleteUserAct->setText(QApplication::translate("UserPlugin::Internal::UserManager", "Delete User", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        deleteUserAct->setToolTip(QApplication::translate("UserPlugin::Internal::UserManager", "Delete User", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        quitUserManagerAct->setText(QApplication::translate("UserPlugin::Internal::UserManager", "Quit User Manager", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        quitUserManagerAct->setToolTip(QApplication::translate("UserPlugin::Internal::UserManager", "Quit User Manager", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox->setTitle(QApplication::translate("UserPlugin::Internal::UserManager", "Memory Usage", 0, QApplication::UnicodeUTF8));
        memoryUsageLabel->setText(QApplication::translate("UserPlugin::Internal::UserManager", "TextLabel", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("UserPlugin::Internal::UserManager", "User Manager Tool Bar", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(UserPlugin__Internal__UserManager);
    } // retranslateUi

};

} // namespace Internal
} // namespace UserPlugin

namespace UserPlugin {
namespace Internal {
namespace Ui {
    class UserManager: public Ui_UserManager {};
} // namespace Ui
} // namespace Internal
} // namespace UserPlugin

#endif // UI_USERMANAGER_H

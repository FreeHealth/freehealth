/********************************************************************************
** Form generated from reading ui file 'mainwindowpreferenceswidget.ui'
**
** Created: Mon Feb 1 11:55:07 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOWPREFERENCESWIDGET_H
#define UI_MAINWINDOWPREFERENCESWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

namespace MainWin {
namespace Internal {

class Ui_MainWindowPreferencesWidget
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *updateCheckingCombo;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *MainWin__Internal__MainWindowPreferencesWidget)
    {
        if (MainWin__Internal__MainWindowPreferencesWidget->objectName().isEmpty())
            MainWin__Internal__MainWindowPreferencesWidget->setObjectName(QString::fromUtf8("MainWin__Internal__MainWindowPreferencesWidget"));
        MainWin__Internal__MainWindowPreferencesWidget->resize(400, 300);
        gridLayout = new QGridLayout(MainWin__Internal__MainWindowPreferencesWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(MainWin__Internal__MainWindowPreferencesWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        updateCheckingCombo = new QComboBox(MainWin__Internal__MainWindowPreferencesWidget);
        updateCheckingCombo->setObjectName(QString::fromUtf8("updateCheckingCombo"));

        gridLayout->addWidget(updateCheckingCombo, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 0, 1, 1);


        retranslateUi(MainWin__Internal__MainWindowPreferencesWidget);

        QMetaObject::connectSlotsByName(MainWin__Internal__MainWindowPreferencesWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWin__Internal__MainWindowPreferencesWidget)
    {
        MainWin__Internal__MainWindowPreferencesWidget->setWindowTitle(QApplication::translate("MainWin::Internal::MainWindowPreferencesWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWin::Internal::MainWindowPreferencesWidget", "Update checking", 0, QApplication::UnicodeUTF8));
        updateCheckingCombo->clear();
        updateCheckingCombo->insertItems(0, QStringList()
         << QApplication::translate("MainWin::Internal::MainWindowPreferencesWidget", "At startup", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWin::Internal::MainWindowPreferencesWidget", "Each weeks", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWin::Internal::MainWindowPreferencesWidget", "Each months", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWin::Internal::MainWindowPreferencesWidget", "Each quarters", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWin::Internal::MainWindowPreferencesWidget", "Never (on user's demand)", 0, QApplication::UnicodeUTF8)
        );
        Q_UNUSED(MainWin__Internal__MainWindowPreferencesWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace MainWin

namespace MainWin {
namespace Internal {
namespace Ui {
    class MainWindowPreferencesWidget: public Ui_MainWindowPreferencesWidget {};
} // namespace Ui
} // namespace Internal
} // namespace MainWin

#endif // UI_MAINWINDOWPREFERENCESWIDGET_H

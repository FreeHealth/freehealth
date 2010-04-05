/********************************************************************************
** Form generated from reading ui file 'pluginerrorview.ui'
**
** Created: Sun Sep 13 10:26:56 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PLUGINERRORVIEW_H
#define UI_PLUGINERRORVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace ExtensionSystem {
namespace Internal {

class Ui_PluginErrorView
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *state;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QSpacerItem *verticalSpacer;
    QTextEdit *errorString;

    void setupUi(QWidget *ExtensionSystem__Internal__PluginErrorView)
    {
        if (ExtensionSystem__Internal__PluginErrorView->objectName().isEmpty())
            ExtensionSystem__Internal__PluginErrorView->setObjectName(QString::fromUtf8("ExtensionSystem__Internal__PluginErrorView"));
        ExtensionSystem__Internal__PluginErrorView->resize(579, 342);
        gridLayout = new QGridLayout(ExtensionSystem__Internal__PluginErrorView);
        gridLayout->setMargin(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(ExtensionSystem__Internal__PluginErrorView);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        state = new QLabel(ExtensionSystem__Internal__PluginErrorView);
        state->setObjectName(QString::fromUtf8("state"));

        gridLayout->addWidget(state, 0, 1, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_2 = new QLabel(ExtensionSystem__Internal__PluginErrorView);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        verticalLayout->addWidget(label_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        gridLayout->addLayout(verticalLayout, 1, 0, 1, 1);

        errorString = new QTextEdit(ExtensionSystem__Internal__PluginErrorView);
        errorString->setObjectName(QString::fromUtf8("errorString"));
        errorString->setTabChangesFocus(true);
        errorString->setReadOnly(true);

        gridLayout->addWidget(errorString, 1, 1, 1, 1);


        retranslateUi(ExtensionSystem__Internal__PluginErrorView);

        QMetaObject::connectSlotsByName(ExtensionSystem__Internal__PluginErrorView);
    } // setupUi

    void retranslateUi(QWidget *ExtensionSystem__Internal__PluginErrorView)
    {
        label->setText(QApplication::translate("ExtensionSystem::Internal::PluginErrorView", "State:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ExtensionSystem::Internal::PluginErrorView", "Error Message:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ExtensionSystem__Internal__PluginErrorView);
    } // retranslateUi

};

} // namespace Internal
} // namespace ExtensionSystem

namespace ExtensionSystem {
namespace Internal {
namespace Ui {
    class PluginErrorView: public Ui_PluginErrorView {};
} // namespace Ui
} // namespace Internal
} // namespace ExtensionSystem

#endif // UI_PLUGINERRORVIEW_H

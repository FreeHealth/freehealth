/********************************************************************************
** Form generated from reading ui file 'pluginview.ui'
**
** Created: Sun Sep 13 10:26:56 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PLUGINVIEW_H
#define UI_PLUGINVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

namespace ExtensionSystem {
namespace Internal {

class Ui_PluginView
{
public:
    QGridLayout *gridLayout;
    QTreeWidget *pluginWidget;

    void setupUi(QWidget *ExtensionSystem__Internal__PluginView)
    {
        if (ExtensionSystem__Internal__PluginView->objectName().isEmpty())
            ExtensionSystem__Internal__PluginView->setObjectName(QString::fromUtf8("ExtensionSystem__Internal__PluginView"));
        ExtensionSystem__Internal__PluginView->resize(773, 304);
        gridLayout = new QGridLayout(ExtensionSystem__Internal__PluginView);
        gridLayout->setMargin(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pluginWidget = new QTreeWidget(ExtensionSystem__Internal__PluginView);
        pluginWidget->setObjectName(QString::fromUtf8("pluginWidget"));
        pluginWidget->setAlternatingRowColors(true);
        pluginWidget->setIndentation(0);
        pluginWidget->setRootIsDecorated(false);
        pluginWidget->setUniformRowHeights(true);
        pluginWidget->setItemsExpandable(false);
        pluginWidget->setSortingEnabled(true);
        pluginWidget->setColumnCount(5);

        gridLayout->addWidget(pluginWidget, 0, 0, 1, 1);


        retranslateUi(ExtensionSystem__Internal__PluginView);

        QMetaObject::connectSlotsByName(ExtensionSystem__Internal__PluginView);
    } // setupUi

    void retranslateUi(QWidget *ExtensionSystem__Internal__PluginView)
    {
        QTreeWidgetItem *___qtreewidgetitem = pluginWidget->headerItem();
        ___qtreewidgetitem->setText(4, QApplication::translate("ExtensionSystem::Internal::PluginView", "Location", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(3, QApplication::translate("ExtensionSystem::Internal::PluginView", "Vendor", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(2, QApplication::translate("ExtensionSystem::Internal::PluginView", "Version", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("ExtensionSystem::Internal::PluginView", "Name", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("ExtensionSystem::Internal::PluginView", "State", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ExtensionSystem__Internal__PluginView);
    } // retranslateUi

};

} // namespace Internal
} // namespace ExtensionSystem

namespace ExtensionSystem {
namespace Internal {
namespace Ui {
    class PluginView: public Ui_PluginView {};
} // namespace Ui
} // namespace Internal
} // namespace ExtensionSystem

#endif // UI_PLUGINVIEW_H

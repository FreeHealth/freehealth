/********************************************************************************
** Form generated from reading ui file 'plugindetailsview.ui'
**
** Created: Sun Sep 13 10:26:56 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PLUGINDETAILSVIEW_H
#define UI_PLUGINDETAILSVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace ExtensionSystem {
namespace Internal {

class Ui_PluginDetailsView
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *name;
    QLabel *label_2;
    QLabel *version;
    QLabel *label_3;
    QLabel *compatVersion;
    QLabel *label_4;
    QLabel *vendor;
    QLabel *label_6;
    QLabel *url;
    QLabel *label_7;
    QLabel *location;
    QVBoxLayout *vboxLayout;
    QLabel *label_8;
    QSpacerItem *spacerItem;
    QTextEdit *description;
    QLabel *label_5;
    QLabel *copyright;
    QVBoxLayout *vboxLayout1;
    QLabel *label_9;
    QSpacerItem *spacerItem1;
    QTextEdit *license;
    QVBoxLayout *vboxLayout2;
    QLabel *label_10;
    QSpacerItem *spacerItem2;
    QListWidget *dependencies;

    void setupUi(QWidget *ExtensionSystem__Internal__PluginDetailsView)
    {
        if (ExtensionSystem__Internal__PluginDetailsView->objectName().isEmpty())
            ExtensionSystem__Internal__PluginDetailsView->setObjectName(QString::fromUtf8("ExtensionSystem__Internal__PluginDetailsView"));
        ExtensionSystem__Internal__PluginDetailsView->resize(674, 505);
        gridLayout = new QGridLayout(ExtensionSystem__Internal__PluginDetailsView);
        gridLayout->setMargin(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        name = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        name->setObjectName(QString::fromUtf8("name"));

        gridLayout->addWidget(name, 0, 1, 1, 1);

        label_2 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        version = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        version->setObjectName(QString::fromUtf8("version"));

        gridLayout->addWidget(version, 1, 1, 1, 1);

        label_3 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        compatVersion = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        compatVersion->setObjectName(QString::fromUtf8("compatVersion"));

        gridLayout->addWidget(compatVersion, 2, 1, 1, 1);

        label_4 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        vendor = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        vendor->setObjectName(QString::fromUtf8("vendor"));

        gridLayout->addWidget(vendor, 3, 1, 1, 1);

        label_6 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_6, 4, 0, 1, 1);

        url = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        url->setObjectName(QString::fromUtf8("url"));
        url->setOpenExternalLinks(true);

        gridLayout->addWidget(url, 4, 1, 1, 1);

        label_7 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_7, 5, 0, 1, 1);

        location = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        location->setObjectName(QString::fromUtf8("location"));
        location->setWordWrap(false);

        gridLayout->addWidget(location, 5, 1, 1, 1);

        vboxLayout = new QVBoxLayout();
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        label_8 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout->addWidget(label_8);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem);


        gridLayout->addLayout(vboxLayout, 6, 0, 1, 1);

        description = new QTextEdit(ExtensionSystem__Internal__PluginDetailsView);
        description->setObjectName(QString::fromUtf8("description"));
        description->setTabChangesFocus(true);
        description->setReadOnly(true);

        gridLayout->addWidget(description, 6, 1, 1, 1);

        label_5 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_5, 7, 0, 1, 1);

        copyright = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        copyright->setObjectName(QString::fromUtf8("copyright"));

        gridLayout->addWidget(copyright, 7, 1, 1, 1);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        label_9 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout1->addWidget(label_9);

        spacerItem1 = new QSpacerItem(17, 13, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout1->addItem(spacerItem1);


        gridLayout->addLayout(vboxLayout1, 8, 0, 1, 1);

        license = new QTextEdit(ExtensionSystem__Internal__PluginDetailsView);
        license->setObjectName(QString::fromUtf8("license"));
        license->setTabChangesFocus(true);
        license->setReadOnly(true);

        gridLayout->addWidget(license, 8, 1, 1, 1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        label_10 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        vboxLayout2->addWidget(label_10);

        spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout2->addItem(spacerItem2);


        gridLayout->addLayout(vboxLayout2, 9, 0, 1, 1);

        dependencies = new QListWidget(ExtensionSystem__Internal__PluginDetailsView);
        dependencies->setObjectName(QString::fromUtf8("dependencies"));

        gridLayout->addWidget(dependencies, 9, 1, 1, 1);


        retranslateUi(ExtensionSystem__Internal__PluginDetailsView);

        QMetaObject::connectSlotsByName(ExtensionSystem__Internal__PluginDetailsView);
    } // setupUi

    void retranslateUi(QWidget *ExtensionSystem__Internal__PluginDetailsView)
    {
        label->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Name:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Version:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Compatibility Version:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Vendor:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Url:", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Location:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Description:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Copyright:", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "License:", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Dependencies:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ExtensionSystem__Internal__PluginDetailsView);
    } // retranslateUi

};

} // namespace Internal
} // namespace ExtensionSystem

namespace ExtensionSystem {
namespace Internal {
namespace Ui {
    class PluginDetailsView: public Ui_PluginDetailsView {};
} // namespace Ui
} // namespace Internal
} // namespace ExtensionSystem

#endif // UI_PLUGINDETAILSVIEW_H

/********************************************************************************
** Form generated from reading ui file 'settingsdialog.ui'
**
** Created: Mon Feb 1 11:51:28 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QSplitter>
#include <QtGui/QStackedWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace Core {
namespace Internal {

class Ui_SettingsDialog
{
public:
    QVBoxLayout *vboxLayout;
    QSplitter *splitter;
    QTreeWidget *pageTree;
    QWidget *layoutWidget;
    QVBoxLayout *vboxLayout1;
    QStackedWidget *stackedPages;
    QFrame *line;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Core__Internal__SettingsDialog)
    {
        if (Core__Internal__SettingsDialog->objectName().isEmpty())
            Core__Internal__SettingsDialog->setObjectName(QString::fromUtf8("Core__Internal__SettingsDialog"));
        Core__Internal__SettingsDialog->resize(697, 476);
        vboxLayout = new QVBoxLayout(Core__Internal__SettingsDialog);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        splitter = new QSplitter(Core__Internal__SettingsDialog);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        pageTree = new QTreeWidget(splitter);
        pageTree->setObjectName(QString::fromUtf8("pageTree"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pageTree->sizePolicy().hasHeightForWidth());
        pageTree->setSizePolicy(sizePolicy);
        pageTree->setColumnCount(1);
        splitter->addWidget(pageTree);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        vboxLayout1 = new QVBoxLayout(layoutWidget);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        stackedPages = new QStackedWidget(layoutWidget);
        stackedPages->setObjectName(QString::fromUtf8("stackedPages"));
        stackedPages->setMinimumSize(QSize(350, 250));

        vboxLayout1->addWidget(stackedPages);

        line = new QFrame(layoutWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        vboxLayout1->addWidget(line);

        splitter->addWidget(layoutWidget);

        vboxLayout->addWidget(splitter);

        buttonBox = new QDialogButtonBox(Core__Internal__SettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Help|QDialogButtonBox::Ok|QDialogButtonBox::RestoreDefaults);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(Core__Internal__SettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Core__Internal__SettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Core__Internal__SettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Core__Internal__SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *Core__Internal__SettingsDialog)
    {
        Core__Internal__SettingsDialog->setWindowTitle(QApplication::translate("Core::Internal::SettingsDialog", "Options", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = pageTree->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("Core::Internal::SettingsDialog", "0", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Core__Internal__SettingsDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace Core

namespace Core {
namespace Internal {
namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui
} // namespace Internal
} // namespace Core

#endif // UI_SETTINGSDIALOG_H

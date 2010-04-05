/********************************************************************************
** Form generated from reading ui file 'aboutdialog.ui'
**
** Created: Sun Sep 13 10:28:07 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

namespace Core {
namespace Internal {

class Ui_AboutDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *applicationNameLabel;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *tree;
    QWidget *forStack;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Core__Internal__AboutDialog)
    {
        if (Core__Internal__AboutDialog->objectName().isEmpty())
            Core__Internal__AboutDialog->setObjectName(QString::fromUtf8("Core__Internal__AboutDialog"));
        Core__Internal__AboutDialog->resize(503, 413);
        gridLayout = new QGridLayout(Core__Internal__AboutDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        applicationNameLabel = new QLabel(Core__Internal__AboutDialog);
        applicationNameLabel->setObjectName(QString::fromUtf8("applicationNameLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(applicationNameLabel->sizePolicy().hasHeightForWidth());
        applicationNameLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        applicationNameLabel->setFont(font);
        applicationNameLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(applicationNameLabel, 0, 0, 1, 1);

        line = new QFrame(Core__Internal__AboutDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tree = new QTreeWidget(Core__Internal__AboutDialog);
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

        horizontalLayout->addWidget(tree);

        forStack = new QWidget(Core__Internal__AboutDialog);
        forStack->setObjectName(QString::fromUtf8("forStack"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(forStack->sizePolicy().hasHeightForWidth());
        forStack->setSizePolicy(sizePolicy2);
        forStack->setMinimumSize(QSize(100, 100));

        horizontalLayout->addWidget(forStack);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(Core__Internal__AboutDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 1);


        retranslateUi(Core__Internal__AboutDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Core__Internal__AboutDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Core__Internal__AboutDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Core__Internal__AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *Core__Internal__AboutDialog)
    {
        Core__Internal__AboutDialog->setWindowTitle(QApplication::translate("Core::Internal::AboutDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        applicationNameLabel->setText(QApplication::translate("Core::Internal::AboutDialog", "Application name", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Core__Internal__AboutDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace Core

namespace Core {
namespace Internal {
namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui
} // namespace Internal
} // namespace Core

#endif // UI_ABOUTDIALOG_H

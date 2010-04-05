/********************************************************************************
** Form generated from reading ui file 'mfDrugSelector.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MFDRUGSELECTOR_H
#define UI_MFDRUGSELECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QTableView>
#include <QtGui/QWidget>
#include <utils/widgets/QButtonLineEdit.h>

namespace DrugsWidget {
namespace Internal {

class Ui_DrugSelector
{
public:
    QGridLayout *gridLayout;
    QSplitter *splitter;
    QTableView *InnView;
    QTableView *drugsView;
    QHBoxLayout *horizontalLayout_2;
    Utils::QButtonLineEdit *searchLine;
    QPushButton *textButton;

    void setupUi(QWidget *DrugsWidget__Internal__DrugSelector)
    {
        if (DrugsWidget__Internal__DrugSelector->objectName().isEmpty())
            DrugsWidget__Internal__DrugSelector->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DrugSelector"));
        DrugsWidget__Internal__DrugSelector->resize(432, 192);
        gridLayout = new QGridLayout(DrugsWidget__Internal__DrugSelector);
        gridLayout->setSpacing(0);
        gridLayout->setMargin(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        splitter = new QSplitter(DrugsWidget__Internal__DrugSelector);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        InnView = new QTableView(splitter);
        InnView->setObjectName(QString::fromUtf8("InnView"));
        InnView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        InnView->setAlternatingRowColors(true);
        InnView->setSelectionMode(QAbstractItemView::SingleSelection);
        InnView->setSelectionBehavior(QAbstractItemView::SelectRows);
        InnView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        InnView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        splitter->addWidget(InnView);
        InnView->horizontalHeader()->setVisible(false);
        InnView->horizontalHeader()->setHighlightSections(false);
        InnView->verticalHeader()->setVisible(false);
        InnView->verticalHeader()->setHighlightSections(false);
        drugsView = new QTableView(splitter);
        drugsView->setObjectName(QString::fromUtf8("drugsView"));
        drugsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        drugsView->setAlternatingRowColors(true);
        drugsView->setSelectionMode(QAbstractItemView::SingleSelection);
        drugsView->setSelectionBehavior(QAbstractItemView::SelectRows);
        drugsView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        drugsView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        splitter->addWidget(drugsView);
        drugsView->horizontalHeader()->setVisible(false);
        drugsView->verticalHeader()->setVisible(false);

        gridLayout->addWidget(splitter, 2, 0, 1, 2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        searchLine = new Utils::QButtonLineEdit(DrugsWidget__Internal__DrugSelector);
        searchLine->setObjectName(QString::fromUtf8("searchLine"));

        horizontalLayout_2->addWidget(searchLine);

        textButton = new QPushButton(DrugsWidget__Internal__DrugSelector);
        textButton->setObjectName(QString::fromUtf8("textButton"));
        textButton->setMinimumSize(QSize(20, 20));
        textButton->setMaximumSize(QSize(20, 20));
        textButton->setFlat(true);

        horizontalLayout_2->addWidget(textButton);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__DrugSelector);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DrugSelector);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DrugSelector)
    {
        DrugsWidget__Internal__DrugSelector->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DrugSelector", "Form", 0, QApplication::UnicodeUTF8));
        textButton->setText(QString());
        Q_UNUSED(DrugsWidget__Internal__DrugSelector);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DrugSelector: public Ui_DrugSelector {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_MFDRUGSELECTOR_H

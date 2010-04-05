/********************************************************************************
** Form generated from reading ui file 'mfDosageAvailableWidget.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MFDOSAGEAVAILABLEWIDGET_H
#define UI_MFDOSAGEAVAILABLEWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>
#include <QtGui/QWidget>

namespace DrugsWidget {
namespace Internal {

class Ui_DosageAvailableWidget
{
public:
    QGridLayout *gridLayout;
    QListView *listView;

    void setupUi(QWidget *DrugsWidget__Internal__DosageAvailableWidget)
    {
        if (DrugsWidget__Internal__DosageAvailableWidget->objectName().isEmpty())
            DrugsWidget__Internal__DosageAvailableWidget->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DosageAvailableWidget"));
        DrugsWidget__Internal__DosageAvailableWidget->resize(400, 300);
        gridLayout = new QGridLayout(DrugsWidget__Internal__DosageAvailableWidget);
        gridLayout->setSpacing(0);
        gridLayout->setMargin(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        listView = new QListView(DrugsWidget__Internal__DosageAvailableWidget);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        listView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        listView->setUniformItemSizes(true);

        gridLayout->addWidget(listView, 0, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__DosageAvailableWidget);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DosageAvailableWidget);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DosageAvailableWidget)
    {
        DrugsWidget__Internal__DosageAvailableWidget->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DosageAvailableWidget", "Form", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DosageAvailableWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DosageAvailableWidget: public Ui_DosageAvailableWidget {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_MFDOSAGEAVAILABLEWIDGET_H

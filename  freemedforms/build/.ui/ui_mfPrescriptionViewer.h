/********************************************************************************
** Form generated from reading ui file 'mfPrescriptionViewer.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MFPRESCRIPTIONVIEWER_H
#define UI_MFPRESCRIPTIONVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace DrugsWidget {
namespace Internal {

class Ui_PrescriptionViewer
{
public:
    QVBoxLayout *verticalLayout;
    QListView *listView;

    void setupUi(QWidget *DrugsWidget__Internal__PrescriptionViewer)
    {
        if (DrugsWidget__Internal__PrescriptionViewer->objectName().isEmpty())
            DrugsWidget__Internal__PrescriptionViewer->setObjectName(QString::fromUtf8("DrugsWidget__Internal__PrescriptionViewer"));
        DrugsWidget__Internal__PrescriptionViewer->resize(400, 300);
        verticalLayout = new QVBoxLayout(DrugsWidget__Internal__PrescriptionViewer);
        verticalLayout->setSpacing(0);
        verticalLayout->setMargin(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        listView = new QListView(DrugsWidget__Internal__PrescriptionViewer);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setAlternatingRowColors(true);
        listView->setSelectionMode(QAbstractItemView::SingleSelection);
        listView->setSelectionBehavior(QAbstractItemView::SelectRows);
        listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        listView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        listView->setResizeMode(QListView::Adjust);

        verticalLayout->addWidget(listView);


        retranslateUi(DrugsWidget__Internal__PrescriptionViewer);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__PrescriptionViewer);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__PrescriptionViewer)
    {
        DrugsWidget__Internal__PrescriptionViewer->setWindowTitle(QApplication::translate("DrugsWidget::Internal::PrescriptionViewer", "Form", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__PrescriptionViewer);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class PrescriptionViewer: public Ui_PrescriptionViewer {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_MFPRESCRIPTIONVIEWER_H

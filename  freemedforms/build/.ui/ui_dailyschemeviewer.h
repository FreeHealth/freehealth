/********************************************************************************
** Form generated from reading ui file 'dailyschemeviewer.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DAILYSCHEMEVIEWER_H
#define UI_DAILYSCHEMEVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QRadioButton>
#include <QtGui/QTableView>
#include <QtGui/QWidget>

namespace DrugsWidget {
namespace Internal {

class Ui_DailySchemeViewer
{
public:
    QGridLayout *gridLayout;
    QRadioButton *repeatRadio;
    QRadioButton *distribRadio;
    QTableView *dailyTableView;

    void setupUi(QWidget *DrugsWidget__Internal__DailySchemeViewer)
    {
        if (DrugsWidget__Internal__DailySchemeViewer->objectName().isEmpty())
            DrugsWidget__Internal__DailySchemeViewer->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DailySchemeViewer"));
        DrugsWidget__Internal__DailySchemeViewer->resize(282, 300);
        gridLayout = new QGridLayout(DrugsWidget__Internal__DailySchemeViewer);
        gridLayout->setSpacing(5);
        gridLayout->setMargin(5);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        repeatRadio = new QRadioButton(DrugsWidget__Internal__DailySchemeViewer);
        repeatRadio->setObjectName(QString::fromUtf8("repeatRadio"));

        gridLayout->addWidget(repeatRadio, 0, 0, 1, 1);

        distribRadio = new QRadioButton(DrugsWidget__Internal__DailySchemeViewer);
        distribRadio->setObjectName(QString::fromUtf8("distribRadio"));

        gridLayout->addWidget(distribRadio, 0, 1, 1, 1);

        dailyTableView = new QTableView(DrugsWidget__Internal__DailySchemeViewer);
        dailyTableView->setObjectName(QString::fromUtf8("dailyTableView"));
        dailyTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
        dailyTableView->setAlternatingRowColors(true);
        dailyTableView->horizontalHeader()->setVisible(false);
        dailyTableView->horizontalHeader()->setStretchLastSection(true);
        dailyTableView->verticalHeader()->setVisible(false);

        gridLayout->addWidget(dailyTableView, 1, 0, 1, 2);


        retranslateUi(DrugsWidget__Internal__DailySchemeViewer);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DailySchemeViewer);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DailySchemeViewer)
    {
        DrugsWidget__Internal__DailySchemeViewer->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DailySchemeViewer", "Form", 0, QApplication::UnicodeUTF8));
        repeatRadio->setText(QApplication::translate("DrugsWidget::Internal::DailySchemeViewer", "Repeat", 0, QApplication::UnicodeUTF8));
        distribRadio->setText(QApplication::translate("DrugsWidget::Internal::DailySchemeViewer", "Distribution", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DailySchemeViewer);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DailySchemeViewer: public Ui_DailySchemeViewer {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_DAILYSCHEMEVIEWER_H

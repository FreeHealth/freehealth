/********************************************************************************
** Form generated from reading ui file 'mfDrugsCentralWidget.ui'
**
** Created: Tue Feb 9 13:56:52 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MFDRUGSCENTRALWIDGET_H
#define UI_MFDRUGSCENTRALWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSplitter>
#include <QtGui/QWidget>
#include <drugsplugin/drugswidget/mfDrugSelector.h>
#include <drugsplugin/drugswidget/mfPrescriptionViewer.h>

namespace DrugsWidget {
namespace Internal {

class Ui_DrugsCentralWidget
{
public:
    QGridLayout *gridLayout;
    QSplitter *splitter;
    DrugSelector *m_DrugSelector;
    PrescriptionViewer *m_PrescriptionView;

    void setupUi(QWidget *DrugsWidget__Internal__DrugsCentralWidget)
    {
        if (DrugsWidget__Internal__DrugsCentralWidget->objectName().isEmpty())
            DrugsWidget__Internal__DrugsCentralWidget->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DrugsCentralWidget"));
        DrugsWidget__Internal__DrugsCentralWidget->resize(400, 300);
        gridLayout = new QGridLayout(DrugsWidget__Internal__DrugsCentralWidget);
        gridLayout->setSpacing(0);
        gridLayout->setMargin(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        splitter = new QSplitter(DrugsWidget__Internal__DrugsCentralWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        m_DrugSelector = new DrugSelector(splitter);
        m_DrugSelector->setObjectName(QString::fromUtf8("m_DrugSelector"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_DrugSelector->sizePolicy().hasHeightForWidth());
        m_DrugSelector->setSizePolicy(sizePolicy);
        splitter->addWidget(m_DrugSelector);
        m_PrescriptionView = new PrescriptionViewer(splitter);
        m_PrescriptionView->setObjectName(QString::fromUtf8("m_PrescriptionView"));
        sizePolicy.setHeightForWidth(m_PrescriptionView->sizePolicy().hasHeightForWidth());
        m_PrescriptionView->setSizePolicy(sizePolicy);
        splitter->addWidget(m_PrescriptionView);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);


        retranslateUi(DrugsWidget__Internal__DrugsCentralWidget);

        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DrugsCentralWidget);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DrugsCentralWidget)
    {
        DrugsWidget__Internal__DrugsCentralWidget->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DrugsCentralWidget", "Form", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DrugsCentralWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DrugsCentralWidget: public Ui_DrugsCentralWidget {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_MFDRUGSCENTRALWIDGET_H

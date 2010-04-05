/********************************************************************************
** Form generated from reading ui file 'templatesview.ui'
**
** Created: Mon Feb 1 19:10:03 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEMPLATESVIEW_H
#define UI_TEMPLATESVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace Templates {
namespace Internal {

class Ui_TemplatesView
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *categoryLayout;
    QTreeView *categoryTreeView;

    void setupUi(QWidget *Templates__Internal__TemplatesView)
    {
        if (Templates__Internal__TemplatesView->objectName().isEmpty())
            Templates__Internal__TemplatesView->setObjectName(QString::fromUtf8("Templates__Internal__TemplatesView"));
        Templates__Internal__TemplatesView->resize(400, 300);
        gridLayout = new QGridLayout(Templates__Internal__TemplatesView);
        gridLayout->setMargin(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        categoryLayout = new QVBoxLayout();
        categoryLayout->setSpacing(2);
        categoryLayout->setObjectName(QString::fromUtf8("categoryLayout"));
        categoryTreeView = new QTreeView(Templates__Internal__TemplatesView);
        categoryTreeView->setObjectName(QString::fromUtf8("categoryTreeView"));
        categoryTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        categoryTreeView->setDragEnabled(true);
        categoryTreeView->setDragDropMode(QAbstractItemView::InternalMove);
        categoryTreeView->setAlternatingRowColors(true);
        categoryTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        categoryTreeView->setAutoExpandDelay(200);
        categoryTreeView->setUniformRowHeights(true);
        categoryTreeView->header()->setVisible(false);

        categoryLayout->addWidget(categoryTreeView);


        gridLayout->addLayout(categoryLayout, 0, 0, 1, 1);


        retranslateUi(Templates__Internal__TemplatesView);

        QMetaObject::connectSlotsByName(Templates__Internal__TemplatesView);
    } // setupUi

    void retranslateUi(QWidget *Templates__Internal__TemplatesView)
    {
        Templates__Internal__TemplatesView->setWindowTitle(QApplication::translate("Templates::Internal::TemplatesView", "Form", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        categoryTreeView->setToolTip(QApplication::translate("Templates::Internal::TemplatesView", "Categories tree", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Q_UNUSED(Templates__Internal__TemplatesView);
    } // retranslateUi

};

} // namespace Internal
} // namespace Templates

namespace Templates {
namespace Internal {
namespace Ui {
    class TemplatesView: public Ui_TemplatesView {};
} // namespace Ui
} // namespace Internal
} // namespace Templates

#endif // UI_TEMPLATESVIEW_H

/********************************************************************************
** Form generated from reading ui file 'tablepropertiesdialog.ui'
**
** Created: Mon Feb 1 11:53:06 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TABLEPROPERTIESDIALOG_H
#define UI_TABLEPROPERTIESDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

namespace Editor {
namespace Internal {

class Ui_TablePropertiesDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QFrame *line;
    QVBoxLayout *verticalLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Editor__Internal__TablePropertiesDialog)
    {
        if (Editor__Internal__TablePropertiesDialog->objectName().isEmpty())
            Editor__Internal__TablePropertiesDialog->setObjectName(QString::fromUtf8("Editor__Internal__TablePropertiesDialog"));
        Editor__Internal__TablePropertiesDialog->resize(400, 300);
        gridLayout = new QGridLayout(Editor__Internal__TablePropertiesDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(Editor__Internal__TablePropertiesDialog);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        line = new QFrame(Editor__Internal__TablePropertiesDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

        gridLayout->addLayout(verticalLayout, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(Editor__Internal__TablePropertiesDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Help|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 1);


        retranslateUi(Editor__Internal__TablePropertiesDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Editor__Internal__TablePropertiesDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Editor__Internal__TablePropertiesDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Editor__Internal__TablePropertiesDialog);
    } // setupUi

    void retranslateUi(QDialog *Editor__Internal__TablePropertiesDialog)
    {
        Editor__Internal__TablePropertiesDialog->setWindowTitle(QApplication::translate("Editor::Internal::TablePropertiesDialog", "Rich Text - Table Properties", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Editor::Internal::TablePropertiesDialog", "Rich Text - Table Properties", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Editor__Internal__TablePropertiesDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace Editor

namespace Editor {
namespace Internal {
namespace Ui {
    class TablePropertiesDialog: public Ui_TablePropertiesDialog {};
} // namespace Ui
} // namespace Internal
} // namespace Editor

#endif // UI_TABLEPROPERTIESDIALOG_H

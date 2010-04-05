/********************************************************************************
** Form generated from reading ui file 'templatescreationdialog.ui'
**
** Created: Mon Feb 1 19:10:03 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEMPLATESCREATIONDIALOG_H
#define UI_TEMPLATESCREATIONDIALOG_H

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
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <templatesplugin/templatesview.h>

namespace Templates {
namespace Internal {

class Ui_TemplatesCreationDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QFrame *line;
    QLabel *label_4;
    QLineEdit *userLineEdit;
    QLabel *label_2;
    QLineEdit *nameLineEdit;
    QLabel *label_3;
    Templates::TemplatesView *parentCategory;
    QLabel *label_5;
    QTextEdit *summaryTextEdit;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *viewButton_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Templates__Internal__TemplatesCreationDialog)
    {
        if (Templates__Internal__TemplatesCreationDialog->objectName().isEmpty())
            Templates__Internal__TemplatesCreationDialog->setObjectName(QString::fromUtf8("Templates__Internal__TemplatesCreationDialog"));
        Templates__Internal__TemplatesCreationDialog->resize(491, 477);
        gridLayout = new QGridLayout(Templates__Internal__TemplatesCreationDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(Templates__Internal__TemplatesCreationDialog);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 3);

        line = new QFrame(Templates__Internal__TemplatesCreationDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 3);

        label_4 = new QLabel(Templates__Internal__TemplatesCreationDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 2, 0, 1, 1);

        userLineEdit = new QLineEdit(Templates__Internal__TemplatesCreationDialog);
        userLineEdit->setObjectName(QString::fromUtf8("userLineEdit"));

        gridLayout->addWidget(userLineEdit, 2, 2, 1, 1);

        label_2 = new QLabel(Templates__Internal__TemplatesCreationDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        nameLineEdit = new QLineEdit(Templates__Internal__TemplatesCreationDialog);
        nameLineEdit->setObjectName(QString::fromUtf8("nameLineEdit"));

        gridLayout->addWidget(nameLineEdit, 3, 2, 1, 1);

        label_3 = new QLabel(Templates__Internal__TemplatesCreationDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        parentCategory = new Templates::TemplatesView(Templates__Internal__TemplatesCreationDialog);
        parentCategory->setObjectName(QString::fromUtf8("parentCategory"));

        gridLayout->addWidget(parentCategory, 4, 2, 1, 1);

        label_5 = new QLabel(Templates__Internal__TemplatesCreationDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        summaryTextEdit = new QTextEdit(Templates__Internal__TemplatesCreationDialog);
        summaryTextEdit->setObjectName(QString::fromUtf8("summaryTextEdit"));

        gridLayout->addWidget(summaryTextEdit, 6, 0, 1, 3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        viewButton_2 = new QPushButton(Templates__Internal__TemplatesCreationDialog);
        viewButton_2->setObjectName(QString::fromUtf8("viewButton_2"));

        horizontalLayout_2->addWidget(viewButton_2);

        buttonBox = new QDialogButtonBox(Templates__Internal__TemplatesCreationDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_2->addWidget(buttonBox);


        gridLayout->addLayout(horizontalLayout_2, 7, 0, 1, 3);

        QWidget::setTabOrder(userLineEdit, nameLineEdit);
        QWidget::setTabOrder(nameLineEdit, summaryTextEdit);
        QWidget::setTabOrder(summaryTextEdit, viewButton_2);
        QWidget::setTabOrder(viewButton_2, buttonBox);

        retranslateUi(Templates__Internal__TemplatesCreationDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Templates__Internal__TemplatesCreationDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Templates__Internal__TemplatesCreationDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Templates__Internal__TemplatesCreationDialog);
    } // setupUi

    void retranslateUi(QDialog *Templates__Internal__TemplatesCreationDialog)
    {
        Templates__Internal__TemplatesCreationDialog->setWindowTitle(QApplication::translate("Templates::Internal::TemplatesCreationDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Templates::Internal::TemplatesCreationDialog", "Template creation", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Templates::Internal::TemplatesCreationDialog", "User owner", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Templates::Internal::TemplatesCreationDialog", "Name", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Templates::Internal::TemplatesCreationDialog", "Parent category", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Templates::Internal::TemplatesCreationDialog", "Summary", 0, QApplication::UnicodeUTF8));
        viewButton_2->setText(QApplication::translate("Templates::Internal::TemplatesCreationDialog", "View content", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Templates__Internal__TemplatesCreationDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace Templates

namespace Templates {
namespace Internal {
namespace Ui {
    class TemplatesCreationDialog: public Ui_TemplatesCreationDialog {};
} // namespace Ui
} // namespace Internal
} // namespace Templates

#endif // UI_TEMPLATESCREATIONDIALOG_H

/********************************************************************************
** Form generated from reading ui file 'templatescontenteditor.ui'
**
** Created: Mon Feb 1 19:10:03 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEMPLATESCONTENTEDITOR_H
#define UI_TEMPLATESCONTENTEDITOR_H

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
#include <QtGui/QTextEdit>

namespace Templates {
namespace Internal {

class Ui_TemplatesContentEditor
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QFrame *line;
    QTextEdit *contentTextEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Templates__Internal__TemplatesContentEditor)
    {
        if (Templates__Internal__TemplatesContentEditor->objectName().isEmpty())
            Templates__Internal__TemplatesContentEditor->setObjectName(QString::fromUtf8("Templates__Internal__TemplatesContentEditor"));
        Templates__Internal__TemplatesContentEditor->resize(400, 300);
        gridLayout = new QGridLayout(Templates__Internal__TemplatesContentEditor);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(Templates__Internal__TemplatesContentEditor);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        line = new QFrame(Templates__Internal__TemplatesContentEditor);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 1);

        contentTextEdit = new QTextEdit(Templates__Internal__TemplatesContentEditor);
        contentTextEdit->setObjectName(QString::fromUtf8("contentTextEdit"));

        gridLayout->addWidget(contentTextEdit, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(Templates__Internal__TemplatesContentEditor);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 1);


        retranslateUi(Templates__Internal__TemplatesContentEditor);
        QObject::connect(buttonBox, SIGNAL(rejected()), Templates__Internal__TemplatesContentEditor, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), Templates__Internal__TemplatesContentEditor, SLOT(accept()));

        QMetaObject::connectSlotsByName(Templates__Internal__TemplatesContentEditor);
    } // setupUi

    void retranslateUi(QDialog *Templates__Internal__TemplatesContentEditor)
    {
        Templates__Internal__TemplatesContentEditor->setWindowTitle(QApplication::translate("Templates::Internal::TemplatesContentEditor", "Content editor", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Templates::Internal::TemplatesContentEditor", "Content editor", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Templates__Internal__TemplatesContentEditor);
    } // retranslateUi

};

} // namespace Internal
} // namespace Templates

namespace Templates {
namespace Internal {
namespace Ui {
    class TemplatesContentEditor: public Ui_TemplatesContentEditor {};
} // namespace Ui
} // namespace Internal
} // namespace Templates

#endif // UI_TEMPLATESCONTENTEDITOR_H

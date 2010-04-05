/********************************************************************************
** Form generated from reading ui file 'tabledialog.ui'
**
** Created: Sun Sep 13 10:30:11 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TABLEDIALOG_H
#define UI_TABLEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>

namespace Editor {
namespace Internal {

class Ui_TableDialog
{
public:
    QGridLayout *gridLayout_3;
    QLabel *label;
    QFrame *line;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QSlider *colSlider;
    QSpinBox *colSpin;
    QLabel *label_3;
    QSlider *rowSlider;
    QSpinBox *rowSpin;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *label_4;
    QSlider *cellSpacingSlider;
    QSpinBox *cellSpacingSpin;
    QLabel *label_5;
    QSlider *cellPaddingSlider;
    QSpinBox *cellPaddingSpin;
    QLabel *label_6;
    QSlider *borderSlider;
    QSpinBox *borderSpin;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_4;
    QCheckBox *header;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Editor__Internal__TableDialog)
    {
        if (Editor__Internal__TableDialog->objectName().isEmpty())
            Editor__Internal__TableDialog->setObjectName(QString::fromUtf8("Editor__Internal__TableDialog"));
        Editor__Internal__TableDialog->resize(343, 436);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Editor__Internal__TableDialog->sizePolicy().hasHeightForWidth());
        Editor__Internal__TableDialog->setSizePolicy(sizePolicy);
        Editor__Internal__TableDialog->setContextMenuPolicy(Qt::NoContextMenu);
        gridLayout_3 = new QGridLayout(Editor__Internal__TableDialog);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label = new QLabel(Editor__Internal__TableDialog);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        line = new QFrame(Editor__Internal__TableDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(line, 1, 0, 1, 1);

        groupBox = new QGroupBox(Editor__Internal__TableDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        groupBox->setFont(font1);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy2);
        QFont font2;
        font2.setBold(false);
        font2.setWeight(50);
        label_2->setFont(font2);

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        colSlider = new QSlider(groupBox);
        colSlider->setObjectName(QString::fromUtf8("colSlider"));
        colSlider->setMinimum(1);
        colSlider->setMaximum(20);
        colSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(colSlider, 0, 1, 1, 1);

        colSpin = new QSpinBox(groupBox);
        colSpin->setObjectName(QString::fromUtf8("colSpin"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(colSpin->sizePolicy().hasHeightForWidth());
        colSpin->setSizePolicy(sizePolicy3);
        colSpin->setMinimumSize(QSize(50, 28));
        colSpin->setFont(font2);
        colSpin->setMinimum(1);
        colSpin->setMaximum(20);

        gridLayout->addWidget(colSpin, 0, 2, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy2.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy2);
        label_3->setFont(font2);

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        rowSlider = new QSlider(groupBox);
        rowSlider->setObjectName(QString::fromUtf8("rowSlider"));
        rowSlider->setMinimum(1);
        rowSlider->setMaximum(20);
        rowSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(rowSlider, 1, 1, 1, 1);

        rowSpin = new QSpinBox(groupBox);
        rowSpin->setObjectName(QString::fromUtf8("rowSpin"));
        sizePolicy3.setHeightForWidth(rowSpin->sizePolicy().hasHeightForWidth());
        rowSpin->setSizePolicy(sizePolicy3);
        rowSpin->setMinimumSize(QSize(50, 28));
        rowSpin->setFont(font2);
        rowSpin->setMinimum(1);
        rowSpin->setMaximum(20);

        gridLayout->addWidget(rowSpin, 1, 2, 1, 1);


        gridLayout_3->addWidget(groupBox, 2, 0, 1, 1);

        groupBox_2 = new QGroupBox(Editor__Internal__TableDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setFont(font1);
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy2.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy2);
        label_4->setFont(font2);

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        cellSpacingSlider = new QSlider(groupBox_2);
        cellSpacingSlider->setObjectName(QString::fromUtf8("cellSpacingSlider"));
        cellSpacingSlider->setMaximum(20);
        cellSpacingSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(cellSpacingSlider, 0, 1, 1, 1);

        cellSpacingSpin = new QSpinBox(groupBox_2);
        cellSpacingSpin->setObjectName(QString::fromUtf8("cellSpacingSpin"));
        sizePolicy3.setHeightForWidth(cellSpacingSpin->sizePolicy().hasHeightForWidth());
        cellSpacingSpin->setSizePolicy(sizePolicy3);
        cellSpacingSpin->setMinimumSize(QSize(50, 28));
        cellSpacingSpin->setFont(font2);
        cellSpacingSpin->setMaximum(20);

        gridLayout_2->addWidget(cellSpacingSpin, 0, 2, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy2.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy2);
        label_5->setFont(font2);

        gridLayout_2->addWidget(label_5, 1, 0, 1, 1);

        cellPaddingSlider = new QSlider(groupBox_2);
        cellPaddingSlider->setObjectName(QString::fromUtf8("cellPaddingSlider"));
        cellPaddingSlider->setMaximum(20);
        cellPaddingSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(cellPaddingSlider, 1, 1, 1, 1);

        cellPaddingSpin = new QSpinBox(groupBox_2);
        cellPaddingSpin->setObjectName(QString::fromUtf8("cellPaddingSpin"));
        sizePolicy3.setHeightForWidth(cellPaddingSpin->sizePolicy().hasHeightForWidth());
        cellPaddingSpin->setSizePolicy(sizePolicy3);
        cellPaddingSpin->setMinimumSize(QSize(50, 28));
        cellPaddingSpin->setFont(font2);
        cellPaddingSpin->setMaximum(20);

        gridLayout_2->addWidget(cellPaddingSpin, 1, 2, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy2.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy2);
        label_6->setFont(font2);

        gridLayout_2->addWidget(label_6, 2, 0, 1, 1);

        borderSlider = new QSlider(groupBox_2);
        borderSlider->setObjectName(QString::fromUtf8("borderSlider"));
        borderSlider->setMaximum(20);
        borderSlider->setValue(1);
        borderSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(borderSlider, 2, 1, 1, 1);

        borderSpin = new QSpinBox(groupBox_2);
        borderSpin->setObjectName(QString::fromUtf8("borderSpin"));
        sizePolicy3.setHeightForWidth(borderSpin->sizePolicy().hasHeightForWidth());
        borderSpin->setSizePolicy(sizePolicy3);
        borderSpin->setMinimumSize(QSize(50, 28));
        borderSpin->setFont(font2);
        borderSpin->setMaximum(20);
        borderSpin->setValue(1);

        gridLayout_2->addWidget(borderSpin, 2, 2, 1, 1);


        gridLayout_3->addWidget(groupBox_2, 3, 0, 1, 1);

        groupBox_3 = new QGroupBox(Editor__Internal__TableDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setFont(font1);
        gridLayout_4 = new QGridLayout(groupBox_3);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        header = new QCheckBox(groupBox_3);
        header->setObjectName(QString::fromUtf8("header"));
        header->setFont(font2);

        gridLayout_4->addWidget(header, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_3, 4, 0, 1, 1);

        buttonBox = new QDialogButtonBox(Editor__Internal__TableDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_3->addWidget(buttonBox, 5, 0, 1, 1);


        retranslateUi(Editor__Internal__TableDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Editor__Internal__TableDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Editor__Internal__TableDialog, SLOT(reject()));
        QObject::connect(colSlider, SIGNAL(valueChanged(int)), colSpin, SLOT(setValue(int)));
        QObject::connect(rowSlider, SIGNAL(valueChanged(int)), rowSpin, SLOT(setValue(int)));
        QObject::connect(colSpin, SIGNAL(valueChanged(int)), colSlider, SLOT(setValue(int)));
        QObject::connect(rowSpin, SIGNAL(valueChanged(int)), rowSlider, SLOT(setValue(int)));
        QObject::connect(cellSpacingSlider, SIGNAL(valueChanged(int)), cellSpacingSpin, SLOT(setValue(int)));
        QObject::connect(cellSpacingSpin, SIGNAL(valueChanged(int)), cellSpacingSlider, SLOT(setValue(int)));
        QObject::connect(cellPaddingSlider, SIGNAL(valueChanged(int)), cellPaddingSpin, SLOT(setValue(int)));
        QObject::connect(borderSlider, SIGNAL(valueChanged(int)), borderSpin, SLOT(setValue(int)));
        QObject::connect(borderSpin, SIGNAL(valueChanged(int)), borderSlider, SLOT(setValue(int)));
        QObject::connect(cellPaddingSpin, SIGNAL(valueChanged(int)), cellPaddingSlider, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(Editor__Internal__TableDialog);
    } // setupUi

    void retranslateUi(QDialog *Editor__Internal__TableDialog)
    {
        Editor__Internal__TableDialog->setWindowTitle(QApplication::translate("Editor::Internal::TableDialog", "Table creation", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Editor::Internal::TableDialog", "Rich Text Editor Table Creator", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Editor::Internal::TableDialog", "Size of the table", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Editor::Internal::TableDialog", "Number of columns", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Editor::Internal::TableDialog", "Number of rows", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Editor::Internal::TableDialog", "Format of the table", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Editor::Internal::TableDialog", "Cell spacing", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Editor::Internal::TableDialog", "Cell padding", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Editor::Internal::TableDialog", "Border", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("Editor::Internal::TableDialog", "Table headers", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        header->setToolTip(QApplication::translate("Editor::Internal::TableDialog", "You can define the first row as header. The table header row get repeated when a table is broken across a page boundary.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        header->setText(QApplication::translate("Editor::Internal::TableDialog", "Define the first row as header", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Editor__Internal__TableDialog);
    } // retranslateUi

};

} // namespace Internal
} // namespace Editor

namespace Editor {
namespace Internal {
namespace Ui {
    class TableDialog: public Ui_TableDialog {};
} // namespace Ui
} // namespace Internal
} // namespace Editor

#endif // UI_TABLEDIALOG_H

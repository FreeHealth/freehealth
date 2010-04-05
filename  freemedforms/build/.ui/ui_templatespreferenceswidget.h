/********************************************************************************
** Form generated from reading ui file 'templatespreferenceswidget.ui'
**
** Created: Mon Feb 1 19:10:03 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEMPLATESPREFERENCESWIDGET_H
#define UI_TEMPLATESPREFERENCESWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFontComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <utils/widgets/colorbuttonchooser.h>

namespace Templates {
namespace Internal {

class Ui_TemplatesPreferencesWidget
{
public:
    QGridLayout *gridLayout_2;
    QLabel *label_6;
    QFrame *line;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QCheckBox *promptBox;
    QCheckBox *deletionBox;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_5;
    QFontComboBox *fontBox;
    QSpinBox *sizeSpin;
    QCheckBox *autoExpandBox;
    QCheckBox *lockViewBox;
    QLabel *label;
    Utils::ColorButtonChooser *categoryBackgroundButton;
    QLabel *label_2;
    Utils::ColorButtonChooser *categoryForegroundButton;
    QLabel *label_3;
    Utils::ColorButtonChooser *templateBackgroundButton;
    QLabel *label_4;
    Utils::ColorButtonChooser *templateForegroundButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *Templates__Internal__TemplatesPreferencesWidget)
    {
        if (Templates__Internal__TemplatesPreferencesWidget->objectName().isEmpty())
            Templates__Internal__TemplatesPreferencesWidget->setObjectName(QString::fromUtf8("Templates__Internal__TemplatesPreferencesWidget"));
        Templates__Internal__TemplatesPreferencesWidget->resize(465, 519);
        gridLayout_2 = new QGridLayout(Templates__Internal__TemplatesPreferencesWidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_6 = new QLabel(Templates__Internal__TemplatesPreferencesWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_6->setFont(font);
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_6, 0, 0, 1, 2);

        line = new QFrame(Templates__Internal__TemplatesPreferencesWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(line, 1, 0, 1, 2);

        groupBox = new QGroupBox(Templates__Internal__TemplatesPreferencesWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        promptBox = new QCheckBox(groupBox);
        promptBox->setObjectName(QString::fromUtf8("promptBox"));

        gridLayout_3->addWidget(promptBox, 1, 0, 1, 1);

        deletionBox = new QCheckBox(groupBox);
        deletionBox->setObjectName(QString::fromUtf8("deletionBox"));

        gridLayout_3->addWidget(deletionBox, 2, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 2, 0, 1, 2);

        groupBox_2 = new QGroupBox(Templates__Internal__TemplatesPreferencesWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout->addWidget(label_5);

        fontBox = new QFontComboBox(groupBox_2);
        fontBox->setObjectName(QString::fromUtf8("fontBox"));

        horizontalLayout->addWidget(fontBox);

        sizeSpin = new QSpinBox(groupBox_2);
        sizeSpin->setObjectName(QString::fromUtf8("sizeSpin"));

        horizontalLayout->addWidget(sizeSpin);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 2);

        autoExpandBox = new QCheckBox(groupBox_2);
        autoExpandBox->setObjectName(QString::fromUtf8("autoExpandBox"));

        gridLayout->addWidget(autoExpandBox, 1, 0, 1, 2);

        lockViewBox = new QCheckBox(groupBox_2);
        lockViewBox->setObjectName(QString::fromUtf8("lockViewBox"));

        gridLayout->addWidget(lockViewBox, 2, 0, 1, 1);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 3, 0, 1, 1);

        categoryBackgroundButton = new Utils::ColorButtonChooser(groupBox_2);
        categoryBackgroundButton->setObjectName(QString::fromUtf8("categoryBackgroundButton"));
        categoryBackgroundButton->setMinimumSize(QSize(22, 22));
        categoryBackgroundButton->setMaximumSize(QSize(22, 22));

        gridLayout->addWidget(categoryBackgroundButton, 3, 1, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 4, 0, 1, 1);

        categoryForegroundButton = new Utils::ColorButtonChooser(groupBox_2);
        categoryForegroundButton->setObjectName(QString::fromUtf8("categoryForegroundButton"));
        categoryForegroundButton->setMinimumSize(QSize(22, 22));
        categoryForegroundButton->setMaximumSize(QSize(22, 22));

        gridLayout->addWidget(categoryForegroundButton, 4, 1, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 5, 0, 1, 1);

        templateBackgroundButton = new Utils::ColorButtonChooser(groupBox_2);
        templateBackgroundButton->setObjectName(QString::fromUtf8("templateBackgroundButton"));
        templateBackgroundButton->setMinimumSize(QSize(22, 22));
        templateBackgroundButton->setMaximumSize(QSize(22, 22));

        gridLayout->addWidget(templateBackgroundButton, 5, 1, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 6, 0, 1, 1);

        templateForegroundButton = new Utils::ColorButtonChooser(groupBox_2);
        templateForegroundButton->setObjectName(QString::fromUtf8("templateForegroundButton"));
        templateForegroundButton->setMinimumSize(QSize(22, 22));
        templateForegroundButton->setMaximumSize(QSize(22, 22));

        gridLayout->addWidget(templateForegroundButton, 6, 1, 1, 1);


        gridLayout_2->addWidget(groupBox_2, 3, 0, 1, 2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 4, 0, 1, 1);


        retranslateUi(Templates__Internal__TemplatesPreferencesWidget);

        QMetaObject::connectSlotsByName(Templates__Internal__TemplatesPreferencesWidget);
    } // setupUi

    void retranslateUi(QWidget *Templates__Internal__TemplatesPreferencesWidget)
    {
        Templates__Internal__TemplatesPreferencesWidget->setWindowTitle(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Templates preferences", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Templates preferences", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Data saving", 0, QApplication::UnicodeUTF8));
        promptBox->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Always save data without prompting user", 0, QApplication::UnicodeUTF8));
        deletionBox->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Always ask for confirmation before deletion", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Presentation", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Font", 0, QApplication::UnicodeUTF8));
        autoExpandBox->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Always expand categories and templates tree", 0, QApplication::UnicodeUTF8));
        lockViewBox->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Lock category view at startup", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Category background color", 0, QApplication::UnicodeUTF8));
        categoryBackgroundButton->setText(QString());
        label_2->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Category foreground color", 0, QApplication::UnicodeUTF8));
        categoryForegroundButton->setText(QString());
        label_3->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Template background color", 0, QApplication::UnicodeUTF8));
        templateBackgroundButton->setText(QString());
        label_4->setText(QApplication::translate("Templates::Internal::TemplatesPreferencesWidget", "Template foreground color", 0, QApplication::UnicodeUTF8));
        templateForegroundButton->setText(QString());
        Q_UNUSED(Templates__Internal__TemplatesPreferencesWidget);
    } // retranslateUi

};

} // namespace Internal
} // namespace Templates

namespace Templates {
namespace Internal {
namespace Ui {
    class TemplatesPreferencesWidget: public Ui_TemplatesPreferencesWidget {};
} // namespace Ui
} // namespace Internal
} // namespace Templates

#endif // UI_TEMPLATESPREFERENCESWIDGET_H

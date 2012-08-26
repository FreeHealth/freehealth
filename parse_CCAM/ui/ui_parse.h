/********************************************************************************
** Form generated from reading UI file 'parse.ui'
**
** Created: Fri Jun 15 13:25:00 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARSE_H
#define UI_PARSE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ParseWidget
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *pathLabel;
    QLabel *activityLabel;
    QPushButton *searchButton;
    QPushButton *ccamButton;
    QPushButton *ngapButton;
    QPushButton *databaseButton;
    QPushButton *quitButton;

    void setupUi(QWidget *ParseWidget)
    {
        if (ParseWidget->objectName().isEmpty())
            ParseWidget->setObjectName(QString::fromUtf8("ParseWidget"));
        ParseWidget->resize(609, 139);
        gridLayout_2 = new QGridLayout(ParseWidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pathLabel = new QLabel(ParseWidget);
        pathLabel->setObjectName(QString::fromUtf8("pathLabel"));
        pathLabel->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(pathLabel, 0, 0, 1, 5);

        activityLabel = new QLabel(ParseWidget);
        activityLabel->setObjectName(QString::fromUtf8("activityLabel"));
        activityLabel->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(activityLabel, 1, 0, 1, 5);

        searchButton = new QPushButton(ParseWidget);
        searchButton->setObjectName(QString::fromUtf8("searchButton"));

        gridLayout->addWidget(searchButton, 2, 0, 1, 1);

        ccamButton = new QPushButton(ParseWidget);
        ccamButton->setObjectName(QString::fromUtf8("ccamButton"));

        gridLayout->addWidget(ccamButton, 2, 1, 1, 1);

        ngapButton = new QPushButton(ParseWidget);
        ngapButton->setObjectName(QString::fromUtf8("ngapButton"));

        gridLayout->addWidget(ngapButton, 2, 2, 1, 1);

        databaseButton = new QPushButton(ParseWidget);
        databaseButton->setObjectName(QString::fromUtf8("databaseButton"));
        databaseButton->setMinimumSize(QSize(171, 0));

        gridLayout->addWidget(databaseButton, 2, 3, 1, 1);

        quitButton = new QPushButton(ParseWidget);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));

        gridLayout->addWidget(quitButton, 2, 4, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(ParseWidget);

        QMetaObject::connectSlotsByName(ParseWidget);
    } // setupUi

    void retranslateUi(QWidget *ParseWidget)
    {
        ParseWidget->setWindowTitle(QApplication::translate("ParseWidget", "ParseCCAM", 0, QApplication::UnicodeUTF8));
        pathLabel->setText(QApplication::translate("ParseWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        activityLabel->setText(QApplication::translate("ParseWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        searchButton->setText(QApplication::translate("ParseWidget", "1 : Search File", 0, QApplication::UnicodeUTF8));
        ccamButton->setText(QApplication::translate("ParseWidget", " 2 : CCAM", 0, QApplication::UnicodeUTF8));
        ngapButton->setText(QApplication::translate("ParseWidget", "3 : Merge All", 0, QApplication::UnicodeUTF8));
        databaseButton->setText(QApplication::translate("ParseWidget", "4 : Create MPDatapack", 0, QApplication::UnicodeUTF8));
        quitButton->setText(QApplication::translate("ParseWidget", "&Quit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ParseWidget: public Ui_ParseWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARSE_H

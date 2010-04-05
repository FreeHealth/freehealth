/********************************************************************************
** Form generated from reading ui file 'mfDosageViewer.ui'
**
** Created: Mon Feb 1 19:11:48 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MFDOSAGEVIEWER_H
#define UI_MFDOSAGEVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>
#include <drugswidget/dailyschemeviewer.h>

namespace DrugsWidget {
namespace Internal {

class Ui_DosageViewer
{
public:
    QGridLayout *gridLayout;
    QFrame *line;
    QHBoxLayout *labelLayout;
    QLabel *labelOfDosageLabel;
    QLineEdit *labelLineEdit;
    QPushButton *frenchRCPButton;
    QTabWidget *tabWidget;
    QWidget *tabBasics;
    QGridLayout *gridLayout_5;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_3;
    QCheckBox *fromToIntakesCheck;
    QHBoxLayout *horizontalLayout_4;
    QLabel *intakesFromLabel;
    QDoubleSpinBox *intakesFromSpin;
    QLabel *intakesToLabel;
    QDoubleSpinBox *intakesToSpin;
    QComboBox *intakesCombo;
    QPushButton *userformsButton;
    QHBoxLayout *horizontalLayout_5;
    QLabel *intakesFromLabel_3;
    QSpinBox *periodSpin;
    QComboBox *periodSchemeCombo;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_4;
    QCheckBox *fromToDurationCheck;
    QHBoxLayout *horizontalLayout_6;
    QLabel *durationFromLabel;
    QDoubleSpinBox *durationFromSpin;
    QLabel *durationToLabel;
    QDoubleSpinBox *durationToSpin;
    QComboBox *durationCombo;
    QLabel *innCompositionLabel;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *dosageForAllInnCheck;
    QCheckBox *aldCheck;
    QWidget *tabSchemes;
    QGridLayout *gridLayout_6;
    QTableWidget *hourlyTableWidget;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer;
    DrugsWidget::Internal::DailySchemeViewer *dailyScheme;
    QGroupBox *groupBox_10;
    QGridLayout *gridLayout_18;
    QHBoxLayout *horizontalLayout;
    QSpinBox *minIntervalIntakesSpin;
    QComboBox *intervalTimeSchemeCombo;
    QGroupBox *groupBox_11;
    QGridLayout *gridLayout_19;
    QComboBox *mealTimeCombo;
    QWidget *tabLimits;
    QGridLayout *gridLayout_12;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_8;
    QComboBox *minAgeCombo;
    QLabel *label_3;
    QComboBox *sexLimitCombo;
    QComboBox *maxAgeCombo;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_5;
    QLabel *label_7;
    QSpinBox *minAgeSpin;
    QSpinBox *maxAgeSpin;
    QSpinBox *minWeightSpin;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_11;
    QSpinBox *minClearanceSpin;
    QLabel *label_8;
    QSpinBox *maxClearanceSpin;
    QLabel *label_9;
    QLabel *label_13;
    QLabel *label_14;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_14;
    QLabel *label_12;
    QWidget *tabLinks;
    QGridLayout *gridLayout_17;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout_15;
    QLabel *label_16;
    QGroupBox *groupBox_9;
    QGridLayout *gridLayout_16;
    QLabel *label_17;
    QWidget *tabMonitoring;
    QGridLayout *gridLayout_10;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_9;
    QLabel *label_6;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_13;
    QLabel *label_10;
    QWidget *tabNote;
    QGridLayout *gridLayout_2;
    QTextEdit *noteTextEdit;
    QWidget *tabTodos;
    QGridLayout *gridLayout_7;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox;
    QLabel *label_4;

    void setupUi(QWidget *DrugsWidget__Internal__DosageViewer)
    {
        if (DrugsWidget__Internal__DosageViewer->objectName().isEmpty())
            DrugsWidget__Internal__DosageViewer->setObjectName(QString::fromUtf8("DrugsWidget__Internal__DosageViewer"));
        DrugsWidget__Internal__DosageViewer->resize(643, 473);
        gridLayout = new QGridLayout(DrugsWidget__Internal__DosageViewer);
        gridLayout->setMargin(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(0);
        gridLayout->setVerticalSpacing(-1);
        line = new QFrame(DrugsWidget__Internal__DosageViewer);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 0, 0, 1, 2);

        labelLayout = new QHBoxLayout();
        labelLayout->setObjectName(QString::fromUtf8("labelLayout"));
        labelOfDosageLabel = new QLabel(DrugsWidget__Internal__DosageViewer);
        labelOfDosageLabel->setObjectName(QString::fromUtf8("labelOfDosageLabel"));

        labelLayout->addWidget(labelOfDosageLabel);

        labelLineEdit = new QLineEdit(DrugsWidget__Internal__DosageViewer);
        labelLineEdit->setObjectName(QString::fromUtf8("labelLineEdit"));

        labelLayout->addWidget(labelLineEdit);

        frenchRCPButton = new QPushButton(DrugsWidget__Internal__DosageViewer);
        frenchRCPButton->setObjectName(QString::fromUtf8("frenchRCPButton"));

        labelLayout->addWidget(frenchRCPButton);


        gridLayout->addLayout(labelLayout, 1, 0, 1, 2);

        tabWidget = new QTabWidget(DrugsWidget__Internal__DosageViewer);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabBasics = new QWidget();
        tabBasics->setObjectName(QString::fromUtf8("tabBasics"));
        gridLayout_5 = new QGridLayout(tabBasics);
        gridLayout_5->setSpacing(5);
        gridLayout_5->setMargin(10);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer, 6, 0, 1, 1);

        groupBox_6 = new QGroupBox(tabBasics);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        gridLayout_3 = new QGridLayout(groupBox_6);
        gridLayout_3->setSpacing(5);
        gridLayout_3->setMargin(10);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        fromToIntakesCheck = new QCheckBox(groupBox_6);
        fromToIntakesCheck->setObjectName(QString::fromUtf8("fromToIntakesCheck"));

        gridLayout_3->addWidget(fromToIntakesCheck, 0, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        intakesFromLabel = new QLabel(groupBox_6);
        intakesFromLabel->setObjectName(QString::fromUtf8("intakesFromLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(intakesFromLabel->sizePolicy().hasHeightForWidth());
        intakesFromLabel->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(intakesFromLabel);

        intakesFromSpin = new QDoubleSpinBox(groupBox_6);
        intakesFromSpin->setObjectName(QString::fromUtf8("intakesFromSpin"));
        sizePolicy.setHeightForWidth(intakesFromSpin->sizePolicy().hasHeightForWidth());
        intakesFromSpin->setSizePolicy(sizePolicy);
        intakesFromSpin->setMinimum(1);

        horizontalLayout_4->addWidget(intakesFromSpin);

        intakesToLabel = new QLabel(groupBox_6);
        intakesToLabel->setObjectName(QString::fromUtf8("intakesToLabel"));
        sizePolicy.setHeightForWidth(intakesToLabel->sizePolicy().hasHeightForWidth());
        intakesToLabel->setSizePolicy(sizePolicy);
        intakesToLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(intakesToLabel);

        intakesToSpin = new QDoubleSpinBox(groupBox_6);
        intakesToSpin->setObjectName(QString::fromUtf8("intakesToSpin"));
        sizePolicy.setHeightForWidth(intakesToSpin->sizePolicy().hasHeightForWidth());
        intakesToSpin->setSizePolicy(sizePolicy);
        intakesToSpin->setMinimum(1);

        horizontalLayout_4->addWidget(intakesToSpin);

        intakesCombo = new QComboBox(groupBox_6);
        intakesCombo->setObjectName(QString::fromUtf8("intakesCombo"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(intakesCombo->sizePolicy().hasHeightForWidth());
        intakesCombo->setSizePolicy(sizePolicy1);
        intakesCombo->setEditable(true);
        intakesCombo->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

        horizontalLayout_4->addWidget(intakesCombo);

        userformsButton = new QPushButton(groupBox_6);
        userformsButton->setObjectName(QString::fromUtf8("userformsButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(userformsButton->sizePolicy().hasHeightForWidth());
        userformsButton->setSizePolicy(sizePolicy2);
        userformsButton->setMinimumSize(QSize(20, 20));
        userformsButton->setMaximumSize(QSize(20, 20));

        horizontalLayout_4->addWidget(userformsButton);


        gridLayout_3->addLayout(horizontalLayout_4, 1, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        intakesFromLabel_3 = new QLabel(groupBox_6);
        intakesFromLabel_3->setObjectName(QString::fromUtf8("intakesFromLabel_3"));
        sizePolicy.setHeightForWidth(intakesFromLabel_3->sizePolicy().hasHeightForWidth());
        intakesFromLabel_3->setSizePolicy(sizePolicy);

        horizontalLayout_5->addWidget(intakesFromLabel_3);

        periodSpin = new QSpinBox(groupBox_6);
        periodSpin->setObjectName(QString::fromUtf8("periodSpin"));
        periodSpin->setMinimum(1);

        horizontalLayout_5->addWidget(periodSpin);

        periodSchemeCombo = new QComboBox(groupBox_6);
        periodSchemeCombo->setObjectName(QString::fromUtf8("periodSchemeCombo"));
        periodSchemeCombo->setEditable(true);

        horizontalLayout_5->addWidget(periodSchemeCombo);


        gridLayout_3->addLayout(horizontalLayout_5, 2, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_6, 0, 0, 1, 1);

        groupBox_7 = new QGroupBox(tabBasics);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        gridLayout_4 = new QGridLayout(groupBox_7);
        gridLayout_4->setSpacing(5);
        gridLayout_4->setMargin(10);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        fromToDurationCheck = new QCheckBox(groupBox_7);
        fromToDurationCheck->setObjectName(QString::fromUtf8("fromToDurationCheck"));

        gridLayout_4->addWidget(fromToDurationCheck, 0, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        durationFromLabel = new QLabel(groupBox_7);
        durationFromLabel->setObjectName(QString::fromUtf8("durationFromLabel"));
        sizePolicy.setHeightForWidth(durationFromLabel->sizePolicy().hasHeightForWidth());
        durationFromLabel->setSizePolicy(sizePolicy);

        horizontalLayout_6->addWidget(durationFromLabel);

        durationFromSpin = new QDoubleSpinBox(groupBox_7);
        durationFromSpin->setObjectName(QString::fromUtf8("durationFromSpin"));
        sizePolicy.setHeightForWidth(durationFromSpin->sizePolicy().hasHeightForWidth());
        durationFromSpin->setSizePolicy(sizePolicy);
        durationFromSpin->setMinimum(1);

        horizontalLayout_6->addWidget(durationFromSpin);

        durationToLabel = new QLabel(groupBox_7);
        durationToLabel->setObjectName(QString::fromUtf8("durationToLabel"));
        sizePolicy.setHeightForWidth(durationToLabel->sizePolicy().hasHeightForWidth());
        durationToLabel->setSizePolicy(sizePolicy);
        durationToLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_6->addWidget(durationToLabel);

        durationToSpin = new QDoubleSpinBox(groupBox_7);
        durationToSpin->setObjectName(QString::fromUtf8("durationToSpin"));
        sizePolicy.setHeightForWidth(durationToSpin->sizePolicy().hasHeightForWidth());
        durationToSpin->setSizePolicy(sizePolicy);
        durationToSpin->setMinimum(1);

        horizontalLayout_6->addWidget(durationToSpin);

        durationCombo = new QComboBox(groupBox_7);
        durationCombo->setObjectName(QString::fromUtf8("durationCombo"));
        sizePolicy1.setHeightForWidth(durationCombo->sizePolicy().hasHeightForWidth());
        durationCombo->setSizePolicy(sizePolicy1);
        durationCombo->setEditable(true);
        durationCombo->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

        horizontalLayout_6->addWidget(durationCombo);


        gridLayout_4->addLayout(horizontalLayout_6, 1, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_7, 1, 0, 1, 1);

        innCompositionLabel = new QLabel(tabBasics);
        innCompositionLabel->setObjectName(QString::fromUtf8("innCompositionLabel"));
        innCompositionLabel->setIndent(20);

        gridLayout_5->addWidget(innCompositionLabel, 5, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        dosageForAllInnCheck = new QCheckBox(tabBasics);
        dosageForAllInnCheck->setObjectName(QString::fromUtf8("dosageForAllInnCheck"));

        horizontalLayout_3->addWidget(dosageForAllInnCheck);

        aldCheck = new QCheckBox(tabBasics);
        aldCheck->setObjectName(QString::fromUtf8("aldCheck"));

        horizontalLayout_3->addWidget(aldCheck);


        gridLayout_5->addLayout(horizontalLayout_3, 2, 0, 1, 1);

        tabWidget->addTab(tabBasics, QString());
        tabSchemes = new QWidget();
        tabSchemes->setObjectName(QString::fromUtf8("tabSchemes"));
        gridLayout_6 = new QGridLayout(tabSchemes);
        gridLayout_6->setSpacing(5);
        gridLayout_6->setMargin(10);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        hourlyTableWidget = new QTableWidget(tabSchemes);
        if (hourlyTableWidget->columnCount() < 8)
            hourlyTableWidget->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        hourlyTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        hourlyTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        hourlyTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        hourlyTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        hourlyTableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        hourlyTableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        hourlyTableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        hourlyTableWidget->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        if (hourlyTableWidget->rowCount() < 3)
            hourlyTableWidget->setRowCount(3);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        hourlyTableWidget->setVerticalHeaderItem(0, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        hourlyTableWidget->setVerticalHeaderItem(1, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        hourlyTableWidget->setVerticalHeaderItem(2, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        hourlyTableWidget->setItem(0, 0, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        hourlyTableWidget->setItem(0, 1, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        hourlyTableWidget->setItem(0, 2, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        hourlyTableWidget->setItem(0, 3, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        hourlyTableWidget->setItem(0, 4, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        hourlyTableWidget->setItem(0, 5, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        hourlyTableWidget->setItem(0, 6, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        hourlyTableWidget->setItem(0, 7, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        hourlyTableWidget->setItem(1, 0, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        hourlyTableWidget->setItem(1, 1, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        hourlyTableWidget->setItem(1, 2, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        hourlyTableWidget->setItem(1, 3, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        hourlyTableWidget->setItem(1, 4, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        hourlyTableWidget->setItem(1, 5, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        hourlyTableWidget->setItem(1, 6, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        hourlyTableWidget->setItem(1, 7, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        hourlyTableWidget->setItem(2, 0, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        hourlyTableWidget->setItem(2, 1, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        hourlyTableWidget->setItem(2, 2, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        hourlyTableWidget->setItem(2, 3, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        hourlyTableWidget->setItem(2, 4, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        hourlyTableWidget->setItem(2, 5, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        hourlyTableWidget->setItem(2, 6, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        hourlyTableWidget->setItem(2, 7, __qtablewidgetitem34);
        hourlyTableWidget->setObjectName(QString::fromUtf8("hourlyTableWidget"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(hourlyTableWidget->sizePolicy().hasHeightForWidth());
        hourlyTableWidget->setSizePolicy(sizePolicy3);
        hourlyTableWidget->setMinimumSize(QSize(0, 94));
        hourlyTableWidget->setMaximumSize(QSize(16777215, 94));
        QFont font;
        font.setPointSize(13);
        hourlyTableWidget->setFont(font);
        hourlyTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        hourlyTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        hourlyTableWidget->setSelectionMode(QAbstractItemView::MultiSelection);

        gridLayout_6->addWidget(hourlyTableWidget, 1, 3, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_6->addItem(verticalSpacer_2, 5, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer, 1, 2, 1, 1);

        dailyScheme = new DrugsWidget::Internal::DailySchemeViewer(tabSchemes);
        dailyScheme->setObjectName(QString::fromUtf8("dailyScheme"));

        gridLayout_6->addWidget(dailyScheme, 1, 0, 5, 2);

        groupBox_10 = new QGroupBox(tabSchemes);
        groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));
        gridLayout_18 = new QGridLayout(groupBox_10);
        gridLayout_18->setObjectName(QString::fromUtf8("gridLayout_18"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        minIntervalIntakesSpin = new QSpinBox(groupBox_10);
        minIntervalIntakesSpin->setObjectName(QString::fromUtf8("minIntervalIntakesSpin"));

        horizontalLayout->addWidget(minIntervalIntakesSpin);

        intervalTimeSchemeCombo = new QComboBox(groupBox_10);
        intervalTimeSchemeCombo->setObjectName(QString::fromUtf8("intervalTimeSchemeCombo"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(intervalTimeSchemeCombo->sizePolicy().hasHeightForWidth());
        intervalTimeSchemeCombo->setSizePolicy(sizePolicy4);
        intervalTimeSchemeCombo->setMinimumSize(QSize(0, 0));
        intervalTimeSchemeCombo->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(intervalTimeSchemeCombo);


        gridLayout_18->addLayout(horizontalLayout, 0, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_10, 2, 3, 1, 1);

        groupBox_11 = new QGroupBox(tabSchemes);
        groupBox_11->setObjectName(QString::fromUtf8("groupBox_11"));
        gridLayout_19 = new QGridLayout(groupBox_11);
        gridLayout_19->setObjectName(QString::fromUtf8("gridLayout_19"));
        mealTimeCombo = new QComboBox(groupBox_11);
        mealTimeCombo->setObjectName(QString::fromUtf8("mealTimeCombo"));

        gridLayout_19->addWidget(mealTimeCombo, 0, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_11, 3, 3, 1, 1);

        tabWidget->addTab(tabSchemes, QString());
        tabLimits = new QWidget();
        tabLimits->setObjectName(QString::fromUtf8("tabLimits"));
        gridLayout_12 = new QGridLayout(tabLimits);
        gridLayout_12->setSpacing(5);
        gridLayout_12->setMargin(10);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        groupBox_2 = new QGroupBox(tabLimits);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_8 = new QGridLayout(groupBox_2);
        gridLayout_8->setSpacing(5);
        gridLayout_8->setMargin(0);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        minAgeCombo = new QComboBox(groupBox_2);
        minAgeCombo->setObjectName(QString::fromUtf8("minAgeCombo"));
        minAgeCombo->setEnabled(true);
        minAgeCombo->setMinimumSize(QSize(0, 22));

        gridLayout_8->addWidget(minAgeCombo, 0, 2, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(0, 22));

        gridLayout_8->addWidget(label_3, 2, 2, 1, 1);

        sexLimitCombo = new QComboBox(groupBox_2);
        sexLimitCombo->setObjectName(QString::fromUtf8("sexLimitCombo"));
        sexLimitCombo->setEnabled(true);
        sexLimitCombo->setMinimumSize(QSize(0, 22));

        gridLayout_8->addWidget(sexLimitCombo, 3, 1, 1, 2);

        maxAgeCombo = new QComboBox(groupBox_2);
        maxAgeCombo->setObjectName(QString::fromUtf8("maxAgeCombo"));
        maxAgeCombo->setEnabled(true);

        gridLayout_8->addWidget(maxAgeCombo, 1, 2, 1, 1);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_8->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_8->addWidget(label_2, 1, 0, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_8->addWidget(label_5, 2, 0, 1, 1);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_8->addWidget(label_7, 3, 0, 1, 1);

        minAgeSpin = new QSpinBox(groupBox_2);
        minAgeSpin->setObjectName(QString::fromUtf8("minAgeSpin"));

        gridLayout_8->addWidget(minAgeSpin, 0, 1, 1, 1);

        maxAgeSpin = new QSpinBox(groupBox_2);
        maxAgeSpin->setObjectName(QString::fromUtf8("maxAgeSpin"));

        gridLayout_8->addWidget(maxAgeSpin, 1, 1, 1, 1);

        minWeightSpin = new QSpinBox(groupBox_2);
        minWeightSpin->setObjectName(QString::fromUtf8("minWeightSpin"));

        gridLayout_8->addWidget(minWeightSpin, 2, 1, 1, 1);


        gridLayout_12->addWidget(groupBox_2, 0, 0, 2, 1);

        groupBox = new QGroupBox(tabLimits);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_11 = new QGridLayout(groupBox);
        gridLayout_11->setSpacing(5);
        gridLayout_11->setMargin(0);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        minClearanceSpin = new QSpinBox(groupBox);
        minClearanceSpin->setObjectName(QString::fromUtf8("minClearanceSpin"));
        minClearanceSpin->setEnabled(true);
        minClearanceSpin->setMinimumSize(QSize(0, 22));
        minClearanceSpin->setMaximum(150);

        gridLayout_11->addWidget(minClearanceSpin, 0, 1, 1, 1);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setMinimumSize(QSize(0, 22));

        gridLayout_11->addWidget(label_8, 0, 2, 1, 1);

        maxClearanceSpin = new QSpinBox(groupBox);
        maxClearanceSpin->setObjectName(QString::fromUtf8("maxClearanceSpin"));
        maxClearanceSpin->setEnabled(true);
        maxClearanceSpin->setMinimumSize(QSize(0, 22));
        maxClearanceSpin->setMaximum(150);

        gridLayout_11->addWidget(maxClearanceSpin, 1, 1, 1, 1);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setMinimumSize(QSize(0, 22));

        gridLayout_11->addWidget(label_9, 1, 2, 1, 1);

        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_11->addWidget(label_13, 0, 0, 1, 1);

        label_14 = new QLabel(groupBox);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_11->addWidget(label_14, 1, 0, 1, 1);


        gridLayout_12->addWidget(groupBox, 0, 1, 1, 1);

        groupBox_5 = new QGroupBox(tabLimits);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_14 = new QGridLayout(groupBox_5);
        gridLayout_14->setObjectName(QString::fromUtf8("gridLayout_14"));
        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_14->addWidget(label_12, 0, 0, 1, 1);


        gridLayout_12->addWidget(groupBox_5, 1, 1, 1, 1);

        tabWidget->addTab(tabLimits, QString());
        tabLinks = new QWidget();
        tabLinks->setObjectName(QString::fromUtf8("tabLinks"));
        gridLayout_17 = new QGridLayout(tabLinks);
        gridLayout_17->setObjectName(QString::fromUtf8("gridLayout_17"));
        groupBox_8 = new QGroupBox(tabLinks);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        groupBox_8->setCheckable(true);
        groupBox_8->setChecked(false);
        gridLayout_15 = new QGridLayout(groupBox_8);
        gridLayout_15->setObjectName(QString::fromUtf8("gridLayout_15"));
        label_16 = new QLabel(groupBox_8);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_15->addWidget(label_16, 0, 0, 1, 1);


        gridLayout_17->addWidget(groupBox_8, 0, 0, 1, 1);

        groupBox_9 = new QGroupBox(tabLinks);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        groupBox_9->setCheckable(true);
        groupBox_9->setChecked(false);
        gridLayout_16 = new QGridLayout(groupBox_9);
        gridLayout_16->setObjectName(QString::fromUtf8("gridLayout_16"));
        label_17 = new QLabel(groupBox_9);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout_16->addWidget(label_17, 0, 0, 1, 1);


        gridLayout_17->addWidget(groupBox_9, 0, 1, 1, 1);

        tabWidget->addTab(tabLinks, QString());
        tabMonitoring = new QWidget();
        tabMonitoring->setObjectName(QString::fromUtf8("tabMonitoring"));
        gridLayout_10 = new QGridLayout(tabMonitoring);
        gridLayout_10->setSpacing(5);
        gridLayout_10->setMargin(10);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        groupBox_3 = new QGroupBox(tabMonitoring);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setCheckable(true);
        groupBox_3->setChecked(false);
        gridLayout_9 = new QGridLayout(groupBox_3);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_9->addWidget(label_6, 0, 0, 1, 1);


        gridLayout_10->addWidget(groupBox_3, 0, 0, 1, 1);

        groupBox_4 = new QGroupBox(tabMonitoring);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setCheckable(true);
        groupBox_4->setChecked(false);
        gridLayout_13 = new QGridLayout(groupBox_4);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_13->addWidget(label_10, 0, 0, 1, 1);


        gridLayout_10->addWidget(groupBox_4, 0, 1, 1, 1);

        tabWidget->addTab(tabMonitoring, QString());
        tabNote = new QWidget();
        tabNote->setObjectName(QString::fromUtf8("tabNote"));
        gridLayout_2 = new QGridLayout(tabNote);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        noteTextEdit = new QTextEdit(tabNote);
        noteTextEdit->setObjectName(QString::fromUtf8("noteTextEdit"));

        gridLayout_2->addWidget(noteTextEdit, 0, 0, 1, 1);

        tabWidget->addTab(tabNote, QString());
        tabTodos = new QWidget();
        tabTodos->setObjectName(QString::fromUtf8("tabTodos"));
        gridLayout_7 = new QGridLayout(tabTodos);
        gridLayout_7->setSpacing(5);
        gridLayout_7->setMargin(10);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        checkBox_2 = new QCheckBox(tabTodos);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        gridLayout_7->addWidget(checkBox_2, 1, 0, 1, 1);

        checkBox = new QCheckBox(tabTodos);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout_7->addWidget(checkBox, 2, 0, 1, 1);

        label_4 = new QLabel(tabTodos);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);

        gridLayout_7->addWidget(label_4, 0, 0, 1, 1);

        tabWidget->addTab(tabTodos, QString());

        gridLayout->addWidget(tabWidget, 2, 0, 1, 1);

        QWidget::setTabOrder(labelLineEdit, fromToIntakesCheck);
        QWidget::setTabOrder(fromToIntakesCheck, intakesFromSpin);
        QWidget::setTabOrder(intakesFromSpin, intakesToSpin);
        QWidget::setTabOrder(intakesToSpin, intakesCombo);
        QWidget::setTabOrder(intakesCombo, userformsButton);
        QWidget::setTabOrder(userformsButton, periodSpin);
        QWidget::setTabOrder(periodSpin, periodSchemeCombo);
        QWidget::setTabOrder(periodSchemeCombo, fromToDurationCheck);
        QWidget::setTabOrder(fromToDurationCheck, durationFromSpin);
        QWidget::setTabOrder(durationFromSpin, durationToSpin);
        QWidget::setTabOrder(durationToSpin, durationCombo);
        QWidget::setTabOrder(durationCombo, intervalTimeSchemeCombo);
        QWidget::setTabOrder(intervalTimeSchemeCombo, minAgeSpin);
        QWidget::setTabOrder(minAgeSpin, minAgeCombo);
        QWidget::setTabOrder(minAgeCombo, maxAgeSpin);
        QWidget::setTabOrder(maxAgeSpin, maxAgeCombo);
        QWidget::setTabOrder(maxAgeCombo, minWeightSpin);
        QWidget::setTabOrder(minWeightSpin, sexLimitCombo);
        QWidget::setTabOrder(sexLimitCombo, minClearanceSpin);
        QWidget::setTabOrder(minClearanceSpin, maxClearanceSpin);
        QWidget::setTabOrder(maxClearanceSpin, groupBox_8);
        QWidget::setTabOrder(groupBox_8, groupBox_9);
        QWidget::setTabOrder(groupBox_9, groupBox_3);
        QWidget::setTabOrder(groupBox_3, groupBox_4);
        QWidget::setTabOrder(groupBox_4, checkBox_2);
        QWidget::setTabOrder(checkBox_2, checkBox);
        QWidget::setTabOrder(checkBox, noteTextEdit);

        retranslateUi(DrugsWidget__Internal__DosageViewer);
        QObject::connect(fromToIntakesCheck, SIGNAL(clicked(bool)), intakesToSpin, SLOT(setVisible(bool)));
        QObject::connect(fromToDurationCheck, SIGNAL(clicked(bool)), durationToSpin, SLOT(setVisible(bool)));
        QObject::connect(fromToDurationCheck, SIGNAL(clicked(bool)), durationToLabel, SLOT(setVisible(bool)));
        QObject::connect(fromToIntakesCheck, SIGNAL(clicked(bool)), intakesToLabel, SLOT(setVisible(bool)));
        QObject::connect(dosageForAllInnCheck, SIGNAL(clicked(bool)), innCompositionLabel, SLOT(setVisible(bool)));

        tabWidget->setCurrentIndex(6);
        minAgeCombo->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(DrugsWidget__Internal__DosageViewer);
    } // setupUi

    void retranslateUi(QWidget *DrugsWidget__Internal__DosageViewer)
    {
        DrugsWidget__Internal__DosageViewer->setWindowTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Dialog", 0, QApplication::UnicodeUTF8));
        labelOfDosageLabel->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Label", 0, QApplication::UnicodeUTF8));
        frenchRCPButton->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "French RCP", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Intakes", 0, QApplication::UnicodeUTF8));
        fromToIntakesCheck->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "From x To y", 0, QApplication::UnicodeUTF8));
        intakesFromLabel->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "From", 0, QApplication::UnicodeUTF8));
        intakesToLabel->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "To", 0, QApplication::UnicodeUTF8));
        userformsButton->setText(QString());
        intakesFromLabel_3->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Each", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Duration", 0, QApplication::UnicodeUTF8));
        fromToDurationCheck->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "From x To y", 0, QApplication::UnicodeUTF8));
        durationFromLabel->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "From", 0, QApplication::UnicodeUTF8));
        durationToLabel->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "To", 0, QApplication::UnicodeUTF8));
        innCompositionLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        dosageForAllInnCheck->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        dosageForAllInnCheck->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Used for INN prescription", 0, QApplication::UnicodeUTF8));
        aldCheck->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "100%", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabBasics), QApplication::translate("DrugsWidget::Internal::DosageViewer", "Basics", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = hourlyTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "0", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = hourlyTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "1", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = hourlyTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = hourlyTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "3", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = hourlyTableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "4", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = hourlyTableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "5", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = hourlyTableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "6", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = hourlyTableWidget->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "7", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = hourlyTableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem8->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "1", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = hourlyTableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem9->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = hourlyTableWidget->verticalHeaderItem(2);
        ___qtablewidgetitem10->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "3", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = hourlyTableWidget->isSortingEnabled();
        hourlyTableWidget->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem11 = hourlyTableWidget->item(0, 0);
        ___qtablewidgetitem11->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "00", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = hourlyTableWidget->item(0, 1);
        ___qtablewidgetitem12->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "01", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = hourlyTableWidget->item(0, 2);
        ___qtablewidgetitem13->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "02", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = hourlyTableWidget->item(0, 3);
        ___qtablewidgetitem14->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "03", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = hourlyTableWidget->item(0, 4);
        ___qtablewidgetitem15->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "04", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = hourlyTableWidget->item(0, 5);
        ___qtablewidgetitem16->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "05", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = hourlyTableWidget->item(0, 6);
        ___qtablewidgetitem17->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "06", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = hourlyTableWidget->item(0, 7);
        ___qtablewidgetitem18->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "07", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem19 = hourlyTableWidget->item(1, 0);
        ___qtablewidgetitem19->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "08", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem20 = hourlyTableWidget->item(1, 1);
        ___qtablewidgetitem20->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "09", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem21 = hourlyTableWidget->item(1, 2);
        ___qtablewidgetitem21->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "10", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem22 = hourlyTableWidget->item(1, 3);
        ___qtablewidgetitem22->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "11", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem23 = hourlyTableWidget->item(1, 4);
        ___qtablewidgetitem23->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "12", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem24 = hourlyTableWidget->item(1, 5);
        ___qtablewidgetitem24->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "13", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem25 = hourlyTableWidget->item(1, 6);
        ___qtablewidgetitem25->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "14", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem26 = hourlyTableWidget->item(1, 7);
        ___qtablewidgetitem26->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "15", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem27 = hourlyTableWidget->item(2, 0);
        ___qtablewidgetitem27->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "16", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem28 = hourlyTableWidget->item(2, 1);
        ___qtablewidgetitem28->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "17", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem29 = hourlyTableWidget->item(2, 2);
        ___qtablewidgetitem29->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "18", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem30 = hourlyTableWidget->item(2, 3);
        ___qtablewidgetitem30->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "19", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem31 = hourlyTableWidget->item(2, 4);
        ___qtablewidgetitem31->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "20", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem32 = hourlyTableWidget->item(2, 5);
        ___qtablewidgetitem32->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "21", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem33 = hourlyTableWidget->item(2, 6);
        ___qtablewidgetitem33->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "22", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem34 = hourlyTableWidget->item(2, 7);
        ___qtablewidgetitem34->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "23", 0, QApplication::UnicodeUTF8));
        hourlyTableWidget->setSortingEnabled(__sortingEnabled);

        groupBox_10->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Min interval between intakes", 0, QApplication::UnicodeUTF8));
        groupBox_11->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Meal time relation", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabSchemes), QApplication::translate("DrugsWidget::Internal::DosageViewer", "Schemes", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Weight / Age / Sex Limited", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Kg", 0, QApplication::UnicodeUTF8));
        sexLimitCombo->clear();
        sexLimitCombo->insertItems(0, QStringList()
         << QApplication::translate("DrugsWidget::Internal::DosageViewer", "No Sex limitation", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DrugsWidget::Internal::DosageViewer", "Man only", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DrugsWidget::Internal::DosageViewer", "Woman only", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Min age", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Max age", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Min weight", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Sex limited", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Renal function limits", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "ml/min", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "ml/min", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Min clearance", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Max clearance", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Pregnancy / breast feeding", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Work in progress", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabLimits), QApplication::translate("DrugsWidget::Internal::DosageViewer", "Limits", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Links to CIM10 diagnosis", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Work in progress", 0, QApplication::UnicodeUTF8));
        groupBox_9->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Links to eDRC diagnosis", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Work in progress", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabLinks), QApplication::translate("DrugsWidget::Internal::DosageViewer", "Links", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "CIM10 Limitations", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Work in progress", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Biological monitoring", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Work in progress", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabMonitoring), QApplication::translate("DrugsWidget::Internal::DosageViewer", "Monitoring", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabNote), QApplication::translate("DrugsWidget::Internal::DosageViewer", "Note", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Don't specify the form", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "INN prescription", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DrugsWidget::Internal::DosageViewer", "Alternate with", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabTodos), QApplication::translate("DrugsWidget::Internal::DosageViewer", "Todos", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DrugsWidget__Internal__DosageViewer);
    } // retranslateUi

};

} // namespace Internal
} // namespace DrugsWidget

namespace DrugsWidget {
namespace Internal {
namespace Ui {
    class DosageViewer: public Ui_DosageViewer {};
} // namespace Ui
} // namespace Internal
} // namespace DrugsWidget

#endif // UI_MFDOSAGEVIEWER_H

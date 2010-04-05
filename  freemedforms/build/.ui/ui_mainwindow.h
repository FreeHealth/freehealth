/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Mon Feb 8 21:16:21 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDateEdit>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <drugsplugin/drugswidget/mfDrugsCentralWidget.h>

namespace MainWin {
namespace Internal {

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLineEdit *patientName;
    QGroupBox *patientInformations;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *dobLabel;
    QDateEdit *dobDateEdit;
    QLabel *label_2;
    QSpinBox *patientWeight;
    QLabel *label_3;
    QSpinBox *patientSize;
    QLabel *label_4;
    QComboBox *sexCombo;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_5;
    QSpinBox *patientClCr;
    QLabel *label_6;
    QDoubleSpinBox *patientCreatinin;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_7;
    QLineEdit *listOfAllergies;
    DrugsWidget::DrugsCentralWidget *m_CentralWidget;
    QPushButton *morePatientInfoButton;

    void setupUi(QMainWindow *MainWin__Internal__MainWindow)
    {
        if (MainWin__Internal__MainWindow->objectName().isEmpty())
            MainWin__Internal__MainWindow->setObjectName(QString::fromUtf8("MainWin__Internal__MainWindow"));
        MainWin__Internal__MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWin__Internal__MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setSpacing(10);
        gridLayout_2->setMargin(10);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        patientName = new QLineEdit(centralwidget);
        patientName->setObjectName(QString::fromUtf8("patientName"));

        gridLayout_2->addWidget(patientName, 0, 1, 1, 1);

        patientInformations = new QGroupBox(centralwidget);
        patientInformations->setObjectName(QString::fromUtf8("patientInformations"));
        gridLayout = new QGridLayout(patientInformations);
        gridLayout->setSpacing(10);
        gridLayout->setMargin(10);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        dobLabel = new QLabel(patientInformations);
        dobLabel->setObjectName(QString::fromUtf8("dobLabel"));

        horizontalLayout->addWidget(dobLabel);

        dobDateEdit = new QDateEdit(patientInformations);
        dobDateEdit->setObjectName(QString::fromUtf8("dobDateEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dobDateEdit->sizePolicy().hasHeightForWidth());
        dobDateEdit->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(dobDateEdit);

        label_2 = new QLabel(patientInformations);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        patientWeight = new QSpinBox(patientInformations);
        patientWeight->setObjectName(QString::fromUtf8("patientWeight"));
        patientWeight->setMinimum(0);
        patientWeight->setMaximum(300);
        patientWeight->setValue(0);

        horizontalLayout->addWidget(patientWeight);

        label_3 = new QLabel(patientInformations);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout->addWidget(label_3);

        patientSize = new QSpinBox(patientInformations);
        patientSize->setObjectName(QString::fromUtf8("patientSize"));
        patientSize->setMinimum(0);
        patientSize->setMaximum(300);
        patientSize->setValue(0);

        horizontalLayout->addWidget(patientSize);

        label_4 = new QLabel(patientInformations);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        sexCombo = new QComboBox(patientInformations);
        sexCombo->setObjectName(QString::fromUtf8("sexCombo"));

        horizontalLayout->addWidget(sexCombo);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_5 = new QLabel(patientInformations);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_2->addWidget(label_5);

        patientClCr = new QSpinBox(patientInformations);
        patientClCr->setObjectName(QString::fromUtf8("patientClCr"));
        patientClCr->setMinimum(0);
        patientClCr->setMaximum(170);
        patientClCr->setValue(0);

        horizontalLayout_2->addWidget(patientClCr);

        label_6 = new QLabel(patientInformations);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_2->addWidget(label_6);

        patientCreatinin = new QDoubleSpinBox(patientInformations);
        patientCreatinin->setObjectName(QString::fromUtf8("patientCreatinin"));
        patientCreatinin->setMinimum(0);
        patientCreatinin->setMaximum(200);
        patientCreatinin->setValue(0);

        horizontalLayout_2->addWidget(patientCreatinin);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_7 = new QLabel(patientInformations);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_3->addWidget(label_7);

        listOfAllergies = new QLineEdit(patientInformations);
        listOfAllergies->setObjectName(QString::fromUtf8("listOfAllergies"));

        horizontalLayout_3->addWidget(listOfAllergies);


        gridLayout->addLayout(horizontalLayout_3, 2, 0, 1, 1);


        gridLayout_2->addWidget(patientInformations, 1, 0, 1, 4);

        m_CentralWidget = new DrugsWidget::DrugsCentralWidget(centralwidget);
        m_CentralWidget->setObjectName(QString::fromUtf8("m_CentralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(m_CentralWidget->sizePolicy().hasHeightForWidth());
        m_CentralWidget->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(m_CentralWidget, 2, 0, 1, 4);

        morePatientInfoButton = new QPushButton(centralwidget);
        morePatientInfoButton->setObjectName(QString::fromUtf8("morePatientInfoButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(morePatientInfoButton->sizePolicy().hasHeightForWidth());
        morePatientInfoButton->setSizePolicy(sizePolicy2);
        morePatientInfoButton->setMinimumSize(QSize(22, 22));
        morePatientInfoButton->setMaximumSize(QSize(22, 22));
        morePatientInfoButton->setCheckable(true);

        gridLayout_2->addWidget(morePatientInfoButton, 0, 2, 1, 1);

        MainWin__Internal__MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWin__Internal__MainWindow);
        QObject::connect(morePatientInfoButton, SIGNAL(toggled(bool)), patientInformations, SLOT(setVisible(bool)));

        QMetaObject::connectSlotsByName(MainWin__Internal__MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWin__Internal__MainWindow)
    {
        label->setText(QApplication::translate("MainWin::Internal::MainWindow", "Patient Name", 0, QApplication::UnicodeUTF8));
        patientInformations->setTitle(QApplication::translate("MainWin::Internal::MainWindow", "Patient informations", 0, QApplication::UnicodeUTF8));
        dobLabel->setText(QApplication::translate("MainWin::Internal::MainWindow", "DOB", 0, QApplication::UnicodeUTF8));
        dobDateEdit->setDisplayFormat(QApplication::translate("MainWin::Internal::MainWindow", "dd/MM/yyyy", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWin::Internal::MainWindow", "Weight (kg)", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWin::Internal::MainWindow", "Size (cm)", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWin::Internal::MainWindow", "Gender", 0, QApplication::UnicodeUTF8));
        sexCombo->clear();
        sexCombo->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("MainWin::Internal::MainWindow", "M", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWin::Internal::MainWindow", "F", 0, QApplication::UnicodeUTF8)
        );
        label_5->setText(QApplication::translate("MainWin::Internal::MainWindow", "Creatinin clearance (ml/min)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWin::Internal::MainWindow", "Creatinin (mg/l)", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWin::Internal::MainWindow", "List of medications allergies", 0, QApplication::UnicodeUTF8));
        morePatientInfoButton->setText(QString());
        Q_UNUSED(MainWin__Internal__MainWindow);
    } // retranslateUi

};

} // namespace Internal
} // namespace MainWin

namespace MainWin {
namespace Internal {
namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui
} // namespace Internal
} // namespace MainWin

#endif // UI_MAINWINDOW_H

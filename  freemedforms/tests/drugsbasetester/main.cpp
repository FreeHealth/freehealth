#include "IamTester.h"
#include "DosageSenderTester.h"

#include <QApplication>
#include <QVariant>
#include <QDebug>

#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugInteraction.h>

#include <tkGlobal.h>
#include <tkSettings.h>
#include <tkSendMessage.h>

QDrugsList drugList;

struct DrugTest {
    DrugTest( QString n, int _CIS, QString mi) : name(n), CIS(_CIS), mainInn(mi) {}
    QString name;
    int CIS;
    QString mainInn;
};


QList<DrugTest> drugTestList;


bool testNameCISMainInn(const mfDrugs *drug, const DrugTest &test)
{
    if (drug->CIS()!=test.CIS)
        qWarning() << "ERROR : CIS" << test.CIS << drug->CIS() << test.name << drug->denomination();
    if (drug->denomination().left(drug->denomination().indexOf(","))!=test.name)
        qWarning() << "ERROR : NAME" << drug->denomination().left(drug->denomination().indexOf(",")) << test.name;
    if (drug->mainInnName()!=test.mainInn)
        qWarning() << "ERROR : mainInn" << drug->mainInnName() << test.mainInn;
    return true;
}

void addDrugs()
{
    mfDrugsModel *m = mfDrugsModel::instance();
    // Add drugs
    qWarning() << "*********** adding drugs";
    int i = 0;
    foreach( const DrugTest &test, drugTestList)  {
        if (m->addDrug(test.CIS, true) != -1) {
            qWarning() << i;
            mfDrugs *drug = m->drugsList().at(i);
            // Test Drug
            if (drug)
                testNameCISMainInn(drug, test);
            ++i;
        } else
            qWarning() << "error : unknown CIS" << test.CIS;
    }
}

void testDrugsModel()
{
    mfDrugsModel *m = mfDrugsModel::instance();
    mfDrugsBase *b = mfDrugsBase::instance();

    qWarning() << "*********** warning drugs";
    // Warn drugs
    QStringList list;
    int i = 0;
//    foreach( mfDrugs *drug, mfDrugsModel::instance()->drugsList()) {
//        QString tmp;
//        tmp += drug->denomination().leftJustified(60,' ');
//        tmp += "\t";
//        tmp += "Iam=";
//        tmp += mfDrugInteraction::typeToString(m->index( i, Drug::MaximumLevelOfInteraction ).data().toInt());
//        tmp += drug->warnText();
//        list << tmp;
//        ++i;
//    }
//    tkGlobal::quickDebugDialog(list);

    list.clear();
    foreach( mfDrugs *drug, mfDrugsModel::instance()->drugsList()) {
        QString tmp;
        tmp += "CIS / Name : " + QString::number(drug->CIS()) + " " + drug->denomination() +"\n";
        tmp += "All Inn : ";
        foreach(int i, drug->allInnAndIamClasses())
            tmp += QString::number(i) + " ; ";
        tmp += "\n";
        tmp += "Dosage of molecule : " + drug->dosageOfMolecules().join("; ") + "\n";
        tmp += "Code Molecules : ";
        foreach(const QVariant &q, drug->listOfCodeMolecules().toList())
            tmp += q.toString() + " ; ";
        tmp += "\n";
        tmp += "Form : " + drug->form() + "\n";
        tmp += "Main inn name : " + drug->mainInnName() + "\n";
        tmp += "Main inn code : " + QString::number(drug->mainInnCode()) + "\n";
        tmp += "Main inn dosage : " + drug->mainInnDosage() + "\n";
        list << tmp;
    }
    tkGlobal::quickDebugDialog(list);
}


void testSendDosage()
{
    DosageSenderTester test(qApp);
    qApp->connect(&test, SIGNAL(end()), qApp, SLOT(quit()));
    qWarning() << "test.start";
    test.start();
}

void testIam()
{
    IamTester it;
    it.testIam();
}



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("drugsbasetester");
    qWarning() << app.applicationName();

    qWarning() << "*********** creating tkSettings";
    tkSettings::instance(qApp);
    qWarning();
    qWarning() << "*********** creating drugsbase instance";
    qWarning() << mfDrugsBase::instance();
    qWarning();
//    drugTestList << DrugTest("CLAMOXYL 1 g"   , 64539416, "amoxicilline");
//    drugTestList << DrugTest("AMOXICILLINE EG LABO 1 g" , AMOX, "amoxicilline");
//    drugTestList << DrugTest("ABUFENE 400 mg" , ABUFENE, "");
//    drugTestList << DrugTest("CADUET 5 mg/10 mg"      , CADUET, "");
//    drugTestList << DrugTest("LANGORAN L.P. 40 mg"    , 61355763, "isosorbide");
//    drugTestList << DrugTest("5-MONONITRATE D'ISOSORBIDE ETHYPHARM LP 40 mg" , 68166740, "isosorbide");

    mfDrugsBase *base = mfDrugsBase::instance();

//    addDrugs();
//    testDrugsModel();
    testIam();

    return app.exec();
}

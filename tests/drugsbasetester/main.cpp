#include <QApplication>
#include <QVariant>
#include <QDebug>

#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugInteraction.h>

#include <tkGlobal.h>
#include <tkSettings.h>

QDrugsList drugList;

enum {
    ACIDE_FUSIDIQUE = 65070886,
    AMOX = 64882258,
    APRANAX = 66152304,
    ATAZANAVIR = 68301348,
    ATORVASTATINE = 69902323,
    BACTRIM = 65181349,

    CADUET = 63123849,
    CARDENSIEL = 60077574,
    CICLOSPORINE = 64938212,
    CYANOCOBALAMINE = 61247741,
    CLARITHROMYCINE = 65576684,
    CLOPIDOGREL = 63564053,
    COLCHICINE = 61331730,
    CORDARONE = 64408662,
    COUMADINE = 63245753,
    DEROXAT = 60160707,
    PAROXETINE = 60160707,


    EUPANTHOL = 60071952,
    ERLOTINIB = 63390528,

    IBUPROFENE = 61478851,
    ISOSORBIDE = 68166740,
    KARDEGIC = 61348441,

    LANZOR = 68143802,
    LASILIX = 65606743,
    FUROSEMIDE = 65606743,
    LOVENOX = 67010731,

    METHOTREXATE = 60695745,
    NUREFLEX = 68475904,
    OMEPRAZOLE = 61337968,

    PARACETAMOL = 61158602,
    PREVISCAN = 68133651,
    PROZAC = 60935279,
    SIMVA = 60583269,

    TRAMADOL = 61644643,
    TRIATEC = 60161143,
    TRIFLUCAN = 61173013,

    SEROPRAM = 69388669,
    VOLTARENE = 62431527,
    INEXIUM = 62824142,
    PARIET = 62909137,
    NEXEN = 60379569,
    TICLODIPINE = 67763864,
    ALIMTA = 67323808,
    CLASTOBAN = 61453218,
    ESTRACYT = 60675618,

ITRACONAZOLE = 66188581,
KETOCONAZOLE = 66227874,
MILLEPERTUIS = 65167997,
POSACONAZOLE = 62076503,
DILTIAZEM = 61348256,
GEMFIBROZIL = 68696386,
LIPANTHYL = 69612398,
STIRIPENTOL = 61501691,
TELITHROMYCINE = 69470123,
VERAPAMIL = 62607039,
BEPRIDIL = 67925490,
DALFOPRISTINE = 64203312,
DANTROLENE = 67807645,
EFAVIRENZ = 64702347,
ERYTHROMYCINE = 66448374,
ESMOLOL = 61177887,
QUINUPRISTINE = 64203312,
CARBAMAZEPINE = 61547468,
ACETAZOLAMIDE = 68693244,
CIMETIDINE = 64073139,
CLONAZEPAM = 68429141,
CLOZAPINE = 67045967,
DANAZOL = 64906556,
DEXTROPROPOXYPHENE = 69223112 ,
DIGOXINE = 67681303,
ETHOSUXIMIDE = 68481191,
FELBAMATE = 60223278,
FLUOXETINE = 63730233,
FLUVOXAMINE = 65470567,
HALOPERIDOL = 61697400,
ISONIAZIDE = 63898512,
JOSAMYCINE = 66113600,
LAMOTRIGINE = 68403716,
LITHIUM = 61197128,
OLANZAPINE = 67974498,
PHENOBARBITAL = 63707416,
PHENYTOINE = 62436200,
RIFAMPICINE = 67876901,
RISPERIDONE = 69505147,
SERTRALINE = 60620835,
TOPIRAMATE = 69104026,
ACIDE_VALPROIQUE = 68762676,
FLUCONAZOLE = 69154659,
ACEBUTOLOL = 63880820,
ATENOLOL =64900101,
BEFUNOLOL = 00000000,
BETAXOLOL = 67723745,
BISOPROLOL = 61500529,
CARTEOLOL = 65144653,
CELIPROLOL = 64832910,
LABETALOL = 60558554,
LEVOBUNOLOL = 67515370,
METIPRANOLOL = 67684299,
METOPROLOL = 66459997,
NADOLOL = 62092895,
NADOXOLOL = 0000000,
NEBIVOLOL = 62794286,
OXPRENOLOL = 67486036,
PINDOLOL = 66057308,
PROPRANOLOL = 66415335,
SOTALOL = 66417507,
TERTATOLOL = 69859092,
TIMOLOL = 63212842,
CARVEDILOL = 66334926,

NNN = 0
};


struct DrugTest {
    DrugTest( QString n, int _CIS, QString mi) : name(n), CIS(_CIS), mainInn(mi) {}
    QString name;
    int CIS;
    QString mainInn;
};

struct IamTest {
    IamTest(const int CIS1, const int CIS2)
            : m_CIS1(CIS1), m_CIS2(CIS2) {}
    int m_CIS1;
    int m_CIS2;
};

QList<DrugTest> drugTestList;
QList<IamTest> iamTestList;


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

void testIam()
{
    qWarning() << "*********** testing Interactions";

    // AINS Interactions p2-3-4
    QSet<int> list;
    list << NEXEN << KARDEGIC << IBUPROFENE << APRANAX << NUREFLEX << NEXEN << VOLTARENE;
    foreach(int i, list) {
        iamTestList << IamTest( i, CLOPIDOGREL );
        iamTestList << IamTest( i, PREVISCAN );
        iamTestList << IamTest( i, LASILIX );
        iamTestList << IamTest( i, TRIATEC );
        iamTestList << IamTest( i, LOVENOX );
        iamTestList << IamTest( i, DEROXAT );
        iamTestList << IamTest( i, SEROPRAM );
        iamTestList << IamTest( i, PROZAC );
        iamTestList << IamTest( i, IBUPROFENE );
        iamTestList << IamTest( i, METHOTREXATE );
        iamTestList << IamTest( i, TICLODIPINE );
        iamTestList << IamTest( i, ALIMTA );
    }

    // AVK
    iamTestList << IamTest( TRIFLUCAN, PREVISCAN );
    iamTestList << IamTest( IBUPROFENE, PREVISCAN );
    iamTestList << IamTest( APRANAX, PREVISCAN );

    // p4
    iamTestList << IamTest(CLASTOBAN, ESTRACYT);

    // IPP  p29
    list.clear();
    list << OMEPRAZOLE << INEXIUM << PARIET << LANZOR << EUPANTHOL;
    foreach(int i, list) {
        iamTestList << IamTest( i, ATAZANAVIR );
        iamTestList << IamTest( i, CLOPIDOGREL);
        iamTestList << IamTest( i, CYANOCOBALAMINE);
        iamTestList << IamTest( i, ERLOTINIB);
        iamTestList << IamTest( i, ITRACONAZOLE);
        iamTestList << IamTest( i, KETOCONAZOLE );
        iamTestList << IamTest( i, MILLEPERTUIS);
        iamTestList << IamTest( i, POSACONAZOLE);
    }

    // ATORVASTATINE  p31
    list.clear();
    list << ACIDE_FUSIDIQUE << CICLOSPORINE << PREVISCAN << COUMADINE << CLARITHROMYCINE << COLCHICINE
         << DILTIAZEM << ERYTHROMYCINE << GEMFIBROZIL << LIPANTHYL << ITRACONAZOLE << KETOCONAZOLE << POSACONAZOLE
         << STIRIPENTOL << TELITHROMYCINE << VERAPAMIL;
    foreach(int i, list) {
        iamTestList << IamTest( i, ATORVASTATINE );
    }

    // BEPRIDIL p34  ajouter bétâbloquants
    list.clear();
    list << CLARITHROMYCINE << DALFOPRISTINE <<  DANTROLENE << EFAVIRENZ << ERYTHROMYCINE  << ESMOLOL 
         << ITRACONAZOLE << KETOCONAZOLE << QUINUPRISTINE << STIRIPENTOL;
    foreach(int i, list) {
        iamTestList << IamTest( i, BEPRIDIL );
    }

    // CARBAMAZEPINE p42
    list.clear();
    list <<  ACETAZOLAMIDE << CIMETIDINE << CLARITHROMYCINE << CLONAZEPAM << CLOZAPINE << DANAZOL << DEXTROPROPOXYPHENE << DIGOXINE
         << ERYTHROMYCINE << FUROSEMIDE << ETHOSUXIMIDE<< FELBAMATE << FLUCONAZOLE << FLUOXETINE << FLUVOXAMINE
         << HALOPERIDOL << ISONIAZIDE << JOSAMYCINE << LAMOTRIGINE << LITHIUM << MILLEPERTUIS << OLANZAPINE
         << PAROXETINE << PHENOBARBITAL << PHENYTOINE << RIFAMPICINE<< RISPERIDONE<< SERTRALINE
         << TOPIRAMATE << TRAMADOL << ACIDE_VALPROIQUE;
    foreach(int i, list) {
        iamTestList << IamTest( i, CARBAMAZEPINE );
    }

    // BB dans insuf cardiaque
//    QSet<int> BBInsuf;
//    BBInsuf << CARVEDILOL << BISOPROLOL << METOPROLOL;
//    foreach(int i, BBInsuf) {
//        iamTestList << IamTest( i,  );
//    }
//        AMIODARONE
//        ANESTHÉSIQUES VOLATILS HALOGÉNÉS
//        ANTICHOLINESTÉRASIQUES
//        ANTIARYTHMIQUES CLASSE I (SAUF LIDOCAÏNE)
//        ANTIDÉPRESSEURS IMIPRAMINIQUES
//        ANTIHYPERTENSEURS CENTRAUX
//        BEPRIDIL
//        DIGITALIQUES
//        DIHYDROPYRIDINES
//        DILTIAZEM
//        FLOCTAFENINE
//        LIDOCAINE
//        MÉDICAMENTS SUSCEPTIBLES DE DONNER DES TORSADES DE POINTES
//        NEUROLEPTIQUES
//        SULFAMIDES HYPOGLYCÉMIANTS
//        VERAPAMIL
//
//    QSet<int> BB;
//    BB << ACEBUTOLOL << ATENOLOL << BEFUNOLOL << BETAXOLOL << BISOPROLOL << CARTEOLOL << CELIPROLOL << LABETALOL << LEVOBUNOLOL
//        << METIPRANOLOL << METOPROLOL << NADOLOL << NADOXOLOL << NEBIVOLOL << OXPRENOLOL << PINDOLOL << PROPRANOLOL
//        << SOTALOL << TERTATOLOL << TIMOLOL;


    iamTestList << IamTest(ACIDE_FUSIDIQUE, SIMVA);




    mfDrugsModel *m = mfDrugsModel::instance();
    int tested = 0;
    int founded = 0;
    foreach(const IamTest &iam, iamTestList) {
        m->clearDrugsList();
        if (m->addDrug(iam.m_CIS1)==-1) {
            qWarning() << "ERROR CIS Unknown" << iam.m_CIS1;
            continue;
        }
        if (m->addDrug(iam.m_CIS2)==-1) {
            qWarning() << "ERROR CIS Unknown" << iam.m_CIS2;
            continue;
        }
        mfDrugs *d1 = m->drugsList().at(0);
        mfDrugs *d2 = m->drugsList().at(1);
        ++tested;
        if (!m->index(0, Drug::Interacts).data().toBool()) {
            qWarning() << "ERROR : interaction" << tested << "not found" << d1->denomination() << d2->denomination();
        } else ++founded;
    }

    qWarning() << "Tested IAM" << tested << "Founded IAM" << founded;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qWarning() << "*********** creating tkSettings";
    new tkSettings(qApp);
    qWarning();
    qWarning() << "*********** creating drugsbase instance";
    qWarning() << mfDrugsBase::instance();
    qWarning();
    drugTestList << DrugTest("CLAMOXYL 1 g"   , 64539416, "amoxicilline");
    drugTestList << DrugTest("AMOXICILLINE EG LABO 1 g" , AMOX, "amoxicilline");
//    drugTestList << DrugTest("ABUFENE 400 mg" , ABUFENE, "");
    drugTestList << DrugTest("CADUET 5 mg/10 mg"      , CADUET, "");
    drugTestList << DrugTest("LANGORAN L.P. 40 mg"    , 61355763, "isosorbide");
    drugTestList << DrugTest("5-MONONITRATE D'ISOSORBIDE ETHYPHARM LP 40 mg" , 68166740, "isosorbide");

    mfDrugsBase *base = mfDrugsBase::instance();

    addDrugs();
    testDrugsModel();

    testIam();

    return 0;//app.exec();
}

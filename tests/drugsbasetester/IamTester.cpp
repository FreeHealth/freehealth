#include "IamTester.h"

#include <drugsmodel/mfDrugsModel.h>

#include <QList>
#include <QVariant>
#include <QSet>
#include <QDebug>


IamTester::IamTester()
{
}

void IamTester::testIam()
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

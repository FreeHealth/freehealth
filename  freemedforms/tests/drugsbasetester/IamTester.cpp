#include "IamTester.h"

#include <drugsmodel/mfDrugsModel.h>

#include <QList>
#include <QVariant>
#include <QSet>
#include <QDebug>
#include <QProgressDialog>


IamTester::IamTester()
{
}

void IamTester::testIam()
{
    qWarning() << "*********** testing Interactions";

    // Classes
    QSet<int> ains;
    ains << NEXEN << KARDEGIC << IBUPROFENE << APRANAX << NUREFLEX << NEXEN << VOLTARENE;
    QSet<int> ipp;
    ipp << OMEPRAZOLE << INEXIUM << PARIET << LANZOR << EUPANTHOL;
    QSet<int> aminosides;
    aminosides << AMIKACINE << DIBEKACINE << GENTAMICINE << NETILMICINE << STREPTOMYCINE << TOBRAMYCINE;
    QSet<int> anticholinesterasiques;
    anticholinesterasiques << AMBENONIUM << DONEPEZIL << GALANTAMINE << NEOSTIGMINE << PYRIDOSTIGMINE << RIVASTIGMINE ;
    QSet<int> betaBloqInsuf;
    betaBloqInsuf << CARVEDILOL << BISOPROLOL << METOPROLOL;
    QSet<int> betaBloq;
    betaBloq << ACEBUTOLOL << ATENOLOL << BEFUNOLOL << BETAXOLOL << BISOPROLOL << CARTEOLOL << CELIPROLOL << LABETALOL << LEVOBUNOLOL
            << METIPRANOLOL << METOPROLOL << NADOLOL << NADOXOLOL << NEBIVOLOL << OXPRENOLOL << PINDOLOL << PROPRANOLOL
            << SOTALOL << TERTATOLOL << TIMOLOL;

    QSet<int> antihtaCentraux;
    antihtaCentraux << CLONIDINE << METHYLDOPA << MOXONIDINE << RILMENIDINE;
    QSet<int> dihydropyridines;
    dihydropyridines << AMLODIPINE<<BARNIDIPINE<<FELODIPINE<<ISRADIPINE<<LACIDIPINE<<LERCANIDIPINE<<MANIDIPINE<<NICARDIPINE<<NIFEDIPINE<<NIMODIPINE<<NITRENDIPINE;
    QSet<int> neuroleptiques;
    neuroleptiques << ACEPROMAZINE<<ACEPROMETAZINE<<ALIMEMAZINE<<ALIZAPRIDE<<AMISULPRIDE<<ARIPIPRAZOLE<<CARPIPRAMINE<<CHLORPROETHAZINE<<CHLORPROMAZINE<<CLOZAPINE<<CYAMEMAZINE<<DROPERIDOL<<FLUANISONE<<FLUPENTIXOL<<FLUPHENAZINE<<HALOPERIDOL<<LEVOMEPROMAZINE<<LOXAPINE<<METOCLOPRAMIDE<< METOPIMAZINE<<OLANZAPINE<<OXOMEMAZINE<<PENFLURIDOL<<PERPHENAZINE<<PIMOZIDE<<PIPAMPERONE<<PIPERAZINE<<PIPOTIAZINE<<PROCHLORPERAZINE<<PROMETHAZINE<<PROPERICIAZINE<< RISPERIDONE<<SERTINDOLE<<SULPIRIDE<<SULTOPRIDE<<THIOPROPERAZINE<<TIAPRIDE<<TRIFLUOPERAZINE<<TRIFLUPERIDOL<<ZUCLOPENTHIXOL;

    // TODO --> TESTER HYPONATREMIANTS <-> AUTRES HYPONATREMIANT et ainsi de suite

    // AINS Interactions p2-3-4
    foreach(int i, ains) {
        iamTestList << IamTest( i, CLOPIDOGREL );
        iamTestList << IamTest( i, PREVISCAN );
        iamTestList << IamTest( i, LASILIX );
        iamTestList << IamTest( i, TRIATEC );
        iamTestList << IamTest( i, LOVENOX );
        iamTestList << IamTest( i, DEROXAT );
        iamTestList << IamTest( i, SEROPRAM );
        iamTestList << IamTest( i, PROZAC );
        iamTestList << IamTest( i, METHOTREXATE );
        iamTestList << IamTest( i, TICLODIPINE );
        iamTestList << IamTest( i, ALIMTA );
        foreach(int z,ains) {
            iamTestList << IamTest(i,z);
        }
    }

    // AVK
    iamTestList << IamTest( TRIFLUCAN, PREVISCAN );
    iamTestList << IamTest( IBUPROFENE, PREVISCAN );
    iamTestList << IamTest( APRANAX, PREVISCAN );

    // p4
    iamTestList << IamTest(CLASTOBAN, ESTRACYT);

    // IPP  p29
    foreach(int i, ipp) {
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
    QSet<int> list;
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
    list << ACETAZOLAMIDE << CIMETIDINE << CLARITHROMYCINE << CLONAZEPAM << CLOZAPINE << DANAZOL << DEXTROPROPOXYPHENE << DIGOXINE
            << ERYTHROMYCINE << FUROSEMIDE << ETHOSUXIMIDE<< FELBAMATE << FLUCONAZOLE << FLUOXETINE << FLUVOXAMINE
            << HALOPERIDOL << ISONIAZIDE << JOSAMYCINE << LAMOTRIGINE << LITHIUM << MILLEPERTUIS << OLANZAPINE
            << PAROXETINE << PHENOBARBITAL << PHENYTOINE << RIFAMPICINE<< RISPERIDONE<< SERTRALINE
            << TOPIRAMATE << TRAMADOL << ACIDE_VALPROIQUE;
    foreach(int i, list) {
        iamTestList << IamTest( i, CARBAMAZEPINE );
    }

    // AMINOSIDES p9
    list.clear();
    list << AMPHOTERICINEB << CEFALOTINE << CICLOSPORINE << COLISTINE << POLYMYXINEB << TACROLIMUS;
    //            BOTULIQUETOXINE
    //            CURARES
    //            ORGANOPLATINES
    //            DIURÉTIQUES DE L'ANSE
    foreach(int i, list) {
        foreach(int z, aminosides)
            iamTestList << IamTest(z,i);
    }
    foreach(int i, aminosides) {
        foreach(int z, aminosides)
            iamTestList << IamTest(z,i);
    }

    // BB dans insuf cardiaque
    list.clear();
    list << AMIODARONE << BEPRIDIL << DILTIAZEM << FLOCTAFENINE << LIDOCAINE << VERAPAMIL;
    foreach(int i, betaBloqInsuf) {
        foreach(int z, list)
            iamTestList << IamTest( i, z );
        foreach(int z, anticholinesterasiques)
            iamTestList << IamTest( i, z );
        foreach(int z, antihtaCentraux)
            iamTestList << IamTest( i, z );
        foreach(int z, dihydropyridines)
            iamTestList << IamTest( i, z );
        foreach(int z, neuroleptiques)
            iamTestList << IamTest( i, z );
        iamTestList << IamTest( i, DIGOXINE );
    }



    //        ANESTHÉSIQUES VOLATILS HALOGÉNÉS
    //        ANTIARYTHMIQUES CLASSE I (SAUF LIDOCAÏNE)
    //        ANTIDÉPRESSEURS IMIPRAMINIQUES

    //        MÉDICAMENTS SUSCEPTIBLES DE DONNER DES TORSADES DE POINTES
    //        SULFAMIDES HYPOGLYCÉMIANTS

    iamTestList << IamTest(ACIDE_FUSIDIQUE, SIMVA);

    mfDrugsModel *m = mfDrugsModel::instance();

    QProgressDialog progress("Processing thesaurus file : reading blocks", "Abort", 0, iamTestList.count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    int tested = 0;
    int founded = 0;
    progress.setValue(1);
    foreach(const IamTest &iam, iamTestList) {
        m->clearDrugsList();
        if (iam.m_CIS1==iam.m_CIS2)
            continue;
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
        } else
            ++founded;

        if (tested%10==0) {
            progress.setValue(tested);
            if (progress.wasCanceled())
                break;
        }
    }

    qWarning() << "Tested IAM" << tested << "Founded IAM" << founded;
}

#include <QApplication>
#include <QDebug>

#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugInteraction.h>


#include <tkSettings.h>

QDrugsList drugList;
const QStringList cisList =
        QStringList()
        << "64539416"  // Clamoxyl 1g dispersible
        << "62846562"  // Amox 1g dispersible
        << "64728712"  // Abufene (non Inn known)
        << "63123849"  // Caduet (amlor+atorvastatine)
        ;

void testDrugsModel()
{
    mfDrugsModel *m = mfDrugsModel::instance();
    mfDrugsBase *b = mfDrugsBase::instance();

    // Add drugs
    foreach( const QString &cis, cisList)  {
        m->addDrug(cis.toInt(), true);
    }
    // Warn drugs
    int i;
    for(i=0; i< m->rowCount();++i) {
        QString tmp;
        tmp += m->index( i, Drug::Denomination ).data().toString().leftJustified(60,' ');
        tmp += "\t";
        tmp += "Iam=";
        tmp += mfDrugInteraction::typeToString(m->index( i, Drug::MaximumLevelOfInteraction ).data().toInt());
        qWarning() << tmp;
    }
    // Find equivalents
    drugList = m->drugsList();

    drugList.clear();
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

    mfDrugsBase *base = mfDrugsBase::instance();

    testDrugsModel();

    return 0;//app.exec();
}

#include "DosageSenderTester.h"

#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugInteraction.h>
#include <drugsmodel/mfDrugsIO.h>

#include <tkGlobal.h>
#include <tkSettings.h>
#include <tkSendMessage.h>

#include <QApplication>
#include <QVariant>
#include <QDebug>
#include <QHash>
#include <QString>

DosageSenderTester::DosageSenderTester(QObject *parent) :
        QObject(parent)
{
    connect(mfDrugsIO::instance(), SIGNAL(transmissionDone()), this, SLOT(done()));
}

void DosageSenderTester::start()
{
    qWarning() << "starting dosage transmission";
    if (!mfDrugsIO::instance()->startsDosageTransmission()) {
        qWarning() << "No transmission";
        Q_EMIT end();
    }
}

void DosageSenderTester::done()
{
    qWarning() << "message sended" << sender.resultMessage();
    if (sender.resultMessage().contains("OK")) {
        qWarning() << "All good";
        mfDrugsBase::instance()->markAllDosageTransmitted(m_Datas.keys());
    }
    Q_EMIT end();
}

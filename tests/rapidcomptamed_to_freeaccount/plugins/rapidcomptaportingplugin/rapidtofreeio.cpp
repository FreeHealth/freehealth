#include "rapidtofreeio.h"
#include "connect.h"
#include <accountbaseplugin/accountbase.h>
#include <QSqlQuery>

enum Warn {WarnDebugMessage = true};

RapidToFreeIO::RapidToFreeIO(QObject * parent)
{
    m_honorairesFields = "id_hono,id_usr,id_drtux_usr,patient,id_site,id_payeurs,GUID,praticien,date,acte,acteclair,remarque,esp,chq,cb,vir,daf,autre,du,du_par,valide,tracabilite";
    fillHashOfAccount();
    fillHashHono();
}

RapidToFreeIO::~RapidToFreeIO(){}

bool RapidToFreeIO::initialiseBases()
{
    //    	AccountBase::instance()->initialize();
    if (!connectToRapidComptamed()) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect to rapidcomptamed" ;
        return false;
    }
    return true;
}

bool RapidToFreeIO::connectToRapidComptamed()
{
    m_dbRapidCompta = QSqlDatabase::addDatabase("QMYSQL", "comptabilite");
    ConnectionDialog * c = new ConnectionDialog(this);
    if (c->exec()==QDialog::Accepted)
    {
        m_dbRapidCompta.setHostName(c->host());
        m_dbRapidCompta.setDatabaseName("comptabilite");
        m_dbRapidCompta.setUserName(c->login());
        m_dbRapidCompta.setPassword(c->password());
        if (!m_dbRapidCompta.isOpen() && !m_dbRapidCompta.open())
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << m_dbRapidCompta.lastError().text() ;
            return false;
        }
        return true;
    }
    return false;
}

bool RapidToFreeIO::getAndSetAccount()
{
    m_dbAccount = QSqlDatabase::database("account");
    int lastId = 0;
    QSqlQuery queryLastId(m_dbAccount);
    QString reqLastId = QString("SELECT %1 FROM %2").arg("ACCOUNT_ID","account");
    while (queryLastId.next())
    {
        int id = queryLastId.value(0).toInt();
        if (id>lastId)
        {
            lastId = id;
        }
    }
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " last account id =" << QString::number(lastId) ;
    QSqlQuery queryHono(m_dbRapidCompta);
    QString req = QString("SELECT %1 FROM %2").arg(m_honorairesFields,"honoraires");
    if (!queryHono.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << queryHono.lastError().text() ;
        return false;

    }
    while (queryHono.next())
    {
        QString id_hono=queryHono.value(RAPID_ID_HONO).toString();
        QString id_usr=queryHono.value(RAPID_ID_USR).toString();
        QString id_drtux_usr=queryHono.value(RAPID_ID_DRTUX_USR).toString();
        QString patient=queryHono.value(RAPID_PATIENT).toString();
        QString id_site=queryHono.value(RAPID_ID_SITE).toString();
        QString id_payeurs=queryHono.value(RAPID_ID_PAYEURS).toString();
        QString guid=queryHono.value(RAPID_GUID).toString();
        QString praticien=queryHono.value(RAPID_PRATICIEN).toString();
        QString date=queryHono.value(RAPID_DATE).toString();
        QString acte=queryHono.value(RAPID_ACTE).toString();
        QString acteclair=queryHono.value(RAPID_ACTECLAIR).toString();
        QString remarque=queryHono.value(RAPID_REMARQUE).toString();
        QString esp=queryHono.value(RAPID_ESP).toString();
        QString chq=queryHono.value(RAPID_CHQ).toString();
        QString cb=queryHono.value(RAPID_CB).toString();
        QString vir=queryHono.value(RAPID_VIR).toString();
        QString daf=queryHono.value(RAPID_DAF).toString();
        QString autre=queryHono.value(RAPID_AUTRE).toString();
        QString du=queryHono.value(RAPID_DU).toString();
        QString du_par=queryHono.value(RAPID_DU_PAR).toString();
        QString valide=queryHono.value(RAPID_VALIDE).toString();
        QString tracabilite=queryHono.value(RAPID_TRACABILITE).toString();


    }
    return true;
}

void RapidToFreeIO::fillHashOfAccount()
{
    m_hashAccount.insert(ACCOUNT_ID,"ACCOUNT_ID");
    m_hashAccount.insert(ACCOUNT_UID,"ACCOUNT_UID");
    m_hashAccount.insert(USER_UID,"USER_UID");
    m_hashAccount.insert(PATIENT_UID,"PATIENT_UID");
    m_hashAccount.insert(PATIENT_NAME,"PATIENT_NAME");
    m_hashAccount.insert(SITE_ID,"SITE_ID");
    m_hashAccount.insert(INSURANCE_ID,"INSURANCE_ID");
    m_hashAccount.insert(DATE,"DATE");
    m_hashAccount.insert(MP_XML,"MP_XML");
    m_hashAccount.insert(MP_TXT,"MP_TXT");
    m_hashAccount.insert(COMMENT,"COMMENT");
    m_hashAccount.insert(CASH,"CASH");
    m_hashAccount.insert(CHEQUE,"CHEQUE");
    m_hashAccount.insert(VISA,"VISA");
    m_hashAccount.insert(BANKING,"BANKING");
    m_hashAccount.insert(OTHER,"OTHER");
    m_hashAccount.insert(DUE,"DUE");
    m_hashAccount.insert(DUE_BY,"DUE_BY");
    m_hashAccount.insert(VALID,"VALID");
    m_hashAccount.insert(TRACE,"TRACE");
}

void RapidToFreeIO::fillHashHono()
{
    m_hashHono.insert(RAPID_ID_HONO ,"id_hono");
    m_hashHono.insert(RAPID_ID_USR ,"id_usr");
    m_hashHono.insert(RAPID_ID_DRTUX_USR ,"id_drtux_usr");
    m_hashHono.insert(RAPID_PATIENT ,"patient");
    m_hashHono.insert(RAPID_ID_SITE ,"id_site");
    m_hashHono.insert(RAPID_ID_PAYEURS ,"id_payeurs");
    m_hashHono.insert(RAPID_GUID ,"GUID");
    m_hashHono.insert(RAPID_PRATICIEN ,"praticien");
    m_hashHono.insert(RAPID_DATE ,"date");
    m_hashHono.insert(RAPID_ACTE ,"acte");
    m_hashHono.insert(RAPID_ACTECLAIR ,"acteclair");
    m_hashHono.insert(RAPID_REMARQUE ,"remarque");
    m_hashHono.insert(RAPID_ESP ,"esp");
    m_hashHono.insert(RAPID_CHQ ,"chq");
    m_hashHono.insert(RAPID_CB ,"cb");
    m_hashHono.insert(RAPID_VIR ,"vir");
    m_hashHono.insert(RAPID_DAF ,"daf");
    m_hashHono.insert(RAPID_AUTRE ,"autre");
    m_hashHono.insert(RAPID_DU ,"du");
    m_hashHono.insert(RAPID_DU_PAR ,"du_par");
    m_hashHono.insert(RAPID_VALIDE ,"valide");
    m_hashHono.insert(RAPID_TRACABILITE ,"tracabilite");
}

void RapidToFreeIO::linkTableHonoAndAccount()
{//TODO gerer creation d'UID MED, UID_SITE, ...
    m_hashLink.insert(USER_UID,RAPID_ID_USR);
    m_hashLink.insert(PATIENT_UID,0);
    m_hashLink.insert(PATIENT_NAME,RAPID_PATIENT);
    m_hashLink.insert(SITE_ID,RAPID_ID_SITE);
    m_hashLink.insert(INSURANCE_ID,RAPID_ID_PAYEURS);
    m_hashLink.insert(DATE,RAPID_DATE);
    m_hashLink.insert(MP_XML,0);
    m_hashLink.insert(MP_TXT,RAPID_ACTE);
    m_hashLink.insert(COMMENT,RAPID_REMARQUE);
    m_hashLink.insert(CASH,RAPID_ESP);
    m_hashLink.insert(CHEQUE,RAPID_CHQ);
    m_hashLink.insert(VISA,RAPID_CB);
    m_hashLink.insert(BANKING,RAPID_VIR);
    m_hashLink.insert(OTHER,RAPID_AUTRE);
    //    m_hashLink.insert(DU,RAPID_DU);
    m_hashLink.insert(DUE_BY,RAPID_DU_PAR);
}

QStringList RapidToFreeIO::getListOfRapidcomptamedUsers()
{
    QStringList listOfUsers;
    QSqlQuery qUsers(m_dbRapidCompta);
    QString req = QString("SELECT login FROM utilisateurs");
    if (!qUsers.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << qUsers.lastError().text() ;
    }
    while (qUsers.next())
    {
        listOfUsers << qUsers.value(0).toString();
    }
    return listOfUsers;
}




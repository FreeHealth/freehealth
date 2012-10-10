#include "rapidtofreeio.h"
#include "connect.h"
#include <accountbaseplugin/accountbase.h>
#include <QSqlQuery>

using namespace AccountDB;

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
    AccountBase::instance()->initialize();
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
    QSqlQuery queryHono(m_dbRapidCompta);
    QString req = QString("SELECT %1 FROM %2").arg(m_honorairesFields,"honoraires");
    if (!queryHono.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << queryHono.lastError().text() ;
        return false;

    }
    int lineCounter = 0;
    while (queryHono.next())
    {
        /*QString id_hono=queryHono.value(RAPID_ID_HONO).toString();
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
        QString tracabilite=queryHono.value(RAPID_TRACABILITE).toString();*/
        
        ++lineCounter;
        QList<QVariant> rapidFieldsList;
        for (int RapidEnum = 0; RapidEnum < RapidFields_MaxParam; ++RapidEnum)
        {
        	  rapidFieldsList << queryHono.value(RapidEnum);
            }
        if (!insertIntoAccount(rapidFieldsList))
        {
        	  qWarning() << __FILE__ << QString::number(__LINE__) 
        	             << "unable to insert datas into account at line : "
        	             << QString::number(lineCounter) ;
            }
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

int RapidToFreeIO::getAccountLastId()
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
    return lastId;
}


bool RapidToFreeIO::insertIntoAccount(QList<QVariant> listOfRapidDatas)
{
    for (int accountFieldNbr = 0; accountFieldNbr < AccountFields_MaxParam; ++accountFieldNbr)
    {
    	  int accountId = 0;
    	  switch(accountFieldNbr){
    	      case ACCOUNT_ID :
    	          accountId = getAccountLastId() +1;
    	          if (!insertIntoFieldOfAccount(accountId,-1,ACCOUNT_ID))
    	          {
    	          	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to insert " 
    	          	  << listOfRapidDatas.at(ACCOUNT_ID).toString() << " in account";
    	          	  return false;
    	              }
    	          break;
    	      default :
    	          QVariant data = listOfRapidDatas.at(accountFieldNbr);
    	          if (!insertIntoFieldOfAccount(data,accountId,accountFieldNbr))
    	          {
    	          	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to insert " 
    	          	  << data.toString() << " in account";
    	          	  return false;
    	              }
    	          break;    
    	      }    	      
        }
    return true;
}

bool RapidToFreeIO::insertIntoFieldOfAccount(QVariant data, int id, int field)
{
   
    QSqlQuery query(m_dbAccount);
    if (id == -1)
    {
    	  QString datas = "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?";     
          //fieldName = fieldName(AccountDB::Constants::Table_Account,AccountDB::Constants::ACCOUNT_ID);
          QStringList itemsFieldsList;
          itemsFieldsList = m_hashAccount.values();
          QString itemsFields = itemsFieldsList.join(",");
          query.prepare(QString("INSERT INTO %1 (%2) VALUES(%3)").arg("account",itemsFields,datas));
          for (int i = 0; i < AccountFields_MaxParam; ++i)
          {
          	  switch(i){
          	      case ACCOUNT_ID :
          	          query.bindValue(i,data);
          	          break;
          	      case ACCOUNT_UID : case USER_UID :
          	          query.bindValue(i,"{00000000-0000-0000-0000-000000000000}}");
          	          break;
          	      case PATIENT_UID : case PATIENT_NAME : case  SITE_ID : case INSURANCE_ID :
          	          query.bindValue(i,0000000000);
          	          break;
          	      case DATE :
          	          query.bindValue(i,"2000-01-01");
          	          break;
          	      case MP_TXT : case MP_XML : case COMMENT : case DUE_BY :
          	          query.bindValue(i,"NULL");
          	          break;
          	      case VALID : case TRACE :
          	          query.bindValue(i,0);
          	          break;
          	      case CASH : case CHEQUE : case VISA : case BANKING : case OTHER : case DUE :
          	          query.bindValue(i,0.0);
          	          break;
          	      default :
          	          break;    
          	      }
          	  
          	      
              } 
    	      
    	  if (!query.exec())
    	  {
    	  	  qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
    	  	  return false;
    	  	  
    	      }
        }
    else
    {
        QString fieldName = m_hashAccount.value(field)  ;
        query.prepare(QString("UPDATE %1  SET %2 = '%3' WHERE %4 = '%5'").arg("account",fieldName,"?","ACCOUNT_ID",QString::number(id)));
        query.bindValue(0,data);
        if (!query.exec())
        {
        	  qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
        	  return false;
        	  
            }	  
        
        }
    return true;
}


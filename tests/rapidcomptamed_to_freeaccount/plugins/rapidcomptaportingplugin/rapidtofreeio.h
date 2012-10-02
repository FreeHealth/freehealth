#ifndef RAPIDTOFREEIO_H
#define RAPIDTOFREEIO_H

#include <QObject>
#include <QHash>

class RapidToFreeIO:public QObject
{
	public:
		RapidToFreeIO (QObject * parent = 0);
		virtual ~RapidToFreeIO ();
		bool initialiseBases();
		QStringList getListOfRapidcomptamedUsers();
		
	private:
		enum HonoFields
		{
		    RAPID_ID_HONO = 0,
                    RAPID_ID_USR,
                    RAPID_ID_DRTUX_USR,
                    RAPID_PATIENT,
                    RAPID_ID_SITE,
                    RAPID_ID_PAYEURS,
                    RAPID_GUID,
                    RAPID_PRATICIEN,
                    RAPID_DATE,
                    RAPID_ACTE,
                    RAPID_ACTECLAIR,
                    RAPID_REMARQUE,
                    RAPID_ESP,
                    RAPID_CHQ,
                    RAPID_CB,
                    RAPID_VIR,
                    RAPID_DAF,
                    RAPID_AUTRE,
                    RAPID_DU,
                    RAPID_DU_PAR,
                    RAPID_VALIDE,
                    RAPID_TRACABILITE,
                    RapidFiels_MaxParam
		    };
		enum AccountFields
		{
		    ACCOUNT_ID = 0,
                    ACCOUNT_UID,
                    USER_UID,
                    PATIENT_UID,
                    PATIENT_NAME,
                    SITE_ID,
                    INSURANCE_ID,
                    DATE,
                    MP_XML,
                    MP_TXT,
                    COMMENT,
                    CASH,
                    CHEQUE,
                    VISA,
                    BANKING,
                    OTHER,
                    DUE,
                    DUE_BY,
                    VALID,
                    TRACE,
                    AccountFields_MaxParam
		    };
		    QString m_honorairesFields;
		
	private:
		bool connectToRapidComptamed();
		bool getAndSetAccount();
		void fillHashOfAccount();
		void fillHashHono();
		void linkTableHonoAndAccount();
		QSqlDatabase m_dbRapidCompta;
		QSqlDatabase m_dbAccount;
		QHash<int,QString> m_hashAccount;
		QHash<int,QString> m_hashHono;
		QHash<int,int> m_hashLink;
		
};

#endif


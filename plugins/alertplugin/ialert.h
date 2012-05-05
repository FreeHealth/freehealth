#ifndef IALERT
#define IALERT

#include <QObject>
#include <QUuid>
#include <QString>

namespace Alert {

class IAlert : public QObject
{
	Q_OBJECT
	public:
	    enum TypeOfAlert
	    {
	        ALL_ALERTS = 0,
	        PATIENT_PRIMARY_PREVENTION_ALERTS,
	        PATIENT_SECONDARY_PREVENTION_ALERTS,
	        PATIENT_PRESCRIPTION_ALERTS,
	        PATIENT_NOTES,
	        TypeOfAlert_MaxParams
	        };
	    enum ItemsDefinition
	    {
                TYPE_OF_ITEM = 0,
                PARENT_ITEM,
                PARENT_OR_CHILD,
                ICON,
                DATE_DONE,
                DATE_NEXT,
                ABSTRACT,
                ID_ITEM,
                PATIENT_UID,
                USER_UID,           
                ItemsDefinition_MaxParam     
	        };
	public:
		IAlert (){}
		virtual ~IAlert (){}
		static IAlert * instance();
		//methods
		virtual void initialize() = 0;
		virtual QString setAlertUuid() = 0 ;
		virtual void showIHMaccordingToType(int) = 0 ;
		
	private:
			
};

};

#endif


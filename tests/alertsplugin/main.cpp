#include "ialert.h"

#include <QApplication>
#include <QStringList>

int main (int argc, char * argv[])
{
	QApplication a(argc,argv);
	QStringList args;
	args = a.arguments();
	int choice = 0;
	if (args.size()>1)
	{
		  choice = args[1].toInt();
	    }
	Alert::IAlert * alert = Alert::IAlert::instance();
	alert->initialize();
	alert->showIHMaccordingToType(choice);//NOTES
	return a.exec();
}

//Arguments for choices of alerts are :
/*	        ALL_ALERTS = 0,
	        PATIENT_PRIMARY_PREVENTION_ALERTS=1,
	        PATIENT_SECONDARY_PREVENTION_ALERTS=2,
	        PATIENT_PRESCRIPTION_ALERTS=3,
	        PATIENT_NOTES=4
*/


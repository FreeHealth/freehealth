#include "preferedreceipts.h"
#include "ui_preferedreceipts.h"
#include "choiceDialog.h"
#include "receiptsIO.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <QMessageBox>

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }

PreferedReceipts::PreferedReceipts(QWidget * parent):QWidget(parent),ui(new Ui::PreferedReceiptsWidget){
    ui->setupUi(this);
    receiptsEngine receiptsIO;
    m_typeOfChoice = 0;
    choiceDialog * choice = new choiceDialog(this);
    if (choice->exec() == QDialog::Accepted)
    {
    	  m_typeOfChoice = choice->returnChoiceDialog();
    	  insertPreferedValuesIntoAccount();
        }
}

PreferedReceipts::~PreferedReceipts(){}

void PreferedReceipts::insertPreferedValuesIntoAccount(){
    receiptsEngine receiptsIO;
    QString userUuid = user()->value(Core::IUser::Uuid).toString();
    QString patientUid = patient()->data(Core::IPatient::Uid).toString();
    if (patientUid.isEmpty())
    {
    	  patientUid = "no-patient-uid";
        }
    QString patientName = patient()->data(Core::IPatient::FullName).toString();
    if (patientName.isEmpty())
    {
    	  patientName = "Patient Name";
        }
    QHash <int,QVariant> hashOfPrefValues = receiptsIO.getListOfPreferedValues(userUuid,m_typeOfChoice); 
    if (!receiptsIO.insertIntoAccount(hashOfPrefValues,userUuid))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to insert datas into account")
    	                                     +__FILE__+QString::number(__LINE__),QMessageBox::Ok);
        }
}

#include "patientbar.h"
#include "patientmodel.h"
#include "ui_patientbar.h"

#include <QDataWidgetMapper>

using namespace Patients;

namespace Patients {
namespace Internal {
class PatientBarPrivate
{
public:
    PatientBarPrivate() :
            ui(new Ui::PatientBar)
    {
    }

    ~PatientBarPrivate()
    {
        delete ui;
    }

public:
    Ui::PatientBar *ui;
};
}
}

PatientBar::PatientBar(QWidget *parent) :
    QWidget(parent),
    d(new Internal::PatientBarPrivate)
{
    d->ui->setupUi(this);
}

PatientBar::~PatientBar()
{
}

void PatientBar::setPatientModel(PatientModel *model)
{
//    d->m_Mapper->setModel(model);
}

void PatientBar::setCurrentIndex(const QModelIndex &index)
{
//    d->m_Mapper->setCurrentModelIndex(index);
}

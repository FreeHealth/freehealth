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
            ui(new Ui::PatientBar),
            m_Mapper(0)
    {
    }

    ~PatientBarPrivate()
    {
        delete ui;
    }

public:
    Ui::PatientBar *ui;
    QDataWidgetMapper *m_Mapper;
};
}
}

PatientBar::PatientBar(QWidget *parent) :
    QWidget(parent),
    d(new Internal::PatientBarPrivate)
{
    d->ui->setupUi(this);
    d->m_Mapper = new QDataWidgetMapper(this);
    d->m_Mapper->addMapping(d->ui->names, PatientModel::FullName, "text");
    d->m_Mapper->addMapping(d->ui->gender, PatientModel::IconizedGender, "icon");
    d->m_Mapper->addMapping(d->ui->age, PatientModel::Age, "text");
    d->m_Mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

}

PatientBar::~PatientBar()
{
}

void PatientBar::setPatientModel(PatientModel *model)
{
    d->m_Mapper->setModel(model);
}

void PatientBar::setCurrentIndex(const QModelIndex &index)
{
    d->m_Mapper->setCurrentModelIndex(index);
}

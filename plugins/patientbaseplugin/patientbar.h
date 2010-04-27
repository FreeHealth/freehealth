#ifndef PATIENTBAR_H
#define PATIENTBAR_H

#include <QWidget>
#include <QModelIndex>

namespace Patients {
class PatientModel;

namespace Internal {
class PatientBarPrivate;
}

class PatientBar : public QWidget
{
    Q_OBJECT

public:
    PatientBar(QWidget *parent = 0);
    ~PatientBar();

    void setPatientModel(PatientModel *model);
    void setCurrentIndex(const QModelIndex &index);

protected:

private:
    Internal::PatientBarPrivate *d;

};

}  // End namespace Patients

#endif // PATIENTBAR_H

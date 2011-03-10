#ifndef IDRUGINTERACTIONALERT_H
#define IDRUGINTERACTIONALERT_H

#include <QObject>
class QIcon;


namespace DrugsDB {

class DrugInteractionResult;
class IDrug;

class IDrugInteractionAlert : public QObject
{
    Q_OBJECT
public:
    enum ProcessTime {
        BeforePrescription = 0,
        DuringPrescription,
        AfterPrescription,
        BeforePrinting
    };

    explicit IDrugInteractionAlert(QObject *parent = 0);
    virtual ~IDrugInteractionAlert();

    virtual void connectTo(DrugInteractionResult *result) = 0;

    virtual QIcon icon(IDrug *drug) = 0;
    virtual QString alertMessage(IDrug *drug) = 0;

public Q_SLOTS:
    virtual void process(const ProcessTime processTime) = 0;

Q_SIGNALS:
    void processDone();
};

}  // End namespace DrugsDB

#endif // IDRUGINTERACTIONALERT_H

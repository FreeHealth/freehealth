#ifndef PATIENTIMPORT_PATIENTIMPORTDIALOG_H
#define PATIENTIMPORT_PATIENTIMPORTDIALOG_H

#include <patientimportplugin/patientimport_exporter.h>
#include <patientimportconstants.h>
#include <qglobal.h>
#include <QDialog>

namespace PatientImport {
namespace Internal {
class PatientImportDialogPrivate;
} // namespace Internal

class PATIENTIMPORT_EXPORT PatientImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PatientImportDialog(QWidget *parent = 0);
    ~PatientImportDialog();

    void setSoftwareComboBoxItems(const QStringList &softlist);

private Q_SLOTS:
    QString getFileName();

private:
    void changeEvent(QEvent *e);

private:
    Internal::PatientImportDialogPrivate *d;
};

} // namespace PatientImport

#endif // PATIENTIMPORT_PATIENTIMPORTDIALOG_H

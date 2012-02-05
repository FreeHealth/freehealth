#ifndef PREGNANCY_IMPORTER_H
#define PREGNANCY_IMPORTER_H

#include <QHash>
#include <QString>

namespace PregnancyImporter
{
enum FieldType {
    Field_Name,
    Field_Category,
    Field_Safety,
    Field_Class1,
    Field_Class2,
    Field_Class3
};

typedef QHash<FieldType, QString> PregnancyRecord;

/**
 * Load a pregnancy db file.
 * \param {QString} fileName The .js pregnancy filename
 * \param {QList<PregnancyRecord>} records a reference to a list of records in which all read records will be loaded into
 * \param {QString} [errorMsg] An optional QString used to get the error message if loading failed
 * \return true if success, false if failed
 */
bool load(const QString &fileName, QList<PregnancyRecord> &records, QString *errorMsg = 0);
}

#endif // PREGNANCY_IMPORTER_H

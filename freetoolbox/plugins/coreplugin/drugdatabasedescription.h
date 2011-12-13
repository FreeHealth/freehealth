#ifndef DRUGSDB_DRUGDATABASEDESCRIPTION_H
#define DRUGSDB_DRUGDATABASEDESCRIPTION_H

#include <utils/genericdescription.h>
#include <coreplugin/core_exporter.h>

namespace Core {

class CORE_EXPORT DrugDatabaseDescription : public Utils::GenericDescription
{
public:
    enum Data {
        Language = Utils::GenericDescription::NonTranslatableExtraData + 1,
        Vendor,
        Copyright,
        DrugUidName,
        IsAtcValid,
        IsDDIValid,
        ComplementaryWebLink,
        PackUidName,
        DrugNameConstructor
    };

    DrugDatabaseDescription();
};

}

#endif // DRUGSDB_DRUGDATABASEDESCRIPTION_H

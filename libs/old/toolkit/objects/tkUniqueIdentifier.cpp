#include "tkUniqueIdentifier.h"

#include <tkConstants.h>

Q_TK_USING_CONSTANTS

tkUniqueIdentifier* tkUniqueIdentifier::m_instance = 0;

tkUniqueIdentifier::tkUniqueIdentifier()
{
    m_instance = this;
    m_uniqueIdentifiers.insert(C_GLOBAL, C_GLOBAL_ID);
}

tkUniqueIdentifier::~tkUniqueIdentifier()
{
    m_instance = 0;
}

bool tkUniqueIdentifier::hasUniqueIdentifier(const QString &id) const
{
    return m_uniqueIdentifiers.contains(id);
}

int tkUniqueIdentifier::uniqueIdentifier(const QString &id)
{
    if (hasUniqueIdentifier(id))
        return m_uniqueIdentifiers.value(id);

    int uid = m_uniqueIdentifiers.count() + 1;
    m_uniqueIdentifiers.insert(id, uid);
    return uid;
}

QString tkUniqueIdentifier::stringForUniqueIdentifier(int uid)
{
    return m_uniqueIdentifiers.key(uid);
}

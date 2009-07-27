#ifndef TKUNIQUEIDENTIFIER_H
#define TKUNIQUEIDENTIFIER_H

#include <tkExporter.h>

#include <QHash>
#include <QString>

#define tkUID  tkUniqueIdentifier::instance()

class Q_TK_EXPORT tkUniqueIdentifier
{
public:
    static tkUniqueIdentifier *instance()
    {
        if (!m_instance)
            m_instance = new tkUniqueIdentifier();
        return m_instance;
    }

    ~tkUniqueIdentifier();

    bool hasUniqueIdentifier(const QString &id) const;
    int uniqueIdentifier(const QString &id);
    QString stringForUniqueIdentifier(int uid);

private:
    tkUniqueIdentifier();
    QHash<QString, int> m_uniqueIdentifiers;
    static tkUniqueIdentifier *m_instance;
};

#endif // TKUNIQUEIDENTIFIER_H

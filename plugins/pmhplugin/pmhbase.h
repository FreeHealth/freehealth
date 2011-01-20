#ifndef PMHBASE_H
#define PMHBASE_H

#include <utils/database.h>

namespace PMH {
namespace Internal {
class PmhBasePrivate;

class PmhBase : public QObject, public Utils::Database
{
    Q_OBJECT

protected:
    PmhBase(QObject *parent = 0);

public:
    // Constructor
    static PmhBase *instance();
    virtual ~PmhBase();

    // initialize
    bool init();


private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );

private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    static bool m_initialized;
    static PmhBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace PMH


#endif // PMHBASE_H

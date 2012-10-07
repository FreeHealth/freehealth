
#ifndef CONNEXION_H
#define CONNEXION_H

#include <QObject>
#include <QSqlDatabase>

namespace Common {

class Connexion : public QObject
{
    Q_OBJECT
    public:
        enum DatabaseDriver {
            Driver_SQLite = 0,
            Driver_MySQL,
            Driver_PostGreSQL,
            };

        Connexion(QObject *parent = 0);
        ~Connexion();

        bool connect();
    private:
        bool createDatabaseSchema(int driver) ;
        bool createDatabase(int driver) ;
        bool createFirstSqlConnection() const;
        bool isDatabaseCorrupted(const int driver,QSqlDatabase &);
        bool setFirstDefaultValues();
        
        bool fillWithValuesForTesting();
    
    private:
        bool m_IsDBConfigured;
        QSqlDatabase m_db;
    
};

} // End namespace Common

#endif   // Connexion_H

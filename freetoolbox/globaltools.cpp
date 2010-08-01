#include "globaltools.h"
#include "constants.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QStringList>
#include <QProcess>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTime>
#include <QVariant>

#include <QDebug>

#include <quazip/quazip/quazip.h>
#include <quazip/quazip/quazipfile.h>

#include <utils/log.h>

namespace Tools
{

const bool unzipFile( const QString & fileName, const QString & pathToUnZippedFiles )
{
     Q_ASSERT_X( QFile( fileName ).exists() , "Function unzipFile()",
                 qPrintable( QString( "File %1 does not exists" ).arg( fileName ) ) );

     Q_ASSERT_X( QDir( pathToUnZippedFiles ).exists() , "Function unzipFile()",
                 qPrintable( QString( "Dir %1 does not exists" ).arg( pathToUnZippedFiles ) ) );

//     QProcess unzip;
//     unzip.start("unzip", QStringList() << fileName << "-d " + pathToUnZippedFiles);
//
//     if (!unzip.waitForFinished(100000))
//         return false;
//
//     Utils::Log::addMessage("Tools", "unzip returned : " + unzip.readAll());
//     return true;

     QuaZip zip( fileName );
     if (!zip.open( QuaZip::mdUnzip ) ) {
          Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
          return false;
     }

     QuaZipFileInfo info;
     QuaZipFile file( &zip );
     QFile out;
     QString name;
     char c;

     for ( bool more = zip.goToFirstFile(); more; more = zip.goToNextFile() ) {
          if ( !file.open( QIODevice::ReadOnly ) ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
               return false;
          }

          name = file.getActualFileName();

          if ( file.getZipError() != UNZ_OK ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
               return false;
          }
          out.setFileName( pathToUnZippedFiles + QDir::separator() + name );

          // inform user
          Utils::Log::addMessage("Tools", QString( "Zip : extracting : %1" ).arg( out.fileName() ));

          if ( !out.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( out.fileName() ).arg( out.error() ));
               return false;
          }
          while ( file.getChar( &c ) ) out.putChar( c );
          out.close();

          if ( file.getZipError() != UNZ_OK ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
               return false;
          }

          if ( !file.atEnd() ) {
               Utils::Log::addError("Tools", QString( "Zip : read all but not EOF : " ) + fileName );
               return false;
          }
          file.close();

          if ( file.getZipError() != UNZ_OK ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ) );
               return false;
          }
     }
     zip.close();
     if ( zip.getZipError() != UNZ_OK ) {
          Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
          return false;
     }

     return true;
}

const void unzipAllFilesIntoDirs( const QStringList & paths )
{
     foreach( QString p, paths ) {
          QDir d( p );
          if ( !d.exists() ) continue;

          // get all zip files in dir
          d.setNameFilters( QStringList() << "*.zip" );
          d.setFilter( QDir::Files | QDir::NoSymLinks | QDir::Readable );
          QStringList zipFiles = d.entryList();

          foreach( QString f, zipFiles ) {
               // if file if already unzipped dir with its baseName exists and is populated with files
               QDir dz( p );
               dz.setFilter( QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::NoDotAndDotDot );
               if ( ( dz.cd( QFileInfo( f ).baseName() ) ) &&
                         ( dz.entryList().count() ) )
                    continue;

               // d must not change here +++
               // file was not unzipped by me, so do it
               // in first create the output directory
               if ( !d.cd( QFileInfo( f ).baseName()  ) ) {
                    d.mkdir( QFileInfo( f ).baseName() );
                    dz.cd( QFileInfo( f ).baseName() );
               }
               else d.cdUp();

               // in second unzip file to the output directory
               unzipFile( d.absolutePath() + QDir::separator() + f, dz.absolutePath() );
          }
     }
}

QString noAccent(const QString & s)
{
    QString tmp = s;
    tmp.replace( QString::fromUtf8( "é" ) , "e" );
    tmp.replace( QString::fromUtf8( "è" ) , "e" );
    tmp.replace( QString::fromUtf8( "ê" ) , "e" );
    tmp.replace( QString::fromUtf8( "ë" ) , "e" );
    tmp.replace( QString::fromUtf8( "î" ) , "i" );
    tmp.replace( QString::fromUtf8( "ï" ) , "i" );
    tmp.replace( QString::fromUtf8( "É" ) , "E" );
    tmp.replace( QString::fromUtf8( "È" ) , "E" );
    tmp.replace( QString::fromUtf8( "Ê" ) , "E" );
    tmp.replace( QString::fromUtf8( "Ë" ) , "E" );
    tmp.replace( QString::fromUtf8( "Î" ) , "I" );
    tmp.replace( QString::fromUtf8( "Ï" ) , "I" );
    return tmp;
}

QString getBlock(const QString &content, const int posStart, int &posEnd, const QString &delimiter)
{
    // find first delimiter starting at pos posStart
    int begin = content.indexOf( delimiter, posStart );
    int end = content.indexOf( delimiter, begin + delimiter.length() + 1 );

    // modify posEnd for next block process
    if (begin == -1) {
        posEnd = posStart;
        return QString::null;
    } else {
        posEnd = end;
    }

    // return block
    QString tmp = content.mid( begin, end - begin );
    tmp.replace( "\r" , "" );
    tmp.replace( "\n" , "" );
    return tmp;
}

QString getBlock(const QString &content, const int posStart, int &posEnd, const QRegExp &delimiter)
{
    // find first delimiter starting at pos posStart
    int begin = content.indexOf( delimiter, posStart );
    int end = content.indexOf( delimiter, begin + 4 );

    // modify posEnd for next block process
    if (begin == -1) {
        posEnd = posStart;
        return QString::null;
    } else {
        posEnd = end;
    }

    // return block
    QString tmp = content.mid( begin, end - begin );
    tmp.replace( "\r" , "" );
    //      tmp.replace( "\n" , "" );
    return tmp;
}

bool executeProcess(const QString &proc)
{
    QProcess process;
    process.start(proc, QIODevice::ReadOnly);

    Utils::Log::addMessage("Tools", QString("Executing process : %1").arg(proc));

    if (!process.waitForStarted())
        Utils::Log::addError("Tools", QString("Process %1 can not start").arg(proc.left(20)));

    if (!process.waitForFinished(100000))
        Utils::Log::addError("Tools", QString("Process %1 does not end").arg(proc.left(20)));

    QString error = process.readAllStandardError();
    if (!error.isEmpty()) {
        Utils::Log::addError("Tools", QString("ERROR : %1").arg(proc));
        Utils::Log::addError("Tools", QString("ERROR : %1").arg(error));
        return false;
    }
    Utils::Log::addMessage("Tools", QString("Process done : %1, output : %2").arg(proc.left(20)).arg(QString(process.readAllStandardOutput())));
    return true;
}

bool executeSqlFile(const QString &connectionName, const QString &fileName)
{
    if (!QFile::exists(qApp->applicationDirPath() + QDir::separator() + fileName)) {
        Utils::Log::addError("Tools", QString("ERROR: missing database schema file : %1. dc_Canadian_DrugsDatabaseCreator::createDatabase()").arg(fileName));
        return false;
    }

    QFile file(qApp->applicationDirPath() + QDir::separator() + fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Utils::Log::addError("Tools", QString("ERROR : Enable to open %1. dc_Canadian_DrugsDatabaseCreator::createDatabase()").arg(fileName));
        return false;
    }

    // execute all sql queries
    QString req = file.readAll();
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");

    QStringList list = req.split("\n");
    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!DB.open())
        return false;
    if (!DB.transaction()) {
        Utils::Log::addError("Tools", "Can not create transaction. Tools::executeSqlFile()");
        return false;
    }

    req.clear();
    QStringList queries;
    // Reconstruct req : removes comments
    foreach(const QString &s, list) {
        if (s.startsWith("--")) {
            Utils::Log::addMessage("Tools", s);
            continue;
        }
        req += s + " \n";
        if (s.endsWith(";")) {
            queries.append(req);
            req.clear();
        }
    }

    // Execute queries
    foreach(const QString &sql, queries) {
        QTime time;
        time.start();

        QString q = sql.simplified();
        // Do not processed empty strings
        if (q.isEmpty())
            continue;

        // No SQLite extra commands
        if (q.startsWith("."))
            continue;

        // No BEGIN, No COMMIT
        if (q.startsWith("BEGIN", Qt::CaseInsensitive) || q.startsWith("COMMIT", Qt::CaseInsensitive))
            continue;

        qWarning() << sql;

        QSqlQuery query(sql, DB);
        if (!query.isActive()) {
            Utils::Log::addError("Tools", QString("SQL ERROR : %1 \"%2\"").arg(query.lastError().text(), sql));
            DB.rollback();
            return false;
        } else {
            Utils::Log::addMessage("Tools", QString("Query correctly done (%1 ms)").arg(time.elapsed()));
        }
    }
    DB.commit();
    return true;
}

bool executeSqlQuery(const QString &sql, const QString &dbName)
{
    QSqlDatabase DB = QSqlDatabase::database(dbName);
    if (!DB.open()) {
        Utils::Log::addError("Tools", "Unable to connect to " + dbName);
        return false;
    }
    QSqlQuery query(sql, DB);
    if (!query.isActive()) {
        Utils::Log::addError("Tools", "Query Error : " + sql + " // " + query.lastError().text());
        return false;
    }
    return true;
}

bool connectDatabase(const QString &connection, const QString &fileName)
{
    // create empty sqlite database
    // test driver
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        Utils::Log::addError("Tools", QString("ERROR : SQLite driver is not available"));
        return false;
    }

    QSqlDatabase DB;

    // test drugs connection
    if (QSqlDatabase::contains(connection)) {
        DB = QSqlDatabase::database(connection);
    }  else {
        DB = QSqlDatabase::addDatabase("QSQLITE" , connection);
        DB.setDatabaseName(qApp->applicationDirPath() + Constants::MACBUNDLE + fileName);

        if (!DB.open()) {
            Utils::Log::addError("Tools", QString("ERROR : %1 // %2").arg(DB.lastError().text()).arg(Constants::MACBUNDLE + fileName));
            return false;
        } else {
            Utils::Log::addMessage("Tools", QString("Connection to database created : %1 %2")
                    .arg(DB.connectionName(), DB.databaseName()));
        }
    }
    return true;
}


QMultiHash<int, int> englishMoleculeLinker(const QHash<QString, int> &mols,
                                           const QHash<QString, QString> &correctedByName,
                                           const QHash<QString, QString> &correctedByAtcCode
                                           )
{
    QMultiHash<int, int> mol_atc;
    // get all ATC ids
    QSqlDatabase iam = QSqlDatabase::database(Constants::IAM_DATABASE_NAME);
    if (!iam.open()) {
        Utils::Log::addError("Tools", "Can not connect to IAM db : Tools::englishMoleculeLinker");
        return mol_atc;
    }

    Utils::Log::addMessage("Tools", "Getting ATC Informations from the interactions database");
    QHash<QString, int> atc_id;
    QMultiHash<QString, int> atcName_id;
    QString req = "SELECT `ID`, `CODE`, `ENGLISH` FROM `ATC` WHERE length(CODE)=7;";
    QSqlQuery query(req, iam);
    if (query.isActive()) {
        while (query.next()) {
            atc_id.insert(query.value(1).toString(), query.value(0).toInt());
            atcName_id.insertMulti(query.value(2).toString().toUpper(), query.value(0).toInt());
        }
    }
    query.finish();
    qWarning() << "ATC" << atc_id.count();

    // manage corrected molecules
    foreach(const QString &mol, correctedByName.keys()) {
        if (!mols.keys().contains(mol))
            continue;
        foreach(int id, atcName_id.values(correctedByName.value(mol)))
            mol_atc.insertMulti(mols.value(mol), id);
    }
    foreach(const QString &mol, correctedByAtcCode.keys()) {
        if (!mols.keys().contains(mol))
            continue;
        QString atcLabel = atcName_id.key(atc_id.value(correctedByAtcCode.value(mol)));
        foreach(int id, atcName_id.values(atcLabel))
            mol_atc.insertMulti(mols.value(mol), id);
    }
    Utils::Log::addMessage("Tools", "Hand made association: " + QString::number(mol_atc.count()));

    // find links
    QStringList unfound;
    foreach(const QString &mol, mols.keys()) {
        int codeMol = mols.value(mol);
        if (!mol_atc.keys().contains(codeMol)) {
            QString molName = mol;
            QList<int> atcIds = atcName_id.values(molName);
            if (atcIds.count()==0) {
                // Not found try some transformations
                // remove '(*)'
                if (molName.contains("(")) {
                    molName = molName.left(molName.indexOf("(")).simplified();
                    atcIds = atcName_id.values(molName);
                    if (atcIds.count()>0) {
                        qWarning() << "Without (*) >>>>>>>>" << molName;
                    }
                }
                if (atcIds.count()==0) {
                    // remove last word : CITRATE, DIHYDRATE, SODIUM, HYDROCHLORIDE, POLISTIREX, BROMIDE, MONOHYDRATE, CHLORIDE, CARBAMATE
                    //  INDANYL SODIUM, ULTRAMICROCRYSTALLINE, TROMETHAMINE
                    molName = molName.left(molName.indexOf(" ")).simplified();
                    atcIds = atcName_id.values(molName);
                    if (atcIds.count()>0) {
                        qWarning() << "Without last word >>>>>>>>" << molName;
                    }
                }
            }
            bool found = false;
            foreach(int id, atcName_id.values(molName)) {
                found = true;
                mol_atc.insertMulti(codeMol, id);
                qWarning() << "Linked" << mol << atcName_id.key(id);
            }
            if(!found) {
                if (!unfound.contains(mol))
                        unfound << mol;
            }
        }
    }

    qWarning() << "\n\n" << "FOUNDED" << mol_atc.uniqueKeys().count() << "\nLEFT" << unfound.count() << "\n\n";

    foreach(const QString &u, unfound)
        qWarning() << u;

    return mol_atc;
}

} // end namespace

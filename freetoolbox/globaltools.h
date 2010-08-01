#ifndef GLOBALTOOLS_H
#define GLOBALTOOLS_H

#include <QString>
#include <QHash>
#include <QMultiHash>

namespace Tools
{


const void unzipAllFilesIntoDirs( const QStringList & paths );
const bool unzipFile( const QString & fileName, const QString & pathToUnZippedFiles );

QString noAccent(const QString & s);

QString getBlock(const QString &content, const int posStart, int &posEnd, const QRegExp &delimiter);
QString getBlock(const QString &content, const int posStart, int &posEnd, const QString &delimiter);

bool executeProcess(const QString &proc);
bool executeSqlFile(const QString &connectionName, const QString &fileName);
bool executeSqlQuery(const QString &sql, const QString &dbName);

bool connectDatabase(const QString &connection, const QString &fileName);


QMultiHash<int, int> englishMoleculeLinker(const QHash<QString, int> &mols,
                                           const QHash<QString, QString> &correctedByName,
                                           const QHash<QString, QString> &correctedByAtcCode = QHash<QString, QString>());

}

#endif // GLOBALTOOLS_H

/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       Jerome Pinguet <jerome@jerome.cc>                                           *
 ***************************************************************************/
/**
  \class Utils::Randomizer
  Randomizer can be used for populating database with random: names, firstnames, zipcodes and cities, strings, numbers, dates...
*/

// Words database creation
// sqlite3 words.db
// CREATE TABLE WORDS (
// ID integer primary key autoincrement,
// LANG char(2) DEFAULT 'fr',
// WORD char(255) NOT NULL
// );
// CREATE TABLE TMP (W char(255));
// .import listemotsfr.txt TMP
// INSERT INTO WORDS (WORD) SELECT upper(W) FROM TMP;
// DROP TABLE TMP;

// FirstNames database creation
// sqlite3 firstnames.db
// CREATE TABLE FIRSTNAMES (
// ID integer primary key autoincrement,
// LANG char(2) DEFAULT 'fr',
// FEMALE bool DEFAULT 0,
// FIRSTNAME char(255) NOT NULL
// );
// CREATE TABLE TMP (W char(255));
// .import boys_surnames.csv TMP
// INSERT INTO FIRSTNAMES (FEMALE, FIRSTNAME) SELECT 0, upper(W) FROM TMP;
// DELETE FROM TMP;
// .import girls_surnames.csv TMP
// INSERT INTO FIRSTNAMES (FEMALE, FIRSTNAME) SELECT 1, upper(W) FROM TMP;
// DROP TABLE TMP;

#include "randomizer.h"
#include "global.h"
#include "log.h"

#include <time.h>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QVector>
#include <QVariant>


#include <time.h>

using namespace Utils;
using namespace std;

#define makeRandDouble(max) ((double)rand() / ((double)RAND_MAX + 1) * (double)max)
#define makeRand(max)       (int)((double)rand() / ((double)RAND_MAX + 1) * (double)max)

namespace Utils {
namespace Internal {
class RandomizerPrivate
{
public:
    RandomizerPrivate() {}

    ~RandomizerPrivate()
    {
        boysFirstnames.clear();
        girlsFirstnames.clear();
    }

//    void readBoysFirstnames()
//    {
//        if (!boysFirstnames.isEmpty())
//            return;
//        QString c = Utils::readTextFile(m_Path + "/boys_surnames.csv");
//        if (c.isEmpty())
//            Utils::Log::addError("Randomizer", "Can not read boy firstnames.",
//                                 __FILE__, __LINE__);
//        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
//            boysFirstnames.append(s);
//        }
//    }

//    void readGirlsFirstnames()
//    {
//        if (!girlsFirstnames.isEmpty())
//            return;
//        QString c = Utils::readTextFile(m_Path + "/girls_surnames.csv");
//        if (c.isEmpty())
//            Utils::Log::addError("Randomizer", "Can not read boy firtsnames.",
//                                 __FILE__, __LINE__);
//        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
//            girlsFirstnames.append(s);
//        }
//    }

    void readZipCodes()
    {
        if (!zipCodes.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/zipcodes.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read zip codes.",
                                 __FILE__, __LINE__);
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            QStringList z = s.split("\t");
            if (z.count() != 2)
                continue;
            zipCodes.insert(z.at(1).toInt(), z.at(0).toUpper());
        }
    }

//    void readFrenchWords()
//    {
//        if (!words.isEmpty())
//            return;
//        QString c = Utils::readTextFile(m_Path + "/listemotsfr.txt");
//        if (c.isEmpty())
//            LOG_ERROR_FOR("Randomizer", "Can not read french words.");
//        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
//            words.append(s.toUpper());
//        }
//    }


public:
    QVector<QString> boysFirstnames;
    QVector<QString> girlsFirstnames;
    QVector<QString> words;
    QMap<int, QString> zipCodes;
    QString m_Path;
};
}
}


Randomizer::Randomizer() :
        d(0)
{
    d = new Internal::RandomizerPrivate;
    srand(time(NULL));
}

Randomizer::~Randomizer()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void Randomizer::setPathToFiles(const QString &p)
{
    d->m_Path = QDir::cleanPath(p);
}

QString Randomizer::randomFirstName(bool male) const
{
    // Get the database && open it
    QSqlDatabase db;
    if (!QSqlDatabase::connectionNames().contains("__RANDOM__FIRSTNAMES__")) {
        db = QSqlDatabase::addDatabase("QSQLITE", "__RANDOM__FIRSTNAMES__");
        db.setDatabaseName(d->m_Path + "/firstnames.db");
    } else {
        db = QSqlDatabase::database("__RANDOM__FIRSTNAMES__");
    }
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Randomizer", "Unable to connect to database: " + db.databaseName());
            return QString::null;
        }
    }
    // Request the firstname according to the sex
    int sex;
    (male) ? sex=0 : sex=1;  // female

    // Get words count
    int max = 255;
    db.transaction();
    QSqlQuery query(db);
    QString req = QString("SELECT count(ID) FROM FIRSTNAMES WHERE LANG='%1' AND FEMALE=%2;")
            .arg("fr") // TODO: manage multiple languages
            .arg(sex);
    if (query.exec(req)) {
        if (query.next()) {
            max = query.value(0).toInt() - 1;
        }
    } else {
        LOG_QUERY_ERROR_FOR("Randomizer", query);
    }
    query.finish();

    req = QString("SELECT FIRSTNAME FROM FIRSTNAMES WHERE (LANG='%1' AND FEMALE=%2) LIMIT %3,1;")
            .arg("fr") // TODO: manage multiple languages
            .arg(sex)
            .arg(randomInt(0, max));
    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toString().toUpper();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Randomizer", query);
    }
    return QString::null;
}

QString Randomizer::randomString(int length) const
{
    static char consonnes[]="BCDFGHJKLMNPQRSTVWXZ";
    static char voyelles[]="AEIOUY";

    QString s;
    int l = length;
    if (length == 0) {
        l = 5;
    }
    for (int i=0; i<l; i++)
    {
        if ((l > 3) && (makeRand(5) == 2))
            s.append(" ");
        else if ((i % 2) == 0)
            s.append(QChar(consonnes[makeRand(20)]));
        else
            s.append(QChar(voyelles[makeRand(6)]));
    }
    return s;
}

QPair<int, QString> Randomizer::randomFrenchCity() const
{
    if (d->zipCodes.isEmpty())
        d->readZipCodes();
    QPair<int, QString> p;
    int r = makeRand(d->zipCodes.count() - 1);
    p.first = d->zipCodes.keys().at(r);
    p.second = d->zipCodes.value(p.first);
    return p;
}

/** Create a random user name less than 7 characters long */
/** so that firstname.surname is less than or equal to 16 characters (MySQL limit) */
QString Randomizer::randomName() const
{
    QString randName;
    do
    {
        randName = randomWords(randomInt(1, 2));
    }while ( randName.size()>7 );
    
    return randName;
}

/** Returns a fake sentence composed of \e nbOfWords words. */
QString Randomizer::randomWords(int nbOfWords) const
{
    // Get the database && open it
    QSqlDatabase db;
    if (!QSqlDatabase::connectionNames().contains("__RANDOM__WORDS__")) {
        db = QSqlDatabase::addDatabase("QSQLITE", "__RANDOM__WORDS__");
        db.setDatabaseName(d->m_Path + "/words.db");
    } else {
        db = QSqlDatabase::database("__RANDOM__WORDS__");
    }
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Randomizer", "Unable to connect to database: " + db.databaseName());
            return QString::null;
        }
    }
    // Get words count
    int max = 255;
    db.transaction();
    QSqlQuery query(db);
    QString req = QString("SELECT max(ID) FROM WORDS WHERE LANG='%1';")
            .arg("fr"); // TODO: manage multiple languages
    if (query.exec(req)) {
        if (query.next()) {
            max = query.value(0).toInt();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Randomizer", query);
    }
    query.finish();

    // Request the words
    QStringList t;
    for(int i=0; i < nbOfWords; ++i) {
        QString req = QString("SELECT WORD FROM WORDS WHERE LANG='%1' AND ID='%2';")
                .arg("fr") // TODO: manage multiple languages
                .arg(randomInt(0, max));
        if (query.exec(req)) {
            if (query.next()) {
                QString val = query.value(0).toString().toUpper();
                // Remove all non-word chars to avoid assert in qascii....cpp when using QTest::keyClicks
                t << val.replace(QRegExp("\\W"), "_");
            }
        } else {
            LOG_QUERY_ERROR_FOR("Randomizer", query);
        }
    }
    db.commit();
    return t.join(" ");
}

/** Returns a randomly created integer lower thant \e max */
int Randomizer::randomInt(int max) const
{
    return makeRand(max);
}

/** Returns a randomly created boolean */
bool Randomizer::randomBool() const
{
    int z = 0;
    for(int i=0; i < randomInt(1, 50); ++i) {
        z += randomInt(0,1);
    }
    return (z%1==0);
}

int Randomizer::randomInt(int min, int max) const
{
    Q_ASSERT_X(min <= max, "Utils::Randomizer", "min > max");
    if (min == max)
        return min;
    int i = min - 10;
    int z = 0;
    while (i < min) {
        i = makeRand(max);
        if (++z == 20) {
            i = max;
            break;
        }
    }
    return i;
}

double Randomizer::randomDouble(double min, double max) const
{
    Q_ASSERT(min < max);
    double i = min - 10.;
    int z = 0;
    while (i < min) {

//        qWarning() << (double)rand() / (double)(RAND_MAX + 1)
//                      << ((double)rand() / ((double)RAND_MAX + 1) * (double)max);

        i = makeRandDouble(max);
        if (++z == 20) {
            i = max;
            break;
        }
    }
    return i;
}

// TODO: code qlonglong Randomizer::randomLongLongInt(qlonglong min, qlonglong max)
//qlonglong Randomizer::randomLongLongInt(qlonglong min, qlonglong max) const
//{
//    Q_ASSERT(min < max);
//    qlonglong i = min - 10;
//    qlonglong z = 0;
//    while (i < min) {
//        i = makeRand(max);
//        if (++z == 20) {
//            i = max;
//            break;
//        }
//    }
//    return i;
//}

QDate Randomizer::randomDate(const int minYear, const int minMonth, const int minDay) const
{
    int r = -1;
    QDate toReturn(minYear, minMonth, minDay);
    int days = toReturn.daysTo(QDate::currentDate());
    if (randomInt(1, 7) > 5)
        days = days / 2;
    if (days > 1) {
        int i = 0;
        while (r < 1) {
            r = randomInt(days);
            if (++i == 20)
                break;
        }
    } else {
        return toReturn.addDays(1);
    }
    return toReturn.addDays(r);
}

/**
 * Generate a random QDateTime >= to \e minDateTime.
 * Note: time is only generated for hours, minutes and seconds (to improve
 * compatibility with Qt::ISODate format specially for testings).
 */
QDateTime Randomizer::randomDateTime(const QDateTime &minDateTime) const
{
    QDateTime toReturn(randomDate(minDateTime.date().year(), minDateTime.date().month(), minDateTime.date().day()));
    if (toReturn.date() == minDateTime.date()) {
        int i = 0;
        while (toReturn < minDateTime) {
            int r = 0;
            int j = 0;
            while (r < 1) {
                r = randomInt(23452634);
                if (++j == 20) {
                    r = i + 1;
                    break;
                }
            }

            if (toReturn.addMSecs(r).isValid() && ++i == 20)
                break;
        }
    } else {
        toReturn.setTime(QTime(randomInt(23), randomInt(59), randomInt(59)));
    }
    return toReturn;
}

QTime Randomizer::randomTime(const int minHour, const int maxHour) const
{
    int h = randomInt(minHour, maxHour);
    int m = randomInt(0, 59);
    int s = randomInt(0, 59);
    return QTime(h,m,s);
}

QFileInfo Randomizer::randomFile(const QDir &inDir, const QStringList &filters) const
{
    QFileInfoList list = inDir.entryInfoList(filters);
    return list.at(randomInt(0, list.count()));
}

/** Returns a randomly created version number 100% compatible with Utils::VersionNumber */
QString Randomizer::randomVersionNumber() const
{
    QString v = QString("%1.%2.%3").arg(randomInt(0, 100)).arg(randomInt(0, 100)).arg(randomInt(0, 100));
    // Add alpha/beta/RC
    if (randomBool()) {
        switch (randomInt(0, 3)) {
        case 0: v += QString("~alpha%1").arg(randomInt(0, 100)); break;
        case 1: v += QString("~beta%1").arg(randomInt(0, 100)); break;
        case 2: v += QString("~RC%1").arg(randomInt(0, 100)); break;
        default: break;
        }
    }
    return v;
}


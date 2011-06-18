/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Utils::Randomizer
  Randomizer can be used for populating database with random: names, firstnames, zipcodes and cities, strings, numbers, dates...
*/

#include "randomizer.h"
#include "global.h"
#include "log.h"

#include <time.h>

#include <QFile>
#include <QVector>

#include <time.h>

using namespace Utils;
using namespace std;

#define makeRand(max) (int)((double)rand() / ((double)RAND_MAX + 1) * max)

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

    void readBoysFirstnames()
    {
        if (!boysFirstnames.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/boys_surnames.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read boy firstnames.",
                                 __FILE__, __LINE__);
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            boysFirstnames.append(s);
        }
    }

    void readGirlsFirstnames()
    {
        if (!girlsFirstnames.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/girls_surnames.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read boy firtsnames.",
                                 __FILE__, __LINE__);
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            girlsFirstnames.append(s);
        }
    }

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

    void readFrenchWords()
    {
        if (!words.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/listemotsfr.txt");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read french words.",
                                 __FILE__, __LINE__);
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            words.append(s.toUpper());
        }
    }


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
    d->m_Path = p;
}

QString Randomizer::getRandomFirstname(bool male)
{
    if (d->boysFirstnames.isEmpty())
        d->readBoysFirstnames();
    if (d->girlsFirstnames.isEmpty())
        d->readGirlsFirstnames();
    if (male)
        return d->boysFirstnames.at(makeRand(d->boysFirstnames.size() - 1));
    return d->girlsFirstnames.at(makeRand(d->girlsFirstnames.size() - 1));
}

QString Randomizer::getRandomString(int length)
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

QPair<int, QString> Randomizer::getRandomFrenchCity()
{
    if (d->zipCodes.isEmpty())
        d->readZipCodes();
    QPair<int, QString> p;
    int r = makeRand(d->zipCodes.count() - 1);
    p.first = d->zipCodes.keys().at(r);
    p.second = d->zipCodes.value(p.first);
    return p;
}

QString Randomizer::getRandomName()
{
    if (d->words.isEmpty())
        d->readFrenchWords();
    return d->words.at(makeRand(d->words.count() - 1));
}

QString Randomizer::randomWords(int nbOfWords)
{
    QStringList t;
    for(int i=0; i < nbOfWords; ++i) {
        t <<  getRandomName();
    }
    return t.join(" ");
}

int Randomizer::randomInt(int max)
{
    return makeRand(max);
}

bool Randomizer::randomBool()
{
    int z = 0;
    for(int i=0; i < randomInt(1, 50); ++i) {
        z += randomInt(0,1);
    }
    return (z%1==0);
}

int Randomizer::randomInt(int min, int max)
{
    Q_ASSERT(min < max);
    int i = min;
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

QDate Randomizer::randomDate(const int minYear, const int minMonth, const int minDay)
{
    int r = -1;
    QDate toReturn(minYear, minMonth, minDay);
    int i = 0;
    int days = toReturn.daysTo(QDate::currentDate());
    if (randomInt(1, 7) > 5) {
        days = days / 2;
    }
    if (days > 1) {
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

QDateTime Randomizer::randomDateTime(const QDateTime &minDate)
{
    QDateTime toReturn(randomDate(minDate.date().year(), minDate.date().month(), minDate.date().day()));
    if (toReturn.date() == minDate.date()) {
        int i = 0;
        while (toReturn < minDate) {
            int r = 0;
            int j = 0;
            while (r < 1) {
                r = randomInt(23452634);
                if (++j == 20) {
                    r = i + 1;
                    break;
                }
            }
            toReturn.addMSecs(r);
            if (++i == 20)
                break;
        }
    } else {
        toReturn.setTime(QTime(randomInt(23), randomInt(59), randomInt(59), randomInt(99)));
    }
    return toReturn;
}

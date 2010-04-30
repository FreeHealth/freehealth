/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
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
        boysSurnames.clear();
        girlsSurnames.clear();
    }

    void readBoysSurnames()
    {
        if (!boysSurnames.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/boys_surnames.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read boy surnames.");
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            boysSurnames.append(s);
        }
    }

    void readGirlsSurnames()
    {
        if (!girlsSurnames.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/girls_surnames.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read boy surnames.");
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            girlsSurnames.append(s);
        }
    }

    void readZipCodes()
    {
        if (!zipCodes.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/zipcodes.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read zip codes.");
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
            Utils::Log::addError("Randomizer", "Can not read french words.");
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            words.append(s.toUpper());
        }
    }


public:
    QVector<QString> boysSurnames;
    QVector<QString> girlsSurnames;
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

QString Randomizer::getRandomSurname(bool male)
{
    if (d->boysSurnames.isEmpty())
        d->readBoysSurnames();
    if (d->girlsSurnames.isEmpty())
        d->readGirlsSurnames();
    if (male)
        return d->boysSurnames.at(makeRand(d->boysSurnames.size() - 1));
    return d->girlsSurnames.at(makeRand(d->girlsSurnames.size() - 1));
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

int Randomizer::randomInt(int max)
{
    return makeRand(max);
}

QDate Randomizer::randomDate(int minYear)
{
    int r = -1;
    while (r < 0)
        r = makeRand(QDate::currentDate().year() - minYear);
    int year = minYear + r;
    return QDate(year, makeRand(12), makeRand(28));
}


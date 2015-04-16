/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <utils/global_exporter.h>

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QPair>
#include <QDate>

/**
 * \file randomizer.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 19 Jun 2011
*/

namespace Utils {
namespace Internal {
class RandomizerPrivate;
}

class UTILS_EXPORT Randomizer
{
public:
    Randomizer();
    ~Randomizer();

    void setPathToFiles(const QString &path);

    QString randomString(int length) const;

    QString randomName() const;
    QString randomFirstName(bool male) const;

    QPair<int, QString> randomFrenchCity() const;

    int randomInt(int max) const;
    int randomInt(int min, int max) const;
//    qlonglong randomLongLongInt(qlonglong min, qlonglong max) const;
    double randomDouble(double min, double max) const;
    bool randomBool() const;

    QString randomWords(int nbOfWords) const;

    QDate randomDate(const int minYear, const int minMonth = 1, const int minDay = 1) const;
    QDateTime randomDateTime(const QDateTime &minDateTime) const;
    QTime randomTime(const int minHour, const int maxHour) const;

    QFileInfo randomFile(const QDir &inDir, const QStringList &filters) const;

    QString randomVersionNumber() const;

private:
    Internal::RandomizerPrivate *d;
};


} // End namespace Utils

#endif // RANDOMIZER_H

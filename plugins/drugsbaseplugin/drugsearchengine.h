/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSEARCHENGINE_H
#define DRUGSEARCHENGINE_H

#include <translationutils/constanttranslations.h>
#include <drugsbaseplugin/drugsbase_exporter.h>
#include <QString>

/**
 * \file drugsearchengine.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 23 Fev 2011
*/


namespace DrugsDB {
class IDrug;
namespace Internal {
class DrugSearchEnginePrivate;

class DRUGSBASE_EXPORT DrugSearchEngine
{
    DrugSearchEngine();
public:
    static DrugSearchEngine *instance();
    ~DrugSearchEngine();

    void clear();

    void addNewEngine(const QString &label, const QString &url, const QString &lang = Trans::Constants::ALL_LANGUAGE);

    void setDrug(const IDrug *drug);

    QStringList processedLabels(const QString &lang = Trans::Constants::ALL_LANGUAGE) const;
    QStringList processedUrls(const QString &label, const QString &lang = Trans::Constants::ALL_LANGUAGE) const;

    int numberOfEngines() const;

private:
    DrugSearchEnginePrivate *d;
    static DrugSearchEngine *m_Instance;
};

}  // End namespace Internal
}  // End namespace DrugsDB

#endif // DRUGSEARCHENGINE_H

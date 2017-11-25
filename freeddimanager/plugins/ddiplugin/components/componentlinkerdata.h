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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_COMPONENTLINKERDATA_H
#define DDIMANAGER_DDIPLUGIN_COMPONENTLINKERDATA_H

#include <ddiplugin/ddi_exporter.h>

#include <QStringList>
#include <QHash>
#include <QMultiHash>
#include <QDebug>

/**
 * \file componentlinkerdata.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 08 Feb 2014
*/

namespace DrugsDB {
namespace Internal {
class DrugBaseEssentials;
}
}

namespace DDI {
class ComponentAtcModel;

class DDI_EXPORT ComponentLinkerData
{
    friend class DDI::ComponentAtcModel;

public:
    /**
     * DDI::ComponentAtcModel needs data to link components with interactors.
     * This class is a container for these data.
     * \sa DDI::ComponentAtcModel::startComponentLinkage()
     */
    ComponentLinkerData() :
        lang("fr")
    {}

    ~ComponentLinkerData()
    {}

    // In setData
    /**
     * Define the language to use for Component to ATC label matching (eg: fr, en, de, sp). \n
     * This data must be defined before the DDI::ComponentModel will try to link drugs
     * components to drug interactors / ATC codes. \n
     * By default, the language is defined to \e french.
    */
    void setAtcLanguage(const QString &_lang) {lang = _lang;}

    /**
     * You must define the database identifiants for the \e drugs \e components. \n
     * Key is the \e name of the component, value its database Id. All names and
     * codes must be uppercase. \n
     * This data must be defined before the DDI::ComponentModel will try to
     * link drugs components to drug interactors / ATC codes.
     */
    void setComponentIds(const QHash<QString, int> &_compoIds) {compoIds = _compoIds;}

    /**
     * You must define the database identifiants for the \e ATC \e code. \n
     * This data must be defined before the DDI::ComponentModel will try to
     * link drugs components to drug interactors / ATC codes.
     */
    void setAtcCodeIds(const QHash<QString, int> &_atcCodeIds) {atcCodeIds = _atcCodeIds;}

    /**
     * If some components are equivalent (same molecule with a different name), you
     * can add the equivalence. It will be automatically managed.
     */
    void addComponentEquivalence(int id1, int id2)
    {
        // Internally all equivalences are stored non-mirrored
        if (_equivalences.values(id1).contains(id2))
            return;
        if (_equivalences.values(id2).contains(id1))
            return;
        _equivalences.insertMulti(id1, id2);
    }

    QList<int> equivalences(const int compoId) const
    {
        QList<int> eq = _equivalences.values(compoId);
        eq << _equivalences.keys(compoId);
        return eq;
    }

    QString debugEquivalences() const
    {
        QString r;
        const QList<int> &ids = _equivalences.uniqueKeys();
        for(int i = 0; i < ids.count(); ++i) {
            int id = ids.at(i);
            QList<int> eq = equivalences(id);
            QStringList lbl;
            for(int j = 0; j < eq.count(); ++j) {
                lbl << compoIds.key(eq.at(j));
            }
            r += QString("%1 = %2\n").arg(compoIds.key(id)).arg(lbl.join(" ; "));
            qWarning() << id << eq << "\n" << QString("%1 = %2\n").arg(compoIds.key(id)).arg(lbl.join(" ; "));
        }
        return r;
    }

protected:
    // In data
    QString lang;
    QHash<QString, int> compoIds;
    QHash<QString, int> atcCodeIds;
    QMultiHash<int, int> _equivalences;
};

class DDI_EXPORT ComponentLinkerResult
{
public:
    /**
     * DDI::ComponentAtcModel manages components with interactors linking.
     * This class is a container for the processing results.
     * \sa DDI::ComponentAtcModel::startComponentLinkage()
     */
    ComponentLinkerResult() :
        _completionPercentage(0.0)
    {}

    virtual ~ComponentLinkerResult()
    {}

    /** Returns the list of error messages */
    const QStringList errors() const {return _errors;}

    /** Returns the list of processing messages */
    const QStringList messages() const {return _msg;}

    /**
     * Returns the component id to ATC id links.
     * Key represents the \e component \e id, while the value is the \e ATC \e id according
     * to the DDI::ComponentLinkerData data.
     * \sa DDI::ComponentLinkerData::setAtcCodeIds(), DDI::ComponentLinkerData::setComponentIds()
     */
    const QMultiHash<int, int> componentIdToAtcId() const {return compoIdToAtcId;}

    /**
     * Returns true if the result already contains linking data for the specific
     * drug component ID.
     */
    bool containsComponentId(const int componentId) const {return compoIdToAtcId.uniqueKeys().contains(componentId);}

    /**
     * Returns the completion level of the linking processus. Is the percentage of
     * component linked to a drug interactor / ATC code.
     */
    double completionPercentage() const {return _completionPercentage;}

protected:
    QStringList unfoundComponentsAssociation, _errors, _msg;
    double _completionPercentage;
    QMultiHash<int, int> compoIdToAtcId; // Key: moleculeId, Values: AtcIds
};

} // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_COMPONENTLINKERDATA_H

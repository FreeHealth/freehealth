/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DRUGSINTERACTION_H
#define DRUGSINTERACTION_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>

// include Qt headers
#include <QIcon>
#include <QVariant>
#include <QObject>

/**
 * \file drugsinteraction.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 25 Oct 2009
*/

/** \todo Some parts should not be Internals */


namespace DrugsDB {
namespace Internal {
class DrugsData;
class InteractionsBase;

class DRUGSBASE_EXPORT DrugsInteraction
{
    friend class InteractionsBase;
    friend class InteractionsBasePrivate;
public:
    enum DataRepresentation {
        DI_Id = 0,
        DI_LinkId,
        DI_Type,
        DI_ATC1,
        DI_ATC2,
        DI_ATC1_Label,
        DI_ATC2_Label,
        DI_Risk,
        DI_Management,

        // only used for test
        DI_RiskFr,
        DI_RiskEn,
        DI_ManagementFr,
        DI_ManagementEn,
        DI_ReferencesLink
    };


    DrugsInteraction() {}
    ~DrugsInteraction() {}

    static QString typeToString( const int t );

    // getters
    QList<DrugsData *> drugs() const;
    QVariant value( const int fieldref ) const;
    QString typeOfIAM( const int & t ) const;
    Constants::Interaction::TypesOfIAM type() const;
    QString header() const;
    QString risk() const;
    QString management() const;
    QString referencesLink() const;

    static bool lessThan(const DrugsInteraction *int1, const DrugsInteraction *int2);

    // viewers
    void warn() const;


protected:
    // setters
    void setValue( const int fieldref, const QVariant & value ); // ajouter CIS1 CIS2
    void addInteractingDrug(DrugsData *drug)
    {
        Q_ASSERT(drug);
        if (!m_InteractingDrugs.contains(drug))
            m_InteractingDrugs << drug;
    }

private:
    QHash<int, QVariant> m_Infos;
    QList<DrugsData *> m_InteractingDrugs;
};

}  // End Internal
}  // End DrugsDB

#endif  // DRUGSINTERACTION_H

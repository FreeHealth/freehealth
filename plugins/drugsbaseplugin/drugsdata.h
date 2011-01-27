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
#ifndef DRUGSDATA_H
#define DRUGSDATA_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>

// include Qt headers
#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QDate>
#include <QSet>
#include <QDebug>

/**
 * \file drugdata.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 23 Sept 2010
*/

/** \todo Some parts should not be Internals */


namespace DrugsDB {
namespace Internal {
class DrugsDataPrivate;
class DrugCompositionPrivate;
class DrugsBase;
class DrugsBasePrivate;

class DrugComposition
{
public:
    DrugComposition();
    ~DrugComposition();

    void setValue(const int fieldref, const QVariant &value);
    void setLinkedSubstance(DrugComposition *link);
    void setInnCode(const int code);
    int linkId() const;
    bool isLinkedWith(DrugComposition *link) const;
    bool isTheActiveSubstance() const;
    int codeMolecule() const;
    int innCode() const;

    QString innName() const;
    QStringList iamClasses() const;
    QString innDosage() const;
    QString dosage() const;
    QString form() const;
    QString moleculeName() const;
    QString nature() const;
    int lkNature() const;
    QString warnText() const;

public:
    DrugCompositionPrivate *d;
    DrugComposition *m_Link;
    QString m_InnName;
    mutable QString m_Dosage;
};


using namespace DrugsDB::Constants;

class DRUGSBASE_EXPORT DrugsData
{
    friend class DrugsBase;
    friend class DrugsBasePrivate;

public:
     DrugsData();
     ~DrugsData();

     // setters
     void setPrescriptionValue( const int fieldref, const QVariant &value );

     // getters
     QVariant          UID() const                { return value(Table_DRUGS, DRUGS_UID);  }
     QList<QVariant>   CIPs() const;
     QStringList       CIPsDenominations() const;
     QString           denomination() const;
     QString           ATC() const                { return value( Table_DRUGS, DRUGS_ATC ).toString(); }
     QString           form() const               { return value( Table_DRUGS, DRUGS_FORM ).toString(); }
//     QString           route() const              { return value( Table_DRUGS, DRUGS_ROUTE ).toString(); }
     QString           strength() const           { return value( Table_DRUGS, DRUGS_STRENGTH ).toString(); }

     QStringList routes() const;

     int               numberOfCodeMolecules() const { return listOfMolecules().count(); }
     QStringList       listOfMolecules() const;
     QList<int>        listOfCodeMolecules() const;

     QStringList       listOfInn() const;
     int               numberOfInn() const;
     QStringList       listOfInnClasses() const;
     QSet<int>         allInnAndIamClasses() const;
     QSet<int>         allSevenCharsAtcIds() const;
     QStringList       allSevenCharsAtcCodes() const;
     QSet<int>         allAtcIds() const;
     QStringList       allAtcCodes() const;
     int               mainInnCode() const;
     QString           mainInnName() const;
     QString           mainInnDosage() const;
     QString           innComposition() const;

     QString           linkToSCP() const;

     QStringList       dosageOfMolecules() const;
     bool              hasPrescription() const;
     bool              isScoredTablet() const;

     QVariant          prescriptionValue(const int filedref) const;

     QString           compositionToXml();

     // this need to be rewritten because there is N CIP for 1 drug
     //const int CIP()                    { return value( mfDrugsTables::PACK_CIP ).toInt(); }

     // viewers
     QString toHtml() const;
     static QString drugsListToHtml( const QList<DrugsData*> & list );

     void warn() const;
     QString warnText() const;
     void smallDrugWarn() const;

     // sorters
     static bool lessThan(const DrugsData *drug1, const DrugsData *drug2);

protected:
     // setters
     void setValue(const int tableref, const int fieldref, const QVariant & value);
     void addInnAndIamClasses(const QSet<int> &codes);
     void addCIP(const int CIP, const QString & denomination, QDate date = QDate());
     void addComposition(DrugComposition *compo);
     void addRoute(const int routeId, const QString &lang, const QString &label);

     // getters
     QVariant value( const int tableref, const int fieldref ) const;

private:
     DrugsDataPrivate *d;
};


class DRUGSBASE_EXPORT TextualDrugsData : public DrugsData
{
public:
    TextualDrugsData();
    void setDenomination(const QString &denomination);
};

}  // End Internal
}  // End DrugsDB

// Q_DECLARE_METATYPE( DrugsDB::DrugsData )
typedef QList<DrugsDB::Internal::DrugsData*> QDrugsList;

#endif  // DRUGSDATA_H

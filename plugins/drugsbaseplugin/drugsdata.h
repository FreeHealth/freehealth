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
class DrugsBase;
class DrugsBasePrivate;

class DrugComposition
{
public:
    DrugComposition() : m_InnCode(-1), m_LinkId(-1), m_Link(0) {}
    ~DrugComposition() {} // Don't delete m_Link

    /** \brief Feed values from the database */
    void setValue(const int fieldref, const QVariant &value )
    {
        using namespace DrugsDB::Constants;
        switch (fieldref)
        {
            case COMPO_MOL_FORM : m_Form = value.toString(); break;
            case COMPO_MOL_NAME : m_MoleculeName = value.toString(); break;
            case COMPO_MOL_CODE : m_CodeMolecule = value.toInt(); break;
            case COMPO_DOSAGE : m_Dosage = value.toString(); break;
            case COMPO_REF_DOSAGE : m_RefDosage = value.toString(); break;
            case COMPO_NATURE : m_Nature = value.toString(); break;
            case COMPO_LK_NATURE: m_LinkId = value.toInt(); break;
            case COMPO_IAM_DENOMINATION: m_InnName = value.toString(); break;
            case COMPO_IAM_CLASS_DENOMINATION : m_IamClass = value.toStringList(); break;
            default : break;
        }
    }

    /** \brief Set the linked mfDrugComposition (this happens when a molecule is transform to another one which is the active one */
    void setLinkedSubstance( DrugComposition *link )
    {
        Q_ASSERT(link);
        m_Link = link;
        link->m_Link = this;
    }

    void setInnCode(const int code)
    {
        m_InnCode = code;
    }

    int linkId() const
    {
        return m_LinkId;
    }

    /** \brief Test link relation with the \e link */
    bool isLinkedWith( DrugComposition *link ) const
    {
        Q_ASSERT(link);
        return (link==m_Link);
    }

    /** \brief Returns composition is the active substance ? */
    bool isTheActiveSubstance() const
    {
        if (m_Nature=="FT")
            return true;
        if (!m_Link)
            return true;
        return false;//(!m_Link->isTheActiveSubstance());  // take care to infinite looping...
    }

    /** \brief Return the INN of the molecule. Check the linked composition for the inn name. */
    QString innName() const
    {
        if (this->isTheActiveSubstance())
            return m_InnName;
        else if (m_Link)
                return m_Link->m_InnName; // avoid infinite loop by retreiving value directly not with the function of m_Link
        return QString();
    }


    /** \brief Returns the iam classes names */
    QStringList iamClasses() const
    {
        return m_IamClass;
    }

    /** \brief Return the corrected dosage of the INN */
    QString innDosage() const
    {
        QString tmp;
        if (this->isTheActiveSubstance())
            tmp = m_Dosage;
        else if (m_Link)
            tmp = m_Link->m_Dosage; // avoid infinite loop by retreiving value directly not with the function of m_Link
        // do some transformations
        if (!tmp.isEmpty()) {
            tmp.replace(",000","");
            tmp.replace(",00","");
        }
        // set the transformed dosage for the next call
        m_Dosage = tmp;
        return tmp;
    }

    /** \brief Return the dosage of the molecule */
    QString dosage() const
    {
        return m_Dosage;
    }

    QString warnText() const
    {
        QString tmp;
        tmp += "Composition : " + m_MoleculeName
               + "\n  Form : " + m_Form + "\n  inn : " + m_InnName +  "\n  dosage : " + m_Dosage
               + "\n  refDosagase : " + m_RefDosage + "\n  nature : " + m_Nature;
        if (m_Link)
            tmp += "\n  Linked";
        tmp += "\n    innName() : " + innName() + "\n    innDosage() : " + innDosage();
        tmp += "\n    iamClasses() : " + iamClasses().join("; ");
        return tmp + "\n";
    }

public:
    QString m_MoleculeName;
    QString m_InnName;
    QStringList m_IamClass;
    int m_CodeMolecule;
    int m_InnCode;
    QString m_Form;       // NOM
    mutable QString m_Dosage;
    QString m_RefDosage;
    QString m_Nature;     // SA / FT
    int m_LinkId;
    DrugComposition *m_Link;
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
     QString           route() const              { return value( Table_DRUGS, DRUGS_ROUTE ).toString(); }
     QString           strength() const              { return value( Table_DRUGS, DRUGS_STRENGTH ).toString(); }


     int               numberOfCodeMolecules() const { return listOfMolecules().count(); }
     QStringList       listOfMolecules() const;
     QList<int>        listOfCodeMolecules() const;

     QStringList       listOfInn() const;
     int               numberOfInn() const        { return allInnAndIamClasses().count(); }
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

     // this need to be rewritten because there is N CIP for 1 drug
     //const int CIP()                    { return value( mfDrugsTables::PACK_CIP ).toInt(); }

     // viewers
     QString toHtml() const;
     static QString drugsListToHtml( const QList<DrugsData*> & list );

     void warn() const;
     QString warnText() const;
     void smallDrugWarn() const;

     // sorters
     static bool lessThan( const DrugsData *drug1, const DrugsData *drug2 );

protected:
     // setters
     void setValue( const int tableref, const int fieldref, const QVariant & value );
     void addInnAndIamClasses( const QSet<int> &codes );
     void addCIP( const int CIP, const QString & denomination, QDate date = QDate() );
     void addComposition( DrugComposition *compo );

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

/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class DrugsData
  \brief This class owns the getters, setters and viewers of drugs.


  \todo Write documentation
  \todo Actually inside FMF and drugwidgetplugins mfDrugs are carried via a QList of pointers. This way we can not
  use the operators with mfDrugs. May be we must redefine QList of pointers in QList of references ?
 \ingroup freediams drugswidget
*/

#include "drugsdata.h"

// include drugswidget headers
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/drugsinteraction.h>
#include <drugsbaseplugin/interactionsmanager.h>

//#include <drugsmodel/mfDosageModel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <QApplication>
#include <QHash>
#include <QMultiHash>
#include <QDebug>
#include <QLocale>

using namespace DrugsDB::Constants;

using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

static const char* const FRENCH_RPC_LINK = "http://afssaps-prd.afssaps.fr/php/ecodex/rcp/R%1.htm"; // 2+2+3
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

//--------------------------------------------------------------------------------------------------------
//------------------------------- mfDrugPrivate constructor / destructor ---------------------------------
//--------------------------------------------------------------------------------------------------------
namespace DrugsDB {
namespace Internal {
class DrugsDataPrivate
{
public:
    struct structCIP {
        int      CIP;
        QString  denomination;
        QDate    date;
    };

    DrugsDataPrivate()
    {
        m_CISValues.reserve(DRUGS_MaxParam);
        m_PrescriptionValues.reserve(Prescription::MaxParam - Prescription::Id);
    }

    ~DrugsDataPrivate()
    {
        qDeleteAll(m_Compositions);
        m_Compositions.clear();
    }

    QString prescriptionToText( bool toHtml );
    bool usesFromTo( const QString &from, QString &to, const int prescriptionIndex );


    QList<DrugComposition *>     m_Compositions;

    QList<structCIP>             m_CIPs;
    QHash< int, QVariant>        m_CISValues;
    QMultiHash< int, QVariant>   m_COMPOValues;
    QHash< int, QVariant>        m_PrescriptionValues;
    QString                      m_NoLaboDenomination;
    QString                      m_LastHtmlPrescription, m_LastTxtPrescription;   // Cache
    bool                         m_PrescriptionChanges;
    QSet<int>                    m_IamCodes;                 /*!< \brief Stores all Inn and IamClasses codes of this drug. */
};
}
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------- mfDrug constructor / destructor ------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Constructor. All created instance are deleted by DrugsDB::Internal::DrugsModel. */
DrugsData::DrugsData()
    : d(new DrugsDataPrivate())
{
    d->m_NoLaboDenomination.clear();
    setPrescriptionValue(Prescription::IsTextualOnly, false);
    d->m_PrescriptionChanges = true;
}

DrugsData::~DrugsData()
{
    if (d) delete d;
    d=0;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Setters -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \brief Stores \e value of drugs referenced by its source table index /e tableref, source field \e fieldref.
  \sa mfDrugsBase::getDrugByCIS()
  \sa mfDrugsConstants
*/
void DrugsData::setValue( const int tableref, const int fieldref, const QVariant & value )
{
    switch(tableref)
    {
    case Table_DRUGS : d->m_CISValues[fieldref] = value; break;
    case Table_COMPO :
        {
            if ( fieldref == COMPO_IAM_DENOMINATION )
                d->m_COMPOValues.insertMulti( fieldref, value );
            else if ( fieldref == COMPO_IAM_CLASS_DENOMINATION )
                d->m_COMPOValues.insert( fieldref, value );
            else if ( !d->m_COMPOValues.values( fieldref ).contains( value ) )
                d->m_COMPOValues.insertMulti( fieldref, value );
            break;
        }
    default: Utils::Log::addError("DrugsData", "Wrong table reference parameter in DrugsData::setValue()."); break;
    }
}

/** \brief Add a composition class description to this drug */
void DrugsData::addComposition(DrugComposition *compo)
{
    d->m_Compositions.append(compo);
}

void DrugsData::addCIP( const int CIP, const QString &denomination, QDate date )
{
    /** \todo one drug can have multi presentations ==> list */
    DrugsDataPrivate::structCIP st;
    st.CIP = CIP;
    st.denomination = denomination;
    st.date = date;
    d->m_CIPs.append( st );
}

/** \brief Stores a new INN and IamClasses codes for this drug assuming no doublons. Only used by mfDrugsBase::getDrugByCIS(). */
void DrugsData::addInnAndIamClasses( const QSet<int> &codes )
{
    foreach( const int i, codes) {
        if (d->m_IamCodes.contains(i))
           continue;
        d->m_IamCodes << i;
    }
}

/**
  \brief Stores \e value of drugs' prescription source field \e fieldref.
  \sa mfDrugsModel mfDosageDialog
*/
void DrugsData::setPrescriptionValue( const int fieldref, const QVariant & value )
{
    if (d->m_PrescriptionValues.value(fieldref) != value) {
        d->m_PrescriptionChanges = true;
        d->m_PrescriptionValues[fieldref] = value;
    }
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Getters -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \brief Return the drugs' value referenced by its table and field reference : \e tableref, \e fieldref.
  \sa mfDrugsConstants
*/
QVariant DrugsData::value( const int tableref, const int fieldref ) const
{
    switch (tableref)
    {
    case Table_DRUGS :
        {
            if ( d->m_CISValues.contains( fieldref ) )
                return d->m_CISValues.value(fieldref);
            return QVariant();
        }
    case Table_COMPO :
        {
            if ( d->m_COMPOValues.contains( fieldref ) ) {
                if ( fieldref == COMPO_IAM_CLASS_DENOMINATION )
                    return d->m_COMPOValues.value( fieldref );
            }
            else {
                return d->m_COMPOValues.values( fieldref );
            }
            return QVariant();
        }
    default: Utils::Log::addError("DrugsData", "Wrong table reference parameter in DrugsData::value()."); break;
    }
    return QVariant();
}

/**
  \brief Return true if drugs has a prescription.
  \sa mfDrugsConstants::Prescription
*/
bool DrugsData::hasPrescription() const
{
    // TODO this needs improvments +++
    int i =0;
    foreach( const QVariant &q , d->m_PrescriptionValues ) {
        if (!q.isNull())
            i++;
    }
    return i > 2;
}

/**
  \brief Return the prescription value referenced by its field \e fieldref.
  \sa mfDrugsConstants::Prescription
*/
QVariant DrugsData::prescriptionValue( const int fieldref ) const
{
    // manage some particularities
    switch (fieldref)
    {
        case Prescription::IntakesTo :
        {
            if (!d->m_PrescriptionValues.value(Prescription::IntakesUsesFromTo,false).toBool())
                return QVariant();
            break;
        }
        case Prescription::DurationTo :
        {
            if (!d->m_PrescriptionValues.value(Prescription::DurationUsesFromTo,false).toBool())
                return QVariant();
            break;
        }
    }
    return d->m_PrescriptionValues.value(fieldref);
}

/**
  \brief Return the drug denomination with or without pharmaceutical firms name
  \sa DrugsDB::Constants::S_HIDELABORATORY
*/
QString DrugsData::denomination() const
{
    if (settings()->value(Constants::S_HIDELABORATORY).toBool()) {
        if (d->m_NoLaboDenomination.isEmpty()) {
            d->m_NoLaboDenomination = value(Constants::Table_DRUGS, Constants::DRUGS_NAME).toString();
            foreach(const QString &name, LABOS) {
                if (d->m_NoLaboDenomination.contains(" " + name + " ")) {
                    d->m_NoLaboDenomination.remove(" " + name + " ");
                    return d->m_NoLaboDenomination;
                }
            }
        }
        return d->m_NoLaboDenomination;
    }
    return value(Constants::Table_DRUGS,Constants::DRUGS_NAME ).toString();
}

/** \brief Returns the list of all the molecules' code of the drug composition */
QVariant DrugsData::listOfCodeMolecules() const
{
    QVariantList tmp;
    foreach(DrugComposition *compo, d->m_Compositions)
        tmp << compo->m_CodeMolecule;
    return tmp;
}

/** \brief Returns the list of all IAM classes' names */
QStringList DrugsData::listOfInnClasses() const
{
    QStringList tmp;
    foreach(int i, this->allInnAndIamClasses())
        if (i < 999)
            tmp << DrugsBase::instance()->getInnDenomination(i);
    return tmp;
}

/** \brief Returns the list of the molecules' name */
QStringList DrugsData::listOfMolecules() const
{
    QStringList toReturn;
    foreach(const DrugComposition *compo, d->m_Compositions)
        toReturn << compo->m_MoleculeName;
    return toReturn;
}

/** \brief Return the list of all unique known Inn and Iam Classes names into a QStringList. */
QStringList DrugsData::listOfInn() const
{
    // return the list of distinct know INN of this drug
    QStringList toReturn;
    QString tmp = "";
    foreach(DrugComposition *compo, d->m_Compositions) {
        if (tmp != compo->innName()) {
            toReturn << compo->innName();
            tmp = compo->innName();
        }
    }
//    qWarning() << toReturn;
    return toReturn;
}

/** \brief Returns all Inn and IamClasses codes knwon for this drug. */
QSet<int> DrugsData::allInnAndIamClasses() const
{
    return d->m_IamCodes;
}

/** \brief Returns all Inn and IamClasses codes knwon for this drug. */
int DrugsData::mainInnCode() const
{
    QSet<int> list;
    foreach(int i, d->m_IamCodes) {
        if (i>999)
            list << i;
    }
    if (list.count() == 1)
        return *list.begin();
    return -1;
}

/** \brief Returns the main Inn for this drug. If there is more than one inn or no inn, it returns a null string.*/
QString DrugsData::mainInnName() const
{
    return DrugsBase::instance()->getInnDenomination(mainInnCode());
}

/**
  \brief Return the main dosage of the main inn
  \sa mainInnCode(), mainInnName()
*/
QString DrugsData::mainInnDosage() const
{
    QString toReturn;
    int main = mainInnCode();
    if (main != -1) {
        foreach( DrugComposition *compo, d->m_Compositions )
            if ((compo->m_InnCode==main) && (compo->isTheActiveSubstance()))
                return compo->innDosage();
    }
    return QString();
}

//QStringList DrugsData::listOfInnWithPosology() const
//{
//    /** \todo add dosage of molecules */
//    QStringList toReturn;
//    QString tmp = "";
//    const QVariantList &poso = d->m_COMPOValues.values( COMPO_DOSAGE );
//    const QVariantList &refPoso = d->m_COMPOValues.values( COMPO_REF_DOSAGE );
//    int idx = 0;
//    foreach( const QVariant &q, d->m_COMPOValues.values( COMPO_IAM_DENOMINATION ) ) {
//        if ( tmp != q.toString() ) {
//            toReturn << q.toString() + " " + poso.at(idx).toString() + " / " + refPoso.at(idx).toString();
//            tmp = q.toString();
//        }
//        ++idx;
//    }
//    return toReturn;
//}

QStringList DrugsData::dosageOfMolecules() const
{
    // return the list of distinct know dosage in composition of the drug
    QStringList toReturn;
    QString tmp = "";
    foreach(DrugComposition *compo, d->m_Compositions)
        toReturn << compo->dosage();
    return toReturn;
}

/**
  /~english \brief Return true if drug an be scored.
  /~french  \brief Retourne true si le médicament est sécable.
*/
bool DrugsData::isScoredTablet() const
{
    /** \todo manage all databases */
    return denomination().contains( QRegExp( "s.cable", Qt::CaseInsensitive ) );
}

QList<QVariant> DrugsData::CIPs() const
{
    QList<QVariant> ret;
    foreach( const DrugsDataPrivate::structCIP &s, d->m_CIPs )
        ret << s.CIP;
    return ret;
}

QStringList DrugsData::CIPsDenominations() const
{
    QStringList ret;
    foreach( const DrugsDataPrivate::structCIP &s, d->m_CIPs )
        ret << s.denomination;
    return ret;
}

QString DrugsData::linkToSCP() const
{
    QString toReturn;
    if (!value(Table_DRUGS, DRUGS_LINK_SPC).toString().isEmpty()) {
        if (Internal::DrugsBase::instance()->actualDatabaseInformations()->identifiant == Constants::DEFAULT_DATABASE_IDENTIFIANT)
            toReturn = QString(FRENCH_RPC_LINK).arg(value(Table_DRUGS, DRUGS_LINK_SPC).toString().rightJustified(7,'0'));
        else
            toReturn = value(Table_DRUGS, DRUGS_LINK_SPC).toString();
    }
    return toReturn;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Viewers -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
QString DrugsData::innComposition() const
{
    QString toReturn;
    QString lastInn;
    foreach( DrugComposition *compo, d->m_Compositions) {
        if (lastInn!=compo->innName())
            toReturn += QString("%1 %2 + ").arg(compo->innName(), compo->innDosage());
        lastInn = compo->innName();
    }
    if (!toReturn.isEmpty()) {
        toReturn.chop(3);
        toReturn = toReturn.toUpper();
        toReturn += " , " + d->m_CISValues.value(DRUGS_FORM).toString();
    }
    return toReturn;
}

QString DrugsData::toHtml() const
{
    QString msg;

    const QStringList & mols = this->listOfMolecules();
    const QStringList & iams = this->listOfInn();
    const QStringList & iamClass = this->listOfInnClasses();
    QString textIams, textClass;

    if ( iams.isEmpty() )
        textIams = QCoreApplication::translate( "DrugsData", "No INN found." );
    else
        textIams = iams.join( "<br>" );

    if ( iamClass.isEmpty() )
        textClass = QCoreApplication::translate( "DrugsData", "No interaction class found." );
    else
        textClass = iamClass.join( "<br>" );

    msg += QString( "<table border=1 cellpadding=2 cellspacing=2 width=100%>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1 align=center>\n"
                    "       <span style=\"font-weight: bold;\">%1</span>\n"
                    "       <br>UID = %2\n"
                    "       <br>ATC = %3\n"
                    "   </td>\n"
                    " </tr>\n"
                    " <tr>\n"
                    "   <td>%4</td>\n"
                    "   <td>%5</td>\n"
                    " </tr>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1>%6</td>\n"
                    " </tr>\n"
                    "</table>\n\n" )
            .arg( denomination() )
            .arg( value(Table_DRUGS, DRUGS_UID).toString() )
            .arg( value(Table_DRUGS, DRUGS_ATC).toString() )
            .arg( mols.join( "<br>" ) )
            .arg( textIams )
            .arg( textClass );
    return msg;
}

inline static DrugsDB::InteractionsManager *new_im() {return new DrugsDB::InteractionsManager();}

QString DrugsData::drugsListToHtml( const QDrugsList & list )
{
    QString msg;

//    // check interactions of the drugs list
//    mfDrugsBase *b = mfDrugsBase::instance();
//    b->interactions( list );
    InteractionsManager *im = new_im();
    im->setDrugsList(list);
    im->checkInteractions();

    // title
    msg = QString( "<html>\n"
                   "<head>\n"
                   "<meta http-equiv=\"content-type \" content=\"text/html; charset=UTF-8\">\n"
                   "<title>%1</title>\n"
                   "<meta name=\"author\" content=\"%1\">\n"
                   "<meta name=\"description\" content=\"%1\">\n"
                   "</head>\n"
                   "<body>\n" )
            .arg( qApp->applicationName() );

    msg += QString( "<p align=center><b>%1</b></p>\n" ).arg( qApp->applicationName() );

    foreach( DrugsData* d, list ) {
        msg += d->toHtml() + "<br>\n\n";
        if ( im->drugHaveInteraction(d) ) {
            const QList<DrugsInteraction *> &listDI = im->getInteractions( d );
            msg.append( im->listToHtml( listDI, true ) );

            msg.append( "<br></ul>" );
        }
    }
    msg.append( "</body>\n</html>\n" );

    delete im;

    return msg;
}


bool DrugsData::lessThan( const DrugsData *drug1, const DrugsData *drug2 )
{
    bool ald1, ald2;
    ald1 = drug1->prescriptionValue(Prescription::IsALD).toBool();
    ald2 = drug2->prescriptionValue(Prescription::IsALD).toBool();
//    qWarning() << "lessThan" << drug1->denomination() << drug2->denomination() << ald1 << ald2 << (drug1->denomination() < drug2->denomination());
    // ALD First of all
    if ((ald1)  && (!ald2)) {
        return true;
    } else if ((!ald1)  && (ald2)) {
        return false;
    }
    return drug1->denomination() < drug2->denomination();
}

/** \brief Fro debugging purpose only. Warn all values of the drug. */
void DrugsData::warn() const
{
    if (Utils::isDebugCompilation()) {
        qWarning() << warnText();
    }
}

/** \brief For debugging purpose only. Warn all values of the drug. */
QString DrugsData::warnText() const
{
    if (!Utils::isDebugCompilation())
        return QString();
    QString tmp;
    foreach( const int i, d->m_CISValues.keys() )
        tmp += QString( "CIS : %1 == %2\n" )
                           .arg( DrugsBase::instance()->field(Table_DRUGS, i) )
                           .arg( d->m_CISValues[i].toString() );
    foreach( DrugComposition *compo, d->m_Compositions )
        tmp += compo->warnText();
    foreach( const int i, d->m_PrescriptionValues.keys() )
        tmp += QString( "Prescription : %1 == %2\n" )
                           .arg( i )
                           .arg( d->m_PrescriptionValues[i].toString() );
    tmp += QString( "Distinct INN = %1\n" ).arg( listOfInn().join( "; " ) );
    tmp += QString( "NumberOfINN = %1\n" ).arg( numberOfInn() );
    tmp += QString( "Scored Tablet = %1\n" ).arg( isScoredTablet() );
    return tmp;
}

/** \brief For debugging purpose only. Warn all values of the drug. */
void DrugsData::smallDrugWarn() const
{
    if (!Utils::isDebugCompilation())
        return;
    Utils::Log::addMessage("DrugsData", QString("get drug: %1 \t %2 \t %3 \t %4")
                      .arg(this->UID())
                      .arg( this->denomination().leftJustified(60, ' '), this->form(), this->dosageOfMolecules().join(";")));
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- TextualDrugsData -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
TextualDrugsData::TextualDrugsData() :
        DrugsData()
{
    setPrescriptionValue(Prescription::IsTextualOnly, true);
    setValue(Table_DRUGS, DRUGS_UID, -1);
}

void TextualDrugsData::setDenomination(const QString &denomination)
{
    setValue(Table_DRUGS, DRUGS_NAME, denomination);
}



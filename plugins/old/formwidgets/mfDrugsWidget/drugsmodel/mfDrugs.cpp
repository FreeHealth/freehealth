/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class mfDrugs
  \brief This class owns the getters, setters and viewers of drugs.


  \todo Write documentation
  \todo Actually inside FMF and drugwidgetplugins mfDrugs are carried via a QList of pointers. This way we can not
  use the operators with mfDrugs. May be we must redefine QList of pointers in QList of references ?
 \ingroup freediams drugswidget
*/

#include "mfDrugs.h"

// include drugswidget headers
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugInteraction.h>
#include <drugsmodel/mfDosageModel.h>
#include <interactionsmodel/mfInteractionsManager.h>

// include toolkit headers
#include <tkLog.h>
#include <tkSettings.h>
#include <tkGlobal.h>
#include <tkConstantTranslations.h>

// include Qt headers
#include <QApplication>
#include <QHash>
#include <QMultiHash>
#include <QDebug>
#include <QLocale>

using namespace mfDrugsConstants;
Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS


//--------------------------------------------------------------------------------------------------------
//------------------------------- mfDrugPrivate constructor / destructor ---------------------------------
//--------------------------------------------------------------------------------------------------------
class mfDrugsPrivate
{
public:
    struct structCIP {
        int      CIP;
        QString  denomination;
        QDate    date;
    };

    mfDrugsPrivate()
    {
        m_CISValues.reserve(CIS_MaxParam);
        m_PrescriptionValues.reserve(Prescription::MaxParam - Prescription::Id);
    }

    ~mfDrugsPrivate()
    {
        qDeleteAll(m_Compositions);
        m_Compositions.clear();
    }

    QString prescriptionToText( bool toHtml );
    bool usesFromTo( const QString &from, QString &to, const int prescriptionIndex );


    QList<mfDrugComposition *>   m_Compositions;

    QList<structCIP>             m_CIPs;
    QHash< int, QVariant>        m_CISValues;
    QMultiHash< int, QVariant>   m_COMPOValues;
    QHash< int, QVariant>        m_PrescriptionValues;
    QString                      m_NoLaboDenomination;
    QString                      m_LastHtmlPrescription, m_LastTxtPrescription;   // Cache
    bool                         m_PrescriptionChanges;
    QSet<int>                    m_IamCodes;                 /*!< \brief Stores all Inn and IamClasses codes of this drug. */
};


//--------------------------------------------------------------------------------------------------------
//----------------------------------- mfDrug constructor / destructor ------------------------------------
//--------------------------------------------------------------------------------------------------------
mfDrugs::mfDrugs( QObject * parent )
    :QObject( parent ),
    d(new mfDrugsPrivate())
{
    // for a better memory cleaning
    if ( ! parent )
        setParent( qApp );
    d->m_NoLaboDenomination.clear();
    d->m_PrescriptionChanges = true;
}

mfDrugs::~mfDrugs()
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
void mfDrugs::setValue( const int tableref, const int fieldref, const QVariant & value )
{
    switch(tableref)
    {
    case Table_CIS : d->m_CISValues[fieldref] = value; break;
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
    default: tkLog::addError( this, tr( "Wrong table reference parameter in mfDrugs::setValue()." ) ); break;
    }
}

/** \brief Add a composition class description to this drug */
void mfDrugs::addComposition( mfDrugComposition *compo )
{
    d->m_Compositions.append(compo);
}

void mfDrugs::addCIP( const int CIP, const QString & denomination, QDate date )
{
    // TODO: one drug can have multi presentations ==> list */
    mfDrugsPrivate::structCIP st;
    st.CIP = CIP;
    st.denomination = denomination;
    st.date = date;
    d->m_CIPs.append( st );
}

/** \brief Stores a new INN and IamClasses codes for this drug assuming no doublons. Only used by mfDrugsBase::getDrugByCIS(). */
void mfDrugs::addInnAndIamClasses( const QSet<int> &codes )
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
void mfDrugs::setPrescriptionValue( const int fieldref, const QVariant & value )
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
QVariant mfDrugs::value( const int tableref, const int fieldref ) const
{
    switch (tableref)
    {
    case Table_CIS :
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
    default: tkLog::addError( this, tr( "Wrong table reference parameter in mfDrugs::value()." ) ); break;
    }
    return QVariant();
}

/**
  \brief Return true if drugs has a prescription.
  \sa mfDrugsConstants::Prescription
*/
bool mfDrugs::hasPrescription() const
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
QVariant mfDrugs::prescriptionValue( const int fieldref ) const
{
    return d->m_PrescriptionValues.value(fieldref);
}

/**
  \brief Return the drug denomination with or without pharmaceutical firms name
  \sa mfrugsConstants::MFDRUGS_SETTING_HIDELABORATORY
*/
const QString mfDrugs::denomination() const
{
    if (tkSettings::instance()->value(MFDRUGS_SETTING_HIDELABORATORY).toBool()) {
        if (d->m_NoLaboDenomination.isEmpty()) {
            d->m_NoLaboDenomination = value( Table_CIS, CIS_DENOMINATION ).toString();
            foreach( const QString &name, LABOS) {
                if (d->m_NoLaboDenomination.contains(name)) {
                    d->m_NoLaboDenomination.remove(name);
                    return d->m_NoLaboDenomination;
                }
            }
        }
        return d->m_NoLaboDenomination;
    }
    return value( Table_CIS, CIS_DENOMINATION ).toString();
}

/** \brief Returns the list of all the molecules' code of the drug composition */
QVariant mfDrugs::listOfCodeMolecules() const
{
    QVariantList tmp;
    foreach(mfDrugComposition *compo, d->m_Compositions)
        tmp << compo->m_CodeMolecule;
    return tmp;
}

/** \brief Returns the list of all IAM classes' names */
QStringList mfDrugs::listOfInnClasses() const
{
    QStringList tmp;
    foreach(int i, this->allInnAndIamClasses())
        if (i < 999)
            tmp << mfDrugsBase::instance()->getInnDenomination(i);
    return tmp;
}

/** \brief Returns the list of the molecules' name */
QStringList mfDrugs::listOfMolecules() const
{
    QStringList toReturn;
    foreach(const mfDrugComposition *compo, d->m_Compositions)
        toReturn << compo->m_MoleculeName;
    return toReturn;
}

/** \brief Return the list of all unique known Inn and Iam Classes names into a QStringList. */
QStringList mfDrugs::listOfInn() const
{
    // return the list of distinct know INN of this drug
    QStringList toReturn;
    QString tmp = "";
    foreach( mfDrugComposition *compo, d->m_Compositions) {
        if ( tmp != compo->innName() ) {
            toReturn << compo->innName();
            tmp = compo->innName();
        }
    }
//    if (d->m_COMPOValues.values( COMPO_IAM_DENOMINATION ).count() == 0)
//        return toReturn;
//    QString tmp = "";
//    foreach( const QVariant &q, d->m_COMPOValues.values( COMPO_IAM_DENOMINATION ) ) {
//        if ( tmp != q.toString() ) {
//            toReturn << q.toString();
//            tmp = q.toString();
//        }
//    }
    return toReturn;
}

/** \brief Returns all Inn and IamClasses codes knwon for this drug. */
QSet<int> mfDrugs::allInnAndIamClasses() const
{
    return d->m_IamCodes;
}

/** \brief Returns all Inn and IamClasses codes knwon for this drug. */
int mfDrugs::mainInnCode() const
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
QString mfDrugs::mainInnName() const
{
    return mfDrugsBase::instance()->getInnDenomination(mainInnCode());
}

/**
  \brief Return the main dosage of the main inn
  \sa mainInnCode(), mainInnName()
*/
QString mfDrugs::mainInnDosage() const
{
    QString toReturn;
    int main = mainInnCode();
    if (main != -1) {
        foreach( mfDrugComposition *compo, d->m_Compositions )
            if ((compo->m_InnCode==main) && (compo->isTheActiveSubstance()))
                return compo->innDosage();
    }
    return QString();
}

//QStringList mfDrugs::listOfInnWithPosology() const
//{
//    // TODO: add dosage of molecules */
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

QStringList mfDrugs::dosageOfMolecules() const
{
    // return the list of distinct know dosage in composition of the drug
    QStringList toReturn;
    QString tmp = "";
    foreach(mfDrugComposition *compo, d->m_Compositions)
        toReturn << compo->dosage();
    return toReturn;
}

/**
  /~english \brief Return true if drug an be scored.
  /~french  \brief Retourne true si le médicament est sécable.
*/
bool mfDrugs::isScoredTablet() const
{
    return denomination().contains( QRegExp( "s.cable", Qt::CaseInsensitive ) );
}

QList<QVariant> mfDrugs::CIPs() const
{
    QList<QVariant> ret;
    foreach( const mfDrugsPrivate::structCIP &s, d->m_CIPs )
        ret << s.CIP;
    return ret;
}

QStringList mfDrugs::CIPsDenominations() const
{
    QStringList ret;
    foreach( const mfDrugsPrivate::structCIP &s, d->m_CIPs )
        ret << s.denomination;
    return ret;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Viewers -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
QString mfDrugs::innComposition() const
{
    QString toReturn;
    QString lastInn;
    foreach( mfDrugComposition *compo, d->m_Compositions) {
        if (lastInn!=compo->innName())
            toReturn += QString("%1 %2 + ").arg(compo->innName(), compo->innDosage());
        lastInn = compo->innName();
    }
    if (!toReturn.isEmpty()) {
        toReturn.chop(3);
        toReturn = toReturn.toUpper();
        toReturn += " , " + d->m_CISValues.value(CIS_FORME).toString();
    }
    return toReturn;
}

QString mfDrugs::toHtml() const
{
    QString msg;

    const QStringList & mols = this->listOfMolecules();
    const QStringList & iams = this->listOfInn();
    const QStringList & iamClass = this->listOfInnClasses();
    QString textIams, textClass;

    if ( iams.isEmpty() )
        textIams = QCoreApplication::translate( "mfDrugs", "No INN found." );
    else
        textIams = iams.join( "<br>" );

    if ( iamClass.isEmpty() )
        textClass = QCoreApplication::translate( "mfDrugs", "No interaction class found." );
    else
        textClass = iamClass.join( "<br>" );

    msg += QString( "<table border=1 cellpadding=2 cellspacing=2 width=100%>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1 align=center>\n"
                    "       <span style=\"font-weight: bold;\">%1</span>\n"
                    "       <br>CIS = %2\n"
                    "   </td>\n"
                    " </tr>\n"
                    " <tr>\n"
                    "   <td>%3</td>\n"
                    "   <td>%4</td>\n"
                    " </tr>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1>%5</td>\n"
                    " </tr>\n"
                    "</table>\n\n" )
            .arg( denomination() )
            .arg( value( Table_CIS, CIS_CIS ).toString() )
            .arg( mols.join( "<br>" ) )
            .arg( textIams )
            .arg( textClass );
    return msg;
}

QString mfDrugs::drugsListToHtml( const QDrugsList & list )
{
    QString msg;

//    // check interactions of the drugs list
//    mfDrugsBase *b = mfDrugsBase::instance();
//    b->interactions( list );
    mfInteractionsManager * im = new mfInteractionsManager();
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

    foreach( mfDrugs* d, list ) {
        msg += d->toHtml() + "<br>\n\n";
        if ( im->drugHaveInteraction(d) ) {
            const QList<mfDrugInteraction *> & listDI = im->getInteractions( d );
            msg.append( im->listToHtml( listDI, true ) );

            msg.append( "<br></ul>" );
        }
    }
    msg.append( "</body>\n</html>\n" );

    delete im;

    return msg;
}

/**
  \brief Transform the prescription values to an understandable string.
  You can specify if you want an html formatted or plain text using \e toHtml
  \sa  mfDrugs::prescriptionToHtml(), mfDrugs::prescriptionToPlainText()
*/
QString mfDrugsPrivate::prescriptionToText( bool toHtml )
{
    // Check cached Prescriptions
    if (!m_PrescriptionChanges) {
        if (toHtml) {
            if (!m_LastHtmlPrescription.isEmpty())
                return m_LastHtmlPrescription;
        } else {
            if (!m_LastTxtPrescription.isEmpty())
                return m_LastTxtPrescription;
        }
    }

    QString toReturn;
    QString intakes, duration, period;
    QString paragraphBegin, paragraphEnd, linebreak, boldBegin, boldEnd;

    if ( toHtml ) {
        linebreak = "<br />";
        boldEnd = "</b>";
        boldBegin = "<b>";
        paragraphBegin = "";
        paragraphEnd = "";
    } else {
        linebreak = "\n";
        boldEnd = "";
        boldBegin = "";
        paragraphBegin = "";
        paragraphEnd = "";
    }

    // INTAKES
    QString to, from, scheme;
    to = QString::number(m_PrescriptionValues[Prescription::IntakesTo].toDouble());
    from = QString::number(m_PrescriptionValues[Prescription::IntakesFrom].toDouble());
    scheme = m_PrescriptionValues[Prescription::IntakesScheme].toString();
    if ( usesFromTo( from, to, Prescription::IntakesUsesFromTo ) ) {
        intakes = QString( "%1 %2 %3 %4 %5" )
                  .arg( QCoreApplication::translate( "mfDrugs" , "from") )
                  .arg( from )
                  .arg( QCoreApplication::translate( "mfDrugs" ,"to") )
                  .arg( to )
                  .arg( scheme )
                  ;
    } else {
        intakes = QString( "%1 %2" )
                  .arg(from)
                  .arg(scheme)
                  ;
    }
    // Intakes daily scheme
    const QStringList & daily = m_PrescriptionValues.value(Prescription::DailyScheme).toStringList();
    if (daily.count() > 0) {
        QString tmp;
        tmp = QString( " %1").arg(daily.join(", "));
        tmp.replace( tmp.lastIndexOf(", "), 2, " " + QCoreApplication::translate("mfDrugs", "and") + " " );
        // replace last , to AND
        intakes += tmp;
    }

    // DURATION
    to = QString::number(m_PrescriptionValues[Prescription::DurationTo].toDouble());
    from = QString::number(m_PrescriptionValues[Prescription::DurationFrom].toDouble());
    scheme = m_PrescriptionValues[Prescription::DurationScheme].toString();
    if ( usesFromTo( from, to, Prescription::DurationUsesFromTo ) ) {
        duration = QString( "%1 %2 %3 %4 %5" )
                   .arg( QCoreApplication::translate( "mfDrugs" ,"from") )
                   .arg( from )
                   .arg( QCoreApplication::translate( "mfDrugs" ,"to") )
                   .arg( to )
                   .arg( scheme )
                   ;
    } else {
        duration = QString( "%1 %2" )
                   .arg( from )
                   .arg( scheme )
                   ;
    }

    // PERIOD
    from = QString::number(m_PrescriptionValues[Prescription::Period].toDouble());
    scheme = m_PrescriptionValues[Prescription::PeriodScheme].toString();
    if (from == "1")
        period = QString( "%1" )
        .arg( scheme );
    else
        period = QString( "%1 %2" )
                 .arg( from )
                 .arg( scheme );

    // Prepare phrase
    toReturn += QString( "%1 %2 %3 %4 %5 %6 %7" )
                .arg( paragraphBegin )
                .arg( intakes )
                .arg( QCoreApplication::translate( "mfDrugs" ,"each") )
                .arg( period )
                .arg( QCoreApplication::translate( "mfDrugs" ,"during") )
                .arg( duration )
                .arg( paragraphEnd )
                ;

    int idx = m_PrescriptionValues.value(Prescription::MealTimeSchemeIndex).toInt();
    if (idx != 0)
        toReturn += linebreak + mealTime(idx);

    QString note = m_PrescriptionValues.value(Prescription::Note).toString();
    if (!note.isEmpty()) {
        if (toHtml)
            note.replace("\n","<br />");
        toReturn += linebreak + note;
    }

    // Assume Prescription cache
    if (toHtml) {
        m_LastHtmlPrescription = toReturn;
        m_LastTxtPrescription.clear();
        m_PrescriptionChanges = false;
    } else {
        m_LastTxtPrescription = toReturn;
        m_LastHtmlPrescription.clear();
        m_PrescriptionChanges = false;
    }

    return toReturn;
}

bool mfDrugsPrivate::usesFromTo( const QString &from, QString &to, const int prescriptionIndex )
{
    if ( (prescriptionIndex==Prescription::DurationUsesFromTo) ||
         (prescriptionIndex==Prescription::IntakesUsesFromTo) )
        return ( (!to.isEmpty()) && (to != from) && (m_PrescriptionValues.value( prescriptionIndex ).toBool()) );
    return false;
}

/** \brief Transform prescription model to a understandable text formatted in HTML */
QString mfDrugs::prescriptionToHtml() const
{
    return d->prescriptionToText(true);
}

/** \brief Transform prescription model to a understandable plain text */
QString mfDrugs::prescriptionToPlainText() const
{
    return d->prescriptionToText(false);
}

bool mfDrugs::lessThan( const mfDrugs *drug1, const mfDrugs *drug2 )
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
void mfDrugs::warn() const
{
    if (tkGlobal::isDebugCompilation()) {
        qWarning() << warnText();
    }
}

/** \brief For debugging purpose only. Warn all values of the drug. */
QString mfDrugs::warnText() const
{
    if (!tkGlobal::isDebugCompilation())
        return QString();
    QString tmp;
    foreach( const int i, d->m_CISValues.keys() )
        tmp += QString( "CIS : %1 == %2\n" )
                           .arg( mfDrugsBase::instance()->field( Table_CIS, i ) )
                           .arg( d->m_CISValues[i].toString() );
    foreach( mfDrugComposition *compo, d->m_Compositions )
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
void mfDrugs::smallDrugWarn() const
{
    if (!tkGlobal::isDebugCompilation())
        return;
    tkLog::addMessage(this, QString("get drug: %1 \t %2 \t %3 \t %4")
                      .arg(this->CIS())
                      .arg( this->denomination().leftJustified(60, ' '), this->form(), this->dosageOfMolecules().join(";")));
}

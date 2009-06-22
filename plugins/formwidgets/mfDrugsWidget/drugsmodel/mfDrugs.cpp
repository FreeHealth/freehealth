/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#include "mfDrugs.h"

// include drugswidget headers
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsdatabase/mfDrugsBase_p.h>
#include <drugsmodel/mfDrugInteraction.h>
#include <drugsmodel/mfDosageModel.h>

// include toolkit headers
#include <tkLog.h>
#include <tkSettings.h>
#include <tkGlobal.h>

// include Qt headers
#include <QApplication>
#include <QHash>
#include <QMultiHash>
#include <QDebug>
#include <QLocale>

using namespace mfDrugsConstants;

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

    mfDrugsPrivate::mfDrugsPrivate()
    {
        m_CISValues.reserve(CIS_MaxParam);
        m_PrescriptionValues.reserve(Prescription::MaxParam - Prescription::Id);
    }

    QString prescriptionToText( bool toHtml );
    bool usesFromTo( const QString &from, QString &to, const int prescriptionIndex );

    QList<structCIP>             m_CIPs;
    QHash< int, QVariant>        m_CISValues;
    QMultiHash< int, QVariant>   m_COMPOValues;
    QHash< int, QVariant>        m_PrescriptionValues;
    QString                      m_NoLaboDenomination;
    QString                      m_LastHtmlPrescription, m_LastTxtPrescription;   // Cache
    bool                         m_PrescriptionChanges;
    QStringList                  m_EquivalentsNames;
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

void mfDrugs::addCIP( const int CIP, const QString & denomination, QDate date )
{
    /** \todo one drug can have multi presentations ==> list */
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
            if ( d->m_COMPOValues.contains( fieldref ) )
                if ( fieldref == COMPO_IAM_CLASS_DENOMINATION )
                    return d->m_COMPOValues.value( fieldref );
            else
                return d->m_COMPOValues.values( fieldref );
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

/** \brief Return the list of all unique known Inn and Iam Classes names into a QStringList. */
QStringList mfDrugs::listOfInn() const
{
    // return the list of distinct know INN of this drug
    QStringList toReturn;
    if (d->m_COMPOValues.values( COMPO_IAM_DENOMINATION ).count() == 0)
        return toReturn;
    QString tmp = "";
    foreach( const QVariant &q, d->m_COMPOValues.values( COMPO_IAM_DENOMINATION ) ) {
        if ( tmp != q.toString() ) {
            toReturn << q.toString();
            tmp = q.toString();
        }
    }
    return toReturn;
}

/** \brief Returns all Inn and IamClasses codes knwon for this drug. */
QSet<int> mfDrugs::allInnAndIamClasses() const
{
    return d->m_IamCodes;
}

QStringList mfDrugs::listOfInnWithPosology() const
{
    /** \todo add dosage of molecules */
    QStringList toReturn;
    QString tmp = "";
    const QVariantList &poso = d->m_COMPOValues.values( COMPO_DOSAGE );
    const QVariantList &refPoso = d->m_COMPOValues.values( COMPO_REF_DOSAGE );
    int idx = 0;
    foreach( const QVariant &q, d->m_COMPOValues.values( COMPO_IAM_DENOMINATION ) ) {
        if ( tmp != q.toString() ) {
            toReturn << q.toString() + " " + poso.at(idx).toString() + " / " + refPoso.at(idx).toString();
            tmp = q.toString();
        }
        ++idx;
    }
    return toReturn;
}

/** \brief Return the list of the inn equivalent drug denomination. */
QStringList mfDrugs::innEquivalentsFullNames() const
{
    return d->m_EquivalentsNames;
}

/**
  \brief Set the list of the inn equivalent drug denomination.
  \sa mfDrugsBase::findInnEquivalentsNames()
*/
void mfDrugs::setInnEquivalentsFullNames( const QStringList &list )
{
    d->m_EquivalentsNames.clear();
    d->m_EquivalentsNames = list;
}

QStringList mfDrugs::dosageOfMolecules() const
{
    // return the list of distinct know dosage in composition of the drug
    QStringList toReturn;
    QString tmp = "";
    foreach( const QVariant &q, d->m_COMPOValues.values( COMPO_DOSAGE ) ) {
        if ( tmp != q.toString() ) {
            toReturn << q.toString();
            tmp = q.toString();
        }
    }
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
/** \brief Fro debugging purpose only. Warn all values of the drug. */
void mfDrugs::warn() const
{
    if (!tkGlobal::isDebugCompilation())
        return;
    foreach( const int i, d->m_CISValues.keys() )
        tkLog::addMessage( this, QString( "CIS : %1 == %2" )
                           .arg( mfDrugsBase::instance()->field( Table_CIS, i ) )
                           .arg( d->m_CISValues[i].toString() ) );
    foreach( const int i, d->m_COMPOValues.keys() )
        foreach( const QVariant &qvar, d->m_COMPOValues.values(i) )
            tkLog::addMessage( this, QString( "COMPO : %1 == %2" )
                               .arg( mfDrugsBase::instance()->field( Table_COMPO, i ) )
                               .arg( qvar.toString() ) );
    foreach( const int i, d->m_PrescriptionValues.keys() )
        tkLog::addMessage( this, QString( "Prescription : %1 == %2" )
                           .arg( i )
                           .arg( d->m_PrescriptionValues[i].toString() ) );
    tkLog::addMessage( this, QString( "Distinct INN = %1" ).arg( listOfInn().join( "; " ) ) );
    tkLog::addMessage( this, QString( "NumberOfINN = %1" ).arg( numberOfInn() ) );
    tkLog::addMessage( this, QString( "Scored Tablet = %1" ).arg( isScoredTablet() ) );
}

void mfDrugs::smallDrugWarn() const
{
    if (!tkGlobal::isDebugCompilation())
        return;
    tkLog::addMessage(this, QString("get drug: %1 \t %2 \t %3 \t %4")
                      .arg(this->CIS())
                      .arg( this->denomination().leftJustified(60, ' '), this->form(), this->dosageOfMolecules().join(";")));
}

QString mfDrugs::toHtml() const
{
    QString msg;

    const QStringList & mols = value( Table_COMPO, COMPO_DENOMINATION ).toStringList();
    const QStringList & iams = value( Table_COMPO, COMPO_IAM_DENOMINATION ).toStringList();
    const QStringList & iamClass = value( Table_COMPO, COMPO_IAM_CLASS_DENOMINATION ).toStringList();
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

    // check interactions of the drugs list
    mfDrugsBase *b = mfDrugsBase::instance();
    b->interactions( list );

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
        if ( b->drugHaveInteraction( d ) ) {
            const QList<mfDrugInteraction *> & listDI = b->getInteractions( d );
            msg.append( mfDrugInteraction::listToHtml( listDI, true ) );

            msg.append( "<br></ul>" );
        }
    }
    msg.append( "</body>\n</html>\n" );

    return msg;
}

/**
  \brief Transform the prescription values to an understandable string.
  You can specify if you want an html formatted or plain text using \e toHtml
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
    to = m_PrescriptionValues[Prescription::IntakesTo].toString();
    from = m_PrescriptionValues[Prescription::IntakesFrom].toString();
    scheme = m_PrescriptionValues[Prescription::IntakesScheme].toString();
    if ( usesFromTo( from, to, Prescription::IntakesUsesFromAndTo ) ) {
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
        tmp.replace( tmp.lastIndexOf(", "), 2, " " + QCoreApplication::translate("mfDrugs", "and"));
        // replace last , to AND
        intakes += tmp;
    }

    // DURATION
    to = m_PrescriptionValues[Prescription::DurationTo].toString();
    from = m_PrescriptionValues[Prescription::DurationFrom].toString();
    scheme = m_PrescriptionValues[Prescription::DurationScheme].toString();
    if ( usesFromTo( from, to, Prescription::DurationUsesFromAndTo ) ) {
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
    from = m_PrescriptionValues[Prescription::Period].toString();
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
        toReturn += linebreak + mfDosageModel::mealTime(idx);

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
    if ( (prescriptionIndex==Prescription::DurationUsesFromAndTo) ||
         (prescriptionIndex==Prescription::IntakesUsesFromAndTo) )
        return ( (!to.isEmpty()) && (to != from) && (m_PrescriptionValues.value( prescriptionIndex ).toBool()) );
    return false;
}

QString mfDrugs::prescriptionToHtml() const
{
    return d->prescriptionToText(true);
}

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

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
/**
  \class DrugsInteraction
  \brief Drugs Interaction class.
  This class is a 'data class'. It only holds datas about interactions.\n
  You can statically :
  \li get the interaction's icon using the static iamIcon().
  \li transform a list of interactions to human readable Html with listToHtml()
  \li transform a list of interactions to a synthesis human readable Html with synthesisToHtml()
  \li get the type of interaction's name using typeToString().

  You can, non statically :
  \li set the values of the interactions with setValue() (only used by mfDrugsBase).
  \li get the values with value()
  \li get the type of interaction with typeOfIAM() and type()
  \li get information's text of the interaction with risk()
  \li get the recommandation's text with management()
  \ingroup freediams drugswidget
*/

#include "drugsinteraction.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/constants.h>

#include <coreplugin/itheme.h>

using namespace DrugsDB;
using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}

/** \brief Used by drugs database to feed values. \e fieldref refers to the enum : mfDrugsConstants::IAMfields */
void DrugsInteraction::setValue( const int fieldref, const QVariant & value )
{
    if (fieldref == DI_Type) {
        QString t = value.toString();
        Constants::Interaction::TypesOfIAM r = Constants::Interaction::noIAM;
        if (t.contains("P"))
            r |= Constants::Interaction::Precaution;
        if (t.contains("C"))
            r |= Constants::Interaction::ContreIndication;
        if (t.contains("D"))
            r |= Constants::Interaction::Deconseille;
        if (t.contains("T"))
            r |= Constants::Interaction::APrendreEnCompte;
        if (t.contains("450"))
            r |= Constants::Interaction::P450;
        if (t.contains("I"))
            r |= Constants::Interaction::Information;
        if (t.contains("Y"))
            r |= Constants::Interaction::GPG;
        m_Infos.insert(fieldref, int(r));
    } else {
        m_Infos.insert(fieldref, value);
    }
}

/** \brief Get values of the interaction class. \e fieldref refers to the enum : mfDrugsConstants::IAMfields */
QVariant DrugsInteraction::value(const int fieldref) const
{
    switch (fieldref) {
    case DI_Type: return typeOfIAM(m_Infos.value(fieldref).toInt());
    case DI_ATC1_Label: return drugsBase()->getAtcLabel(m_Infos.value(DI_ATC1).toInt());
    case DI_ATC2_Label: return drugsBase()->getAtcLabel(m_Infos.value(DI_ATC2).toInt());
    case DI_Risk:
        {
            QString l = QLocale().name().left(2);
            QString r;
            if (l=="en")
                r = m_Infos.value(DI_RiskEn).toString();
            else
                r = m_Infos.value(DI_RiskFr).toString();
            return r.replace("<br />", "<br>");
        }
    case DI_Management:
        {
            QString l = QLocale().name().left(2);
            QString r;
            if (l=="en")
                r = m_Infos.value(DI_ManagementEn).toString();
            else
                r = m_Infos.value(DI_ManagementFr).toString();
            return r.replace("<br />", "<br>");
        }
    default: return m_Infos.value(fieldref, QVariant());
    }
    return QVariant();
}

/** \brief Transforms the type \e t to its name. \e t refers to enum : mfInteractionsConstants::Interaction::TypesOfIAM */
QString DrugsInteraction::typeToString(const int t)
{
     QStringList tmp;
     Constants::Interaction::TypesOfIAM r = Constants::Interaction::TypesOfIAM(t);
     if (r & Constants::Interaction::APrendreEnCompte)
          tmp << tkTr(Trans::Constants::TAKE_INTO_ACCOUNT);
     if (r & Constants::Interaction::Deconseille)
          tmp << tkTr(Trans::Constants::DISCOURAGED);
     if (r & Constants::Interaction::ContreIndication)
          tmp << tkTr(Trans::Constants::CONTRAINDICATION);
     if (r & Constants::Interaction::Precaution)
          tmp << tkTr(Trans::Constants::PRECAUTION_FOR_USE);
     if (r & Constants::Interaction::P450)
          tmp << tkTr(Trans::Constants::P450_IAM);
     if (r & Constants::Interaction::GPG)
          tmp << tkTr(Trans::Constants::GPG_IAM);
     if (r & Constants::Interaction::Information)
          tmp << tkTr(Trans::Constants::INFORMATION);
     return tmp.join( ", " );
}

/** \brief Transforms the type \e t to its name. \e t refers to enum : mfInteractionsConstants::Interaction::TypesOfIAM */
QString DrugsInteraction::typeOfIAM( const int & t ) const
{
    return typeToString(t);
}

/** \brief Returns the type of interactions. Type refers to enum : mfInteractionsConstants::Interaction::TypesOfIAM */
Constants::Interaction::TypesOfIAM DrugsInteraction::type() const
{
    return Constants::Interaction::TypesOfIAM(m_Infos.value(DI_Type, 0).toInt());
}

QList<DrugsData *> DrugsInteraction::drugs() const
{
    return m_InteractingDrugs;
}

/** \brief for debugging purpose */
void DrugsInteraction::warn() const
{
     qWarning() << "DrugsInteraction Warning";
     foreach( const int i, m_Infos.keys() )
         qWarning() << i << m_Infos.value(i).toString();
     foreach(DrugsData * dr, m_InteractingDrugs)
         qWarning() << "drug" << dr->denomination();
}

QString DrugsInteraction::header() const
{
    return drugsBase()->getAtcLabel(value(DI_ATC1).toInt()) + " - " + drugsBase()->getAtcLabel(value(DI_ATC2).toInt());
}

/** \brief Returns the information's text of the interaction */
QString DrugsInteraction::risk() const
{
    return value(DI_Risk).toString();
}

/** \brief Returns the recommandation's text of the interaction */
QString DrugsInteraction::management() const
{
    return value(DI_Management).toString();
}

QString DrugsInteraction::referencesLink() const
{
    return value(DI_ReferencesLink).toString();
}

/** \brief Function used to sort interactions list */
bool DrugsInteraction::lessThan(const DrugsInteraction *int1, const DrugsInteraction *int2)
{
    return int1->type() < int2->type();
}

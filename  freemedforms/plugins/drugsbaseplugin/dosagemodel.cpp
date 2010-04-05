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
  \class DosageModel
  \brief Manages predetermined dosage.
  A \b "dosage" is a set of values that defines what, when and how to prescribe a drug. A dosage can apply :
  \li on a specific drug base on its CIS,
  \li on the INN molecule of the drug which are presented in the same "dosage" (100mg, 1g...).
  For example :
  \li CLAMOXYL 1g can have prescription based on its CIS or on the \e amoxicilline molecule.
  \li CIS based could be : one intake morning and evening during seven days.
  \li INN based could be the same and can be applied to AMOXICILLINE PHARMACEUTICAL_LABO 1g.

  A \b "prescription" is the when and how you prescribe a selected drug.\n
  database(DOSAGES_DATABASE_NAME) should be defined BEFORE instance()

  \todo Create a specific user's right for dosage creation/edition/modification/deletion +++.

  \ingroup freediams drugswidget
*/


#include "dosagemodel.h"

#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>

/** \todo reimplement user management */

#include <QSqlRecord>
#include <QSqlError>
#include <QApplication>
#include <QStringList>
#include <QUuid>
#include <QDateTime>
#include <QColor>
#include <QLocale>

namespace mfDosageModelConstants {
    const char *const XML_DOSAGE_MAINTAG            = "DOSAGE";
    const char* const DIRTYROW_BACKGROUNDCOLOR      = "yellow";
}

using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;
using namespace mfDosageModelConstants;

static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Static Datas --------------------------------------------
//--------------------------------------------------------------------------------------------------------
// intialize static private members
QStringList DosageModel::m_ScoredTabletScheme = QStringList();
QStringList DosageModel::m_PreDeterminedForms = QStringList();
QString     DosageModel::m_ActualLangage = "";

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Translations ----------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Used to retranslate static stringlists */
void DosageModel::changeEvent(QEvent * event)
{
    // proceed only LangageChange events
    if (event->type() != QEvent::LanguageChange)
        return;
    retranslate();
}

/** \brief Used to retranslate static stringlists */
void DosageModel::retranslate()
{
    // proceed only if needed
    if (m_ActualLangage == QLocale().name().left(2))
        return;

    // store the langage and retranslate
    m_ActualLangage = QLocale().name().left(2);

    m_ScoredTabletScheme =
            QStringList()
            << tr("complet tab.")
            << tr("half tab.")
            << tr("quater tab.");

    m_PreDeterminedForms =
            QStringList()
            << tr("dose per kilograms")
            << tr("reference spoon")
            << tr("2.5 ml spoon")
            << tr("5 ml spoon")
            << tr("puffs")
            << tr("dose")
            << tr("mouthwash")
            << tr("inhalation")
            << tr("application")
            << tr("washing")
            << tr("shampooing")
            << tr("eyewash")
            << tr("instillation")
            << tr("pulverization");


}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Static datas ----------------------------------------
//--------------------------------------------------------------------------------------------------------

/** \brief Scored tablet predetermined stringlist */
QStringList DosageModel::scoredTabletScheme()
{
    if (m_ScoredTabletScheme.count() == 0)
        retranslate();
    return m_ScoredTabletScheme;
}

QStringList DosageModel::predeterminedForms()
{
    if (m_PreDeterminedForms.count()==0)
        retranslate();
    return m_PreDeterminedForms;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ Dosage Model ------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Constructor */
DosageModel::DosageModel(DrugsDB::DrugsModel *parent)
        : QSqlTableModel(parent, QSqlDatabase::database(Dosages::Constants::DOSAGES_DATABASE_NAME)),
        m_DrugsModel(parent)
{
    setObjectName("DosageModel");
    QSqlTableModel::setTable(Dosages::Constants::DOSAGES_TABLE_NAME);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_UID = -1;
    if (drugsBase()->isDatabaseTheDefaultOne()) {
        setFilter(QString("%1 = \"%2\"")
                  .arg(database().tables(QSql::Tables).at(Dosages::Constants::DrugsDatabaseIdentifiant))
                  .arg(DrugsDB::Constants::DEFAULT_DATABASE_IDENTIFIANT));
    } else {
        if (drugsBase()->actualDatabaseInformations()) {
            setFilter(QString("%1 = \"%2\"")
                      .arg(database().tables(QSql::Tables).at(Dosages::Constants::DrugsDatabaseIdentifiant))
                      .arg(drugsBase()->actualDatabaseInformations()->identifiant));
        }
    }
}

/** \brief Defines datas for the dosage. Database is only updated when calling submitAll(). */
bool DosageModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    Q_ASSERT_X(m_UID != -1, "DosageModel::setData", "before using the dosagemodel, you must specify the CIS of the related drug");
    if (! index.isValid())
        return false;
    if (role == Qt::EditRole) {
        // verify the value is different as model
        QVariant q = data(index,Qt::DisplayRole);
        if (q==value)
            return false;

        // set only once modification date (infinite loop prevention)
        if (index.column()!=Dosages::Constants::ModificationDate)
            setData(this->index(index.row(), Dosages::Constants::ModificationDate), QDateTime::currentDateTime());

        // mark row as dirty
        if (!m_DirtyRows.contains(index.row())) {
            m_DirtyRows << index.row();
        }

        if (!QSqlTableModel::setData(index, value, role))
            return false;

        QModelIndex label = this->index(index.row(), Dosages::Constants::Label);
        emit dataChanged(label, label);
    }
    return false;
}

/** \brief Retreive dosage's datas. */
QVariant DosageModel::data(const QModelIndex & item, int role) const
{
    Q_ASSERT_X(m_UID != -1, "DosageModel::data", "before using the dosagemodel, you must specify the CIS of the related drug");
    if (!item.isValid())
        return QVariant();

    switch (role)
    {
    case Qt::FontRole:
        {
            QFont font;
            if (m_DirtyRows.contains(item.row()))
                font.setBold(true);
            else
                font.setBold(false);
            return font;
        }
    case Qt::BackgroundRole :
        {
            if (m_DirtyRows.contains(item.row()))
                return QColor(DIRTYROW_BACKGROUNDCOLOR);
            else
                return QColor("white");
        }
    case Qt::DisplayRole:
    case Qt::EditRole:
        {
            return QSqlTableModel::data(item, role);
            break;
        }
    case Qt::DecorationRole :
        {
            //            qWarning() << QSqlTableModel::index(item.row(), Dosage::INN_LK).data().toString();
            if (!QSqlTableModel::index(item.row(), Dosages::Constants::INN_LK).data().toString().isEmpty())
                return theme()->icon(DrugsDB::Constants::I_SEARCHINN);
            return theme()->icon(DrugsDB::Constants::I_SEARCHCOMMERCIAL);
        }
    }
    return QVariant();
}

/** \brief Create new dosages. Defaults values of the dosages are stted here. */
bool DosageModel::insertRows(int row, int count, const QModelIndex & parent)
{
    Q_ASSERT_X(m_UID != -1, "DosageModel::insertRows", "before inserting row, you must specify the UID of the related drug");
    QString userUuid;
#ifdef FREEDIAMS
    userUuid = DrugsDB::Constants::FREEDIAMS_DEFAULT_USER_UUID;
#else
    /** \todo FMF : add user */
    //    userUuid = tkUserModel::instance()->currentUserData(User::Uuid).toString();
#endif
    int i;
    int createdRow;
    bool toReturn = true;
    for (i=0; i < count; ++i) {
        createdRow = row+i;
        if (!QSqlTableModel::insertRows(createdRow, 1, parent)) {
            Utils::Log::addError(this, tr("Model Error : unable to insert a row"));
            toReturn = false;
        } else {
            setData(index(createdRow, Dosages::Constants::Uuid) , QUuid::createUuid().toString());
            if (drugsBase()->actualDatabaseInformations())
                setData(index(createdRow, Dosages::Constants::DrugsDatabaseIdentifiant) , drugsBase()->actualDatabaseInformations()->identifiant);
            setData(index(createdRow, Dosages::Constants::CIS_LK) , m_UID);
            setData(index(createdRow, Dosages::Constants::IntakesTo) , 1);
            setData(index(createdRow, Dosages::Constants::IntakesFrom) , 1);
            setData(index(createdRow, Dosages::Constants::IntakesUsesFromTo) , false);
            QString s = settings()->value(DrugsDB::Constants::S_PROTOCOL_DEFAULT_SCHEMA).toString();
            if (s.isEmpty()) {
                setData(index(createdRow, Dosages::Constants::IntakesScheme) , m_DrugsModel->drugData(m_UID, Drug::AvailableForms).toStringList().at(0));
            } else if (s=="||") {
                setData(index(createdRow, Dosages::Constants::IntakesScheme) , tkTr(Trans::Constants::INTAKES, 1));
            } else {
                setData(index(createdRow, Dosages::Constants::IntakesScheme) , s);
            }
            setData(index(createdRow, Dosages::Constants::Period), 1);
            setData(index(createdRow, Dosages::Constants::PeriodScheme) , tkTr(Trans::Constants::DAYS));
            setData(index(createdRow, Dosages::Constants::DurationTo) , 1);
            setData(index(createdRow, Dosages::Constants::DurationFrom) , 1);
            setData(index(createdRow, Dosages::Constants::DurationUsesFromTo) , false);
            setData(index(createdRow, Dosages::Constants::DurationScheme) , tkTr(Trans::Constants::MONTHS));
            setData(index(createdRow, Dosages::Constants::IntakesIntervalOfTime) , 0);
            setData(index(createdRow, Dosages::Constants::MinAge) , 0);
            setData(index(createdRow, Dosages::Constants::MaxAge) , 0);
            setData(index(createdRow, Dosages::Constants::MinWeight) , 0);
            setData(index(createdRow, Dosages::Constants::MinClearance) , 0);
            setData(index(createdRow, Dosages::Constants::MaxClearance) , 0);
            setData(index(createdRow, Dosages::Constants::SexLimitedIndex) , 0);
            setData(index(createdRow, Dosages::Constants::Note) , QVariant());
            setData(index(createdRow, Dosages::Constants::CreationDate) , QDateTime::currentDateTime());
            setData(index(createdRow, Dosages::Constants::Transmitted) , QVariant());
        }
    }
    return toReturn;
}

/**
  \brief Removes somes rows.
  \todo  when QSqlTableModel removes rows it may call select() and reset(),  this causes non submitted modifications deletion on the model THIS MUST BE IMPROVED
*/
bool DosageModel::removeRows(int row, int count, const QModelIndex & parent)
{
    Q_ASSERT_X(m_UID != -1, "DosageModel::removeRows", "before using the dosagemodel, you must specify the CIS of the related drug");

    if (row < 0)
        return false;

    setEditStrategy(QSqlTableModel::OnRowChange);
    bool toReturn = false;
    if (QSqlTableModel::removeRows(row, count, parent)) {
        int i;
        for(i=0; i < count; ++i) {
            if (m_DirtyRows.contains(row+i))
                m_DirtyRows.remove(row+i);
        }
        toReturn = true;
    }
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    return toReturn;
}

/** \brief Revert a row */
void DosageModel::revertRow(int row)
{
    Q_ASSERT_X(m_UID != -1, "DosageModel::revertRow", "before using the dosagemodel, you must specify the CIS of the related drug");
    QSqlTableModel::revertRow(row);
    if (m_DirtyRows.contains(row))
        m_DirtyRows.remove(row);
}

/** \brief Submit model changes to database */
bool DosageModel::submitAll()
{
    Q_ASSERT_X(m_UID != -1, "DosageModel::submitAll", "before using the dosagemodel, you must specify the CIS of the related drug");
    //    warn();
    QSet<int> safe = m_DirtyRows;
    m_DirtyRows.clear();
    if (QSqlTableModel::submitAll()) {
        return true;
    } else
        m_DirtyRows = safe;
    //    reset();
    return false;
}

/**
  \brief Filter the model from the drug CIS and if possible for the inn prescriptions.
*/
bool DosageModel::setDrugUID(const int uid)
{
    if (uid == m_UID)
        return true;
    m_UID = uid;
    QString filter = QString("%1=%2").arg(record().fieldName(Dosages::Constants::CIS_LK)).arg(m_UID);
    int inn = -1;
    if (m_DrugsModel)
        inn = m_DrugsModel->drugData(uid, Constants::Drug::MainInnCode).toInt();
    //    int inn = DRUGMODEL->drugData(_CIS,Drug::MainInnCode).toInt();
    if (inn!=-1) {
        // add INN_LK
        QString innFilter = QString::number(inn);
        innFilter = QString("%1=%2").arg(record().fieldName(Dosages::Constants::INN_LK)).arg(innFilter);
        // add INN_DOSAGE
        innFilter = QString("(%1) AND (%2='%3')")
                    .arg(innFilter)
                    .arg(record().fieldName(Dosages::Constants::InnLinkedDosage))
                    .arg(m_DrugsModel->drugData(uid, Constants::Drug::MainInnDosage).toString());
        filter = QString("((%1) OR (%2))").arg(filter).arg(innFilter);
    }        
    //    qWarning() << "filter" << filter;
    setFilter(filter);
    select();
    return true;
}

/** \brief Return the actual drug's CIS */
int DosageModel::drugUID()
{
    return m_UID;
}

/** \brief Test all the column of the selected row, if one value is dirty return true. */
bool DosageModel::isDirty(const int row) const
{
    int i;
    for(i=0;i<columnCount();++i) {
        if (QSqlTableModel::isDirty(index(row, i)))
            return true;
    }
    return false;
}

/** \brief Check the validity of the dosage. */
QStringList DosageModel::isDosageValid(const int row)
{
    Q_ASSERT_X(m_UID != -1, "DosageModel::isDosageValid", "before using the dosagemodel, you must specify the CIS of the related drug");
    QStringList errors;
    // Label
    if (index(row, Dosages::Constants::Label).data().toString().isEmpty())
        errors << tr("The protocol must be labelled.");
    // Intakes
    if (index(row, Dosages::Constants::IntakesScheme).data().toString().isEmpty())
        errors << tr("The intakes' scheme must be defined.");
    if (index(row, Dosages::Constants::PeriodScheme).data().toString().isEmpty())
        errors << tr("The period's scheme must be defined.");
    // Duration
    if (index(row, Dosages::Constants::DurationScheme).data().toString().isEmpty())
        errors << tr("The duration's scheme must be defined.");

    //TODO
    return errors;
}

#ifndef FREEDIAMS
/** \todo here */
///** \brief Test user's permissions for the drugs read access */
//bool DosageModel::userCanRead()
//{
//    //TODO --> test
//    User::UserRights r = User::UserRights(tkUserModel::instance()->currentUserData(User::DrugsRights).toInt());
//    return (r & User::ReadOwn) || (r & User::ReadAll);
//}
//
///** \brief Test user's permissions for the drugs write access */
//bool DosageModel::userCanWrite()
//{
//    //TODO  --> test
//    User::UserRights r = User::UserRights(tkUserModel::instance()->currentUserData(User::DrugsRights).toInt());
//    return (r & User::WriteOwn) || (r & User::WriteAll);
//}
#endif

/**
  \brief Transforms a dosage to a drug's prescription.
  \sa DrugsModel
*/
void DosageModel::toPrescription(const int row)
{
    Q_ASSERT(m_DrugsModel);
    Q_ASSERT(m_DrugsModel->containsDrug(m_UID));
    /** \todo add a mutext ? */
    QHash<int,int> prescr_dosage;
    prescr_dosage.insert(Constants::Prescription::UsedDosage ,           Dosages::Constants::Uuid);
    prescr_dosage.insert(Constants::Prescription::IntakesFrom ,          Dosages::Constants::IntakesFrom);
    prescr_dosage.insert(Constants::Prescription::IntakesTo ,            Dosages::Constants::IntakesTo);
    prescr_dosage.insert(Constants::Prescription::IntakesUsesFromTo ,    Dosages::Constants::IntakesUsesFromTo);
    prescr_dosage.insert(Constants::Prescription::IntakesScheme ,        Dosages::Constants::IntakesScheme);
    prescr_dosage.insert(Constants::Prescription::Period ,               Dosages::Constants::Period);
    prescr_dosage.insert(Constants::Prescription::PeriodScheme ,         Dosages::Constants::PeriodScheme);
    prescr_dosage.insert(Constants::Prescription::DurationFrom ,         Dosages::Constants::DurationFrom);
    prescr_dosage.insert(Constants::Prescription::DurationTo ,           Dosages::Constants::DurationTo);
    prescr_dosage.insert(Constants::Prescription::DurationUsesFromTo,    Dosages::Constants::DurationUsesFromTo);
    prescr_dosage.insert(Constants::Prescription::DurationScheme ,       Dosages::Constants::DurationScheme);
    prescr_dosage.insert(Constants::Prescription::IntakesIntervalOfTime ,Dosages::Constants::IntakesIntervalOfTime);
    prescr_dosage.insert(Constants::Prescription::IntakesIntervalScheme ,Dosages::Constants::IntakesIntervalScheme);
    prescr_dosage.insert(Constants::Prescription::Note ,                 Dosages::Constants::Note);
    prescr_dosage.insert(Constants::Prescription::DailyScheme ,          Dosages::Constants::DailyScheme);
    prescr_dosage.insert(Constants::Prescription::MealTimeSchemeIndex ,  Dosages::Constants::MealScheme);
    prescr_dosage.insert(Constants::Prescription::IsALD ,                Dosages::Constants::IsALD);
    foreach(const int i, prescr_dosage.keys()) {
        m_DrugsModel->setDrugData(m_UID, i, data(index(row, prescr_dosage.value(i))));
    }
    if (index(row,Dosages::Constants::INN_LK).data().toInt() > 999) // this is an INN prescription
        m_DrugsModel->setDrugData(m_UID, Constants::Prescription::IsINNPrescription, true);
    else
        m_DrugsModel->setDrugData(m_UID, Constants::Prescription::IsINNPrescription, false);
    m_DrugsModel->resetModel();
}

/** \brief Transforms a model's row into XML encoded QString. This part is using the database fieldnames.*/
QString DosageModel::toXml(const int row)
{
    int j;
    QHash<QString,QString> datas;
    for(j=0; j < columnCount(); ++j) {
        datas.insert(record().fieldName(j).toLower(), index(row, j).data().toString());
    }
    return Utils::createXml(XML_DOSAGE_MAINTAG,datas,4,false);
}

/**
  \brief Adds a XML file to the model. A verification of the drugs' CIS is done.
  \todo write this
*/
bool DosageModel::addFromXml(const QString &xml)
{
    /** \todo here */
    //    QHash<QString, QString> datas;
    //    int n = xml.count("<"+XML_DOSAGE_MAINTAG+">");
    //    if (n<=0)
    //        return false;
    //    foreach(const QString &s, xml.split("</"+XML_DOSAGE_MAINTAG+">")) {
    //        if (!Utils::readXml(xml,XML_DOSAGE_MAINTAG,datas,false))
    //            return false;
    //        // insert row, get the index of it
    //        foreach(const QString &k, datas.keys()) {
    //            int id = record().indexOf(k);
    //            if (id==-1)
    //                continue;
    //            //        setData(index(
    //        }
    //
    //    }
    return true;
}

/** \brief For debugging purpose only */
void DosageModel::warn(const int row)
{
    if (!Utils::isDebugCompilation())
        return;
    //    qWarning() << "database connection" << database().connectionName();

    // row == -1 -> warn all rows
    int i;
    int j;
    if (row == -1) {
        for (i=0; i < rowCount(); ++i)
            qWarning() << toXml(i);
        //            for(j=0; j < columnCount(); ++j) {
        //            qWarning() << record().fieldName(j) << index(i, j).data() ;
        //        }
    } else {
        for(j=0; j < columnCount(); ++j) {
            qWarning() << record().fieldName(j) << index(row, j).data() ;
        }
    }
}

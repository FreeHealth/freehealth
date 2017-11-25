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
/**
 * \class DrugsDB::DrugDrugInteractionCore
 * This class owns all the shared part of the DDI code part.\n
 * The single instance is create by the DrugsDB::DrugsDBCore object.
 * \n FreeToolBox specific class.
 */

#include "drugdruginteractioncore.h"
#include "drugdruginteraction.h"
#include "druginteractor.h"
#include "drugdruginteractiondatabasepopulator.h"
#include <drugsdb/drugsdbcore.h>
#include <drugsdb/pim/pimdatabasecreator.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>
#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <biblio/bibliocore.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/googletranslator.h>

#include <QMultiHash>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDir>
#include <QFile>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::DrugsDBCore *drugsDbCore() {return DrugsDB::DrugsDBCore::instance();}

static inline QString ddiNewXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString newInteractorsFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_INTERACTORS_FILENAME);}

static inline QString atcFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::ATC_FILENAME);}
static inline QString ddiFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString pimsFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::PIMS_FILENAME);}

using namespace DrugsDB;
using namespace Internal;

namespace DrugsDB {
namespace Internal {
class DrugDrugInteractionCorePrivate
{
public:
    DrugDrugInteractionCorePrivate(DrugDrugInteractionCore *parent) :
        m_InteractingClassesModel(0),
        m_InteractorsModel(0),
        m_DDIDbPopulator(0),
        m_PimDatabaseCreator(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~DrugDrugInteractionCorePrivate()
    {
    }

public:
    QHash<DrugDrugInteraction *, QDomNode> m_ddisToNode;
    QHash<DrugInteractor *, QDomNode> m_interactorsToNode;
    DrugInteractorModel *m_InteractingClassesModel, *m_InteractorsModel;
    DrugDrugInteractionDatabasePopulator *m_DDIDbPopulator;
    DrugsDB::Internal::PimDatabaseCreator *m_PimDatabaseCreator;

private:
    DrugDrugInteractionCore *q;
};
}  // namespace Internal
}  // namespace DrugsDB

/** Return the single instance of the object using the DrugsDB::DrugsDbCore instance */
DrugDrugInteractionCore *DrugDrugInteractionCore::instance()
{
    return drugsDbCore()->ddiCore();
}

DrugDrugInteractionCore::DrugDrugInteractionCore(QObject *parent) :
    QObject(parent),
    d(new Internal::DrugDrugInteractionCorePrivate(this))
{
    setObjectName("DrugDrugInteractionCore");
}

DrugDrugInteractionCore::~DrugDrugInteractionCore()
{
    if (d)
        delete d;
    d = 0;
}

bool DrugDrugInteractionCore::initialize()
{
    d->m_InteractorsModel = new DrugInteractorModel(DrugInteractorModel::InteractingMolecules, this);
    d->m_InteractingClassesModel = new DrugInteractorModel(DrugInteractorModel::InteractingClasses, this);
    d->m_InteractingClassesModel->setObjectName("DrugInteractorModel::Classes");
    d->m_InteractorsModel->setObjectName("DrugInteractorModel::Molecules");
    d->m_DDIDbPopulator = new DrugDrugInteractionDatabasePopulator(this);
    d->m_DDIDbPopulator->initialize();
    d->m_PimDatabaseCreator = new DrugsDB::Internal::PimDatabaseCreator(this);
    d->m_PimDatabaseCreator->initialize();
    return true;
}

/** Return a uuid integer */
int DrugDrugInteractionCore::createInternalUuid() const
{
    static int uuid = 0;
    return ++uuid;
}

/** Return the interacting molecule model */
Internal::DrugInteractorModel *DrugDrugInteractionCore::interactingMoleculesModel() const
{
    return d->m_InteractorsModel;
}

/** Return the interacting classes model */
Internal::DrugInteractorModel *DrugDrugInteractionCore::interactingClassesModel() const
{
    return d->m_InteractingClassesModel;
}

/** Read the DDI XML file if needed and return the list of created DDI. */
QList<Internal::DrugDrugInteraction *> DrugDrugInteractionCore::getDrugDrugInteractions() const
{
    if (d->m_ddisToNode.isEmpty()) {
        QDomDocument domDocument;
        // Read the file
        QFile file(ddiNewXmlFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                LOG_ERROR(tr("Can not read XML file content %1").arg(file.fileName()));
                LOG_ERROR(QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error));
            } else {
                LOG(tr("Reading file: %1").arg(file.fileName()));
            }
            file.close();
        } else {
            LOG_ERROR(tr("Can not open XML file %1").arg(file.fileName()));
        }

        // Create the m_ddisToNode content
        QDomElement root = domDocument.firstChildElement("DrugDrugInteractions");
        QDomElement ddiNode = root.firstChildElement("DDI");
        while (!ddiNode.isNull()) {
            DrugDrugInteraction *ddi = new DrugDrugInteraction(ddiNode);
            ddi->setData(DrugDrugInteraction::InternalUuid, createInternalUuid());
            d->m_ddisToNode.insert(ddi, ddiNode);
            ddiNode = ddiNode.nextSiblingElement("DDI");
        }
        LOG(tr("Getting %1 non duplicated drug-drug interactions").arg(d->m_ddisToNode.keys().count()));
    }
    return d->m_ddisToNode.keys();
}

/** Return true if the core can add the full ATC to any drugs database */
bool DrugDrugInteractionCore::canAddAtc() const
{
    return QFile(atcFile()).exists();
}

/** Return true if the core can add the DDI to any drugs database */
bool DrugDrugInteractionCore::canAddDrugDrugInteractions() const
{
    return QFile(ddiFile()).exists();
}

/** Return true if the core can add the potentially inappropriate medications to any drugs database */
bool DrugDrugInteractionCore::canAddPims() const
{
    return QFile(pimsFile()).exists();
}

/** Return true if the core can add the pregnancy checking data to any drugs database */
bool DrugDrugInteractionCore::canAddPregnancyChecking() const
{
    return true;
}

/** Read the DDI XML file if needed and return the list of created interactors. */
QList<Internal::DrugInteractor *> DrugDrugInteractionCore::getDrugInteractors() const
{
    if (d->m_interactorsToNode.isEmpty()) {
        QDomDocument domDocument;
        // Read the file
        QFile file(newInteractorsFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                LOG_ERROR(tr("Can not read XML file content %1").arg(file.fileName()));
                LOG_ERROR(QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error));
            } else {
                LOG(tr("Reading file: %1").arg(file.fileName()));
            }
            file.close();
        } else {
            LOG_ERROR(tr("Can not open XML file %1").arg(file.fileName()));
        }

        // Create the m_ddisToNode content
        QDomElement root = domDocument.firstChildElement("DDI_Interactors");
        QDomElement iNode = root.firstChildElement("I");
        QHash<QString, DrugInteractor *> initialLabelToDI;
        while (!iNode.isNull()) {
            DrugInteractor *di = new DrugInteractor(iNode);
            di->setData(DrugInteractor::Id, createInternalUuid());
            d->m_interactorsToNode.insert(di, iNode);
            initialLabelToDI.insert(di->data(DrugInteractor::Uid).toString(), di);
            iNode = iNode.nextSiblingElement("I");
        }

        // reparent items
        for(int i=0;i<d->m_interactorsToNode.count();++i) {
            DrugInteractor *di = d->m_interactorsToNode.keys().at(i);
            const QString &parentUid = di->data(DrugInteractor::Uid).toString();

            foreach(const QString &child, di->childrenIds()) {
                DrugInteractor *childInteractor = initialLabelToDI.value(child, 0);
                if (childInteractor) {
                    childInteractor->addParentId(parentUid);
                }
            }
        }

        LOG(tr("Getting %1 interactors and interacting classes").arg(d->m_interactorsToNode.keys().count()));
    }
    return d->m_interactorsToNode.keys();
}

/** Test if database is populated with ATC data */
bool DrugDrugInteractionCore::isAtcInstalledInDatabase(DrugsDB::Internal::DrugBaseEssentials *database)
{
    return d->m_DDIDbPopulator->isAtcInstalledInDatabase(database);
}

/** Add the full ATC to the drugs database */
bool DrugDrugInteractionCore::addAtcDataToDatabase(DrugsDB::Internal::DrugBaseEssentials *database)
{
    if (!d->m_DDIDbPopulator->saveAtcClassification(database))
        return false;
    return true;
}

/** Populate a drug database with:
 * - the ATC data
 * - DDI data
 * \sa DrugsDB::DrugsDbCore::addInteractionData(), IAMDB::Internal::DrugDrugInteractionDatabasePopulator
 */
bool DrugDrugInteractionCore::populateDrugDatabase(DrugsDB::Internal::DrugBaseEssentials *database)
{
    if (!d->m_DDIDbPopulator->saveDrugDrugInteractions(database))
        return false;
    if (!d->m_PimDatabaseCreator->populateDatabaseWithPims(database))
        return false;
    return true;
}

/** Add the DDIs to the drugs database */
bool DrugDrugInteractionCore::addDrugDrugInteractionsToDatabase(DrugsDB::Internal::DrugBaseEssentials *database)
{
    if (!d->m_DDIDbPopulator->saveDrugDrugInteractions(database))
        return false;
    return true;
}

/** Add the PIMs to the drugs database */
bool DrugDrugInteractionCore::addPimsToDatabase(DrugsDB::Internal::DrugBaseEssentials *database)
{
    if (!d->m_PimDatabaseCreator->populateDatabaseWithPims(database))
        return false;
    return true;
}

/** Add the pregnancy checking to the drugs database */
bool DrugDrugInteractionCore::addPregnancyCheckingDataToDatabase(DrugsDB::Internal::DrugBaseEssentials *database)
{
//    if (!d->m_DDIDbPopulator->saveAtcClassification(database))
//        return false;
    return true;
}

/** Update the XML file for the specified DrugDrugInteraction pointer. This pointer should be extracted from the list created using the getDrugDrugInteractions(). \sa getDrugDrugInteractions() */
void DrugDrugInteractionCore::updateXmlFileForDrugDrugInteraction(Internal::DrugDrugInteraction */*ddi*/)
{
    // Populate the DomNode
    // TODO: code here ??? */
}

/** Will overwrite the thesaurus file with a new one created on the basis of the \e ddis. All precedent data will be lost. */
void DrugDrugInteractionCore::saveCompleteList(const QList<Internal::DrugDrugInteraction *> &ddis)
{
    QString xml = "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- date format = yyyy-MM-dd -->\n"
            "<!--\n"
            "  Interaction levels :\n"
            "     P: Precaution\n"
            "     C: ContreIndication\n"
            "     D: Deconseille\n"
            "     T: APrendreEnCompte Take into Account\n"
            "     450: P450\n"
            "     I: Information\n"
            "     Y: Glycoprotein P\n"
            "-->\n"
            "<DrugDrugInteractions>\n";
    for(int i=0; i < ddis.count(); ++i) {
        xml += ddis.at(i)->toXml();
    }
    xml += "\n</DrugDrugInteractions>\n";
    Utils::saveStringToFile(xml, ddiNewXmlFile(), Utils::Overwrite, Utils::DontWarnUser);
}

/** Will overwrite the thesaurus file with a new one created on the basis of the \e interactors. All precedent data will be lost. */
void DrugDrugInteractionCore::saveCompleteList(const QList<Internal::DrugInteractor *> &interactors)
{
    QString xml = "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- date format = yyyy-MM-dd -->\n"
            "<DDI_Interactors>\n";
    for(int i=0; i < interactors.count(); ++i) {
        xml += interactors.at(i)->toXml();
    }
    xml += "\n</DDI_Interactors>\n";
    Utils::saveStringToFile(xml, newInteractorsFile(), Utils::Overwrite, Utils::DontWarnUser);
}

/** Create a new DrugInteractor with the \e initialLabel as \e isClass. */
DrugInteractor *DrugDrugInteractionCore::createNewInteractor(const QString &initialLabel, const bool isClass)
{
    DrugInteractor *di = 0;
    if (isClass)
        di = d->m_InteractorsModel->createInteractingClass(initialLabel);
    else
        di = d->m_InteractorsModel->createInteractor(initialLabel);
    return di;
}

/** Start the download of all needed pubmed references and store to the BiblioPlugin database. */
void DrugDrugInteractionCore::downloadAllPmids()
{
    // get all pmids to download
    QStringList pmids;
    foreach(DrugDrugInteraction *ddi, getDrugDrugInteractions()) {
        pmids.append(ddi->data(DrugDrugInteraction::PMIDsStringList).toStringList());
    }
    foreach(DrugInteractor *di, getDrugInteractors()) {
        pmids.append(di->data(DrugInteractor::PMIDsStringList).toStringList());
        if (di->isClass())
            pmids.append(di->allNeededPMIDs());
    }
    pmids.removeAll("");
    pmids.removeDuplicates();
    Biblio::BiblioCore::instance()->downloadPubMedData(pmids);
}

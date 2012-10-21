/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugdruginteractioncore.h"
#include "drugdruginteraction.h"
#include "druginteractor.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

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

using namespace IAMDb;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

static inline QString ddiNewXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString newInteractorsFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_INTERACTORS_FILENAME);}


using namespace IAMDb;

DrugDrugInteractionCore *DrugDrugInteractionCore::m_Instance = 0;
DrugDrugInteractionCore *DrugDrugInteractionCore::instance()
{
    if (!m_Instance) {
        m_Instance = new DrugDrugInteractionCore;
    }
    return m_Instance;
}

DrugDrugInteractionCore::DrugDrugInteractionCore(QObject *parent) :
    QObject(parent)
{
    setObjectName("DrugDrugInteractionCore");
}

int DrugDrugInteractionCore::createInternalUuid() const
{
    static int uuid = 0;
    return ++uuid;
}

/** Read the DDI XML file if needed and return the list of created DDI. */
QList<DrugDrugInteraction *> DrugDrugInteractionCore::getDrugDrugInteractions() const
{
    if (m_ddisToNode.isEmpty()) {
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
            m_ddisToNode.insert(ddi, ddiNode);
            ddiNode = ddiNode.nextSiblingElement("DDI");
        }
        LOG(tr("Getting %1 non duplicated drug-drug interactions").arg(m_ddisToNode.keys().count()));
    }
    return m_ddisToNode.keys();
}

QList<DrugInteractor *> DrugDrugInteractionCore::getDrugInteractors() const
{
    if (m_interactorsToNode.isEmpty()) {
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
            m_interactorsToNode.insert(di, iNode);
            initialLabelToDI.insert(di->data(DrugInteractor::InitialLabel).toString(), di);
            iNode = iNode.nextSiblingElement("I");
        }

        // reparent items
        for(int i=0;i<m_interactorsToNode.count();++i) {
            DrugInteractor *di = m_interactorsToNode.keys().at(i);
            const QString &parentInitialLabel = di->data(DrugInteractor::InitialLabel).toString();

            foreach(const QString &child, di->childrenIds()) {
                DrugInteractor *childInteractor = initialLabelToDI.value(child, 0);
                if (childInteractor) {
                    childInteractor->addParentId(parentInitialLabel);
                }
            }
        }

        LOG(tr("Getting %1 interactors and interacting classes").arg(m_interactorsToNode.keys().count()));
    }
    return m_interactorsToNode.keys();
}

/** Update the XML file for the specified DrugDrugInteraction pointer. This pointer should be extracted from the list created using the getDrugDrugInteractions(). \sa getDrugDrugInteractions() */
void DrugDrugInteractionCore::updateXmlFileForDrugDrugInteraction(DrugDrugInteraction */*ddi*/)
{
    // Populate the DomNode
    // TODO: code here ??? */
}

/** Will overwrite the thesaurus file with a new one created on the basis of the \e ddis. All precedent data will be lost. */
void DrugDrugInteractionCore::saveCompleteList(const QList<DrugDrugInteraction *> &ddis)
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
void DrugDrugInteractionCore::saveCompleteList(const QList<DrugInteractor *> &interactors)
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
    DrugInteractor *di = new DrugInteractor;
    di->setData(DrugInteractor::IsValid, true);
    di->setData(DrugInteractor::InitialLabel, Utils::removeAccents(initialLabel.toUpper()));
    di->setData(DrugInteractor::FrLabel, initialLabel.toUpper());
    di->setData(DrugInteractor::IsClass, isClass);
    di->setData(DrugInteractor::DateOfCreation, QDate::currentDate());
    di->setData(DrugInteractor::IsDuplicated, false);
    Q_EMIT interactorCreated(di);
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

/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "drugdruginteractioncore.h"
#include "drugdruginteraction.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

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

static inline QString afssapsNewIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::NEW_AFSSAPS_INTERACTIONS_FILENAME);}


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
        QFile file(afssapsNewIamXmlFile());
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

/** Update the XML file for the specified DrugDrugInteraction pointer. This pointer should be extracted from the list created using the getDrugDrugInteractions(). \sa getDrugDrugInteractions() */
void DrugDrugInteractionCore::updateXmlFileForDrugDrugInteraction(DrugDrugInteraction *ddi)
{
    // Populate the DomNode

}

/** While overwrite the thesaurus file with a new one created on the basis of the \e ddis. All precedent datas will be lost. */
void DrugDrugInteractionCore::saveCompleteList(QList<DrugDrugInteraction *> ddis)
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
    Utils::saveStringToFile(xml, afssapsNewIamXmlFile(), Utils::Overwrite, Utils::DontWarnUser);
}

/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class DataPack::ServerContent
  Content of a datapack server including:
  - pack filenames
*/

#include "servercontent.h"

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QDomDocument>
#include <QDomElement>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

//<ServerContents>
//  <Pack serverFileName=""/>
//</ServerContents>

namespace {
const char * const TAG_SERVERCONTENT = "ServerContents";
const char * const TAG_PACK = "Pack";
const char * const ATTRIB_FILENAME = "serverFileName";
}

ServerContent::ServerContent()
{
}

void ServerContent::clear()
{
    m_PackFileNames.clear();
}

bool ServerContent::fromXml(const QString &xml)
{
    m_PackFileNames.clear();
    QDomDocument doc;
    QString error;
    int line, col;
    if (!doc.setContent(xml, &error, &line, &col)) {
        LOG_ERROR_FOR("DataPack::Pack", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
        return false;
    }
    QDomElement root = doc.firstChildElement(::TAG_SERVERCONTENT);
    return fromDomElement(root);
}

bool ServerContent::fromDomElement(const QDomElement &root)
{
    if (root.tagName().compare(::TAG_SERVERCONTENT, Qt::CaseInsensitive)!=0) {
        LOG_ERROR_FOR("ServerContent", "Wrong XML. No root tag: " + QString(::TAG_SERVERCONTENT));
        return false;
    }

    QDomElement pack = root.firstChildElement(::TAG_PACK);

    while (!pack.isNull()) {
        m_PackFileNames << pack.attribute(::ATTRIB_FILENAME);
        pack = pack.nextSiblingElement(::TAG_PACK);
    }
    return true;
}

bool ServerContent::toXml(QDomElement *root, QDomDocument *doc)
{
    QDomElement content = doc->createElement(::TAG_SERVERCONTENT);
    if (!root)
        doc->appendChild(content);
    else
        root->appendChild(content);

    foreach(const QString &pack, m_PackFileNames) {
        QDomElement p = doc->createElement(::TAG_PACK);
        p.setAttribute(::ATTRIB_FILENAME, pack);
        content.appendChild(p);
    }
    return true;
}

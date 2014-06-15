/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "packdescription.h"

#include <utils/log.h>

#include <QDomElement>
#include <QDomDocument>

using namespace DataPack;

namespace {
const char * const TAG_DEPENDENCIES_ROOT_TAG = "PackDependencies";
const char * const TAG_DEPENCENCY_TAG = "Dependency";

const char * const ATTRIB_TYPE = "t";
const char * const ATTRIB_NAME = "n";
const char * const ATTRIB_UUID = "u";
const char * const ATTRIB_VERSION = "v";

const char * const TYPE_DEPENDS      = "depends";
const char * const TYPE_RECOMMENDS   = "recommends";
const char * const TYPE_SUGGESTS     = "suggests";
const char * const TYPE_REQUIRES     = "requires";
const char * const TYPE_CONFLICTS    = "conflicts";
const char * const TYPE_BREAKS       = "breaks";
const char * const TYPE_PROVIDES     = "provides";

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////   PackDependencyData   //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PackDependencyData::PackDependencyData()
{}

PackDependencyData::~PackDependencyData()
{}


bool PackDependencyData::operator<(const PackDependencyData &to)
{
    return type() < to.type();
}

QString PackDependencyData::typeName(int typeReference)
{
    switch (typeReference) {
    case Depends: return ::TYPE_DEPENDS;
    case Recommends: return ::TYPE_RECOMMENDS;
    case Suggests: return ::TYPE_SUGGESTS;
    case Requires: return ::TYPE_REQUIRES;
    case Conflicts: return ::TYPE_CONFLICTS;
    case Breaks: return ::TYPE_BREAKS;
    case Provides: return ::TYPE_PROVIDES;
    default: return QString::null;
    }
    return QString::null;
}

int PackDependencyData::typeFromName(const QString &name)
{
    if (name.compare(::TYPE_BREAKS, Qt::CaseInsensitive)==0)
        return Breaks;
    if (name.compare(::TYPE_CONFLICTS, Qt::CaseInsensitive)==0)
        return Conflicts;
    if (name.compare(::TYPE_DEPENDS, Qt::CaseInsensitive)==0)
        return Depends;
    if (name.compare(::TYPE_PROVIDES, Qt::CaseInsensitive)==0)
        return Provides;
    if (name.compare(::TYPE_RECOMMENDS, Qt::CaseInsensitive)==0)
        return Recommends;
    if (name.compare(::TYPE_REQUIRES, Qt::CaseInsensitive)==0)
        return Requires;
    if (name.compare(::TYPE_SUGGESTS, Qt::CaseInsensitive)==0)
        return Suggests;
    return -1;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////   PackDependencies   ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PackDependencies::PackDependencies()
{}

PackDependencies::~PackDependencies()
{}

const PackDependencyData &PackDependencies::at(const int index) const
{
    return dependencies[index];
}

bool PackDependencies::fromDomElement(const QDomElement &root)
{
    dependencies.clear();
    if (root.isNull())
        return true;
    if (root.tagName().compare(::TAG_DEPENDENCIES_ROOT_TAG, Qt::CaseInsensitive)!=0) {
        LOG_ERROR_FOR("DataPack::PackDependencies", "Wrong root tag: " + QString(::TAG_DEPENDENCIES_ROOT_TAG));
        return false;
    }
    QDomElement dep = root.firstChildElement(::TAG_DEPENCENCY_TAG);
    while (!dep.isNull()) {
        PackDependencyData data;
        data.setType(PackDependencyData::typeFromName(dep.attribute(::ATTRIB_TYPE)));
        data.setName(dep.attribute(::ATTRIB_NAME));
        data.setVersion(dep.attribute(::ATTRIB_VERSION));
        data.setUuid(dep.attribute(::ATTRIB_UUID));
        dependencies << data;
        dep = dep.nextSiblingElement(::TAG_DEPENCENCY_TAG);
    }
    return true;
}

bool PackDependencies::toDomElement(QDomElement *root, QDomDocument *doc) const
{
    Q_ASSERT(root);
    Q_ASSERT(doc);
    if (!root || !doc)
        return false;
    QDomElement depRoot;
    bool includeToRoot = false;
    if (root->tagName().compare(::TAG_DEPENDENCIES_ROOT_TAG)!=0) {
        depRoot = doc->createElement(::TAG_DEPENDENCIES_ROOT_TAG);
        root->appendChild(depRoot);
    } else {
        includeToRoot = true;
    }
    for(int i = 0; i < dependencies.count(); ++i) {
        QDomElement dep = doc->createElement(::TAG_DEPENCENCY_TAG);
        const PackDependencyData &data = dependencies.at(i);
        dep.setAttribute(::ATTRIB_TYPE, PackDependencyData::typeName(data.type()));
        dep.setAttribute(::ATTRIB_NAME, data.name());
        dep.setAttribute(::ATTRIB_UUID, data.uuid());
        dep.setAttribute(::ATTRIB_VERSION, data.version());
        if (includeToRoot)
            root->appendChild(dep);
        else
            depRoot.appendChild(dep);
    }
    return true;
}

QString PackDependencies::toXml() const
{
    QDomDocument doc;
    // Create the main description tag
    QDomElement root = doc.createElement(::TAG_DEPENDENCIES_ROOT_TAG);
    doc.appendChild(root);
    toDomElement(&root, &doc);
    return doc.toString(2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////   PackDescription   ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PackDescription::PackDescription() :
    Utils::GenericDescription("PackDescription")
{
    addNonTranslatableExtraData(Size, "size");
    addNonTranslatableExtraData(UnzipToPath, "unziptopath");
    addNonTranslatableExtraData(Md5, "md5");
    addNonTranslatableExtraData(Sha1, "sha1");
    addNonTranslatableExtraData(DataType, "datatype");
    addNonTranslatableExtraData(InstalledFiles, "instfiles");
}

bool PackDescription::setData(const int ref, const QVariant &value, const QString &lang)
{
    // Unusure the UUID is correct
    if (ref == Uuid) {
        QRegExp reg("[^a-zA-Z0-9._]");
        QString uuid = value.toString();
        return Utils::GenericDescription::setData(ref, uuid.replace(reg, "_"), lang);
    }
    return Utils::GenericDescription::setData(ref, value, lang);
}

QString PackDescription::toXml() const
{
    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<!DOCTYPE FreeMedForms>\n"
            "<DataPack_Pack>\n";
    xml += Utils::GenericDescription::toXml();
    xml += "</DataPack_Pack>\n";
    return xml;
}

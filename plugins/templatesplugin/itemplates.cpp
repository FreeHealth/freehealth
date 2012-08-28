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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "itemplates.h"
#include "constants.h"

#include <utils/global.h>
#include <utils/serializer.h>

#include <QMimeData>
#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QVariant>

enum { base64MimeDatas = true };

using namespace Templates;

//static void parseMimeData(const QDomElement &element, const QModelIndex &parent, const int row, QAbstractItemModel *model)
//{
//    static int indent = 0;
//    indent += 2;
////    qWarning() << QString().fill(' ', indent) + "parse ; add to modelIndex" << parent.data().toString();
//    QDomNode n = element;
//    QDomNode child = element;
//    QModelIndex newItem = parent;
//    int zz = 0;
//
//    while (!n.isNull()) {
//        QDomElement e = n.toElement();
//        if (!e.isNull()) {
////            qWarning() << QString().fill(' ', indent) + "  " + e.tagName() + ": " + e.text().mid(14, e.text().indexOf(")#", 14) - 14) << e.childNodes().count();
//            // Manage templates
//            if (e.tagName().compare("Template", Qt::CaseInsensitive) == 0) {
//                QHash<int, QVariant> data = Utils::Serializer::toVariantHash(e.attribute("content"), base64MimeDatas);
////                qWarning() << e.attribute("content");
////                qWarning() << data;
//                data.insert(TemplatesModel::Data_ParentId, model->index(parent.row(), TemplatesModel::Data_Id, parent.parent()).data());
//                model->insertRow(row+zz, parent);
//                for(int i=0; i<TemplatesModel::Data_Max_Param; ++i) {
//                    model->setData(model->index(row+zz, i, parent), data.value(i, QVariant()));
//                }
//                ++zz;
//            }
//            if (e.tagName().compare("Child", Qt::CaseInsensitive) == 0) {
////                qWarning() << QString().fill(' ', indent) + "    -> Child";
//                newItem = model->index(0, 0, newItem);
//                parseMimeData(child.firstChildElement(), newItem, 0, model);
//                child = child.firstChildElement("Child");
//            }
//            // Manage all children
//            child = n.firstChildElement("Child");
//            while (!child.isNull()) {
////                qWarning() << QString().fill(' ', indent) + "    -> Manage Child";
//                newItem = model->index(0, 0, newItem);
//                parseMimeData(child.firstChildElement("Template"), newItem, 0, model);
//                child = child.firstChildElement("Child");
//            }
//        }
//        n = n.nextSibling();
//    }
//
//}

QMimeData *ITemplate::toMimeData()
{
    return 0;
}

bool ITemplate::fromMimeData(QMimeData *)
{
    return false;
}

QString ITemplate::serialize()
{
    return Utils::Serializer::toString(m_Data, base64MimeDatas);
}

bool ITemplate::deserialize(const QString &serialized)
{
    m_Data.clear();
    m_Data = Utils::Serializer::toVariantHash(serialized, base64MimeDatas);
    return true;
}

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
 ***************************************************************************/
#include "icdio.h"
#include "icdcollectionmodel.h"

#include <utils/log.h>

#include <QHash>
#include <QString>
#include <QDomDocument>


using namespace ICD;

namespace ICD {

    namespace Constants {

        const char * const XML_HEADER = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        const char * const XML_MAINTAG = "IcdCollection";
        const char * const XML_TAG_CODE = "IcdCode";
        const char * const XML_TAG_CODEASSOCIATION = "IcdAssociation";

        const char * const XML_ATTRIB_XMLVERSION = "xmlversion";
        const char * const XML_ATTRIB_DATABASEVERSION = "dbversion";
        const char * const XML_ATTRIB_DATE = "date";

        const char * const XML_ATTRIB_ICDCODE = "code";
        const char * const XML_ATTRIB_ICDLABEL = "label";
        const char * const XML_ATTRIB_DATABASECODESID = "sid";

        const char * const XML_ATTRIB_DAGET = "daget";
        const char * const XML_ATTRIB_DAGCODE = "dagetcode";
        const char * const XML_ATTRIB_TYPEINASSOCIATION = "is";

        const char * const XML_VALUE_ASSO_ISMAIN = "main";
        const char * const XML_VALUE_ASSO_ISASSOCIATED = "associated";

    }  // End namespace Constants


namespace Internal {

class IcdIOPrivate {
public:

    void populateHash()
    {
        if (m_XmlAttribs.isEmpty()) {
            m_XmlAttribs.insert(IcdCollectionModel::CodeWithoutDaget, Constants::XML_ATTRIB_ICDCODE);
            m_XmlAttribs.insert(IcdCollectionModel::Label,   Constants::XML_ATTRIB_ICDLABEL);
            m_XmlAttribs.insert(IcdCollectionModel::HumanReadableDaget, Constants::XML_ATTRIB_DAGET);
            m_XmlAttribs.insert(IcdCollectionModel::DagCode, Constants::XML_ATTRIB_DAGCODE);
            m_XmlAttribs.insert(IcdCollectionModel::SID,     Constants::XML_ATTRIB_DATABASECODESID);
        }
    }

    QString modelRowToXml(const IcdCollectionModel *model, int row, const QModelIndex &parent)
    {
        QString xmlRow, xmlChild;
        bool hasChild = false;
        bool hasParent = false;
        if (parent==QModelIndex()) {
            if (model->hasChildren(model->index(row,0))) {
                xmlChild = modelRowToXml(model, 0, model->index(row, 0));
                hasChild = true;
            }
        } else {
            hasParent = true;
        }

        // Get XML attribs == columns of model
        populateHash();
        for(int i = 0; i < model->columnCount();++i) {
            if (i==IcdCollectionModel::CodeWithDaget) {
                continue;
            }
            const QString &modelValue = model->index(row, i, parent).data().toString();
            if (modelValue.isEmpty())
                continue;
            xmlRow += QString("%1=\"%2\" ").arg(m_XmlAttribs.value(i)).arg(modelValue);
        }

        // Add the type of associated code
        if (hasChild) {
            xmlRow +=  QString("%1=\"%2\" ").arg(Constants::XML_ATTRIB_TYPEINASSOCIATION, Constants::XML_VALUE_ASSO_ISMAIN);
        } else if (hasParent) {
            xmlRow +=  QString("%1=\"%2\" ").arg(Constants::XML_ATTRIB_TYPEINASSOCIATION, Constants::XML_VALUE_ASSO_ISASSOCIATED);
        }

        if (!xmlChild.isEmpty()) {
            xmlRow = QString("<%1 %2/>").arg(Constants::XML_TAG_CODE, xmlRow);
            xmlRow = QString("<%1>"
                             "%2"
                             "%3"
                             "</%1>")
                    .arg(Constants::XML_TAG_CODEASSOCIATION)
                    .arg(xmlRow, xmlChild);
        } else {
            xmlRow = QString("<%1 %2/>").arg(Constants::XML_TAG_CODE, xmlRow);
        }

        return xmlRow;
    }

    static QHash<int, QString> m_XmlAttribs;
};

}  // End namespace Internal
}  // End namespace ICD

QHash<int, QString> ICD::Internal::IcdIOPrivate::m_XmlAttribs;

IcdIO::IcdIO() :
        d(new Internal::IcdIOPrivate)
{
}

IcdIO::~IcdIO()
{
    if (d)
        delete d;
    d = 0;
}

QString IcdIO::icdCollectionToXml(const IcdCollectionModel *model)
{
    Q_ASSERT(model);
    if (!model) {
        Utils::Log::addError("IcdIO", "toXml: No model", __FILE__, __LINE__);
        return QString();
    }
    QString xml;
    for(int i=0; i < model->rowCount(); ++i) {
        xml += d->modelRowToXml(model, i, QModelIndex());
    }

    // Adding maintag + dbversion + date
    xml = QString("<%1 %2=\"%3\" %4=\"%5\">"
                  "%6"
                  "</%1>")
            .arg(Constants::XML_MAINTAG)
            /** \todo Add database version */
            .arg(Constants::XML_ATTRIB_DATABASEVERSION).arg("VERSION")
            .arg(Constants::XML_ATTRIB_DATE).arg(QDateTime::currentDateTime().toString(Qt::ISODate))
            .arg(xml);
    xml.prepend(Constants::XML_HEADER);

    // Beautifying the XML
    QDomDocument root;
    root.setContent(xml);

    return root.toString(2);
}

bool IcdIO::icdCollectionFromXml(IcdCollectionModel *model)
{
}

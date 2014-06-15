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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "icdio.h"
#include "icdcollectionmodel.h"
#include "icddatabase.h"
#include "icdassociation.h"

#include <utils/log.h>

#include <QHash>
#include <QString>
#include <QDomDocument>

static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}

using namespace ICD;

namespace ICD {

    namespace Constants {

        const char * const XML_HEADER = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        // FIXME: unused? const char * const XML_GLOBALTAG = "FreeMedForms";
        const char * const XML_MAINTAG = "IcdCollection";
        const char * const XML_TAG_CODE = "IcdCode";
        const char * const XML_TAG_CODEASSOCIATION = "IcdAssociation";

        // FIXME: unused? const char * const XML_ATTRIB_XMLVERSION = "xmlversion";
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


        const char * const HTML_HEADER = "<ol>";
        const char * const HTML_FOOTER = "</ol>";

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
            const QModelIndex &current = model->index(row, 0);
            if (model->hasChildren(current)) {
                int i = 0;
                while (model->hasIndex(i, 0, current)) {
                    xmlChild += modelRowToXml(model, i, current);
                    ++i;
                }
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

    QString modelRowToHtml(const IcdCollectionModel *model, int row, const QModelIndex &parent)
    {
        QString htmlRow, htmlChild;
//        bool hasChild = false;
//        bool hasParent = false;
        if (parent==QModelIndex()) {
            const QModelIndex &current = model->index(row, 0);
            if (model->hasChildren(current)) {
                int i = 0;
                while (model->hasIndex(i, 0, current)) {
                    htmlChild += modelRowToHtml(model, i, current);
                    ++i;
                }
                htmlChild = QString("<ol type=i>%1</ol>").arg(htmlChild);
//                hasChild = true;
            }
//        } else {
//            hasParent = true;
        }

        const QString &code = model->index(row, IcdCollectionModel::CodeWithDaget, parent).data().toString();
        const QString &label = model->index(row, IcdCollectionModel::Label, parent).data().toString();
        htmlRow = QString("<li>%1 - %2").arg(code, label);
        htmlRow += htmlChild;
        return htmlRow;
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

/** Return true if the ICD10 database is available and initialized */
bool IcdIO::isDatabaseInitialized()
{
    return icdBase()->isInitialized();
}

/** Return the XML translation of a ICD::IcdCollectionModel \e model */
QString IcdIO::icdCollectionToXml(const IcdCollectionModel *model)
{
    Q_ASSERT(model);
    if (!model) {
        LOG_ERROR_FOR("IcdIO", "toXml: No model");
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
            .arg(Constants::XML_ATTRIB_DATABASEVERSION).arg(icdBase()->getDatabaseVersion())
            .arg(Constants::XML_ATTRIB_DATE).arg(QDateTime::currentDateTime().toString(Qt::ISODate))
            .arg(xml);
    xml.prepend(Constants::XML_HEADER);

    // Beautifying the XML
    QDomDocument root;
    root.setContent(xml);

    return root.toString(2);
}

/** Populate a ICD::IcdCollectionModel \e model using the XML ICD10 collection \e xml according to the \e management method. */
bool IcdIO::icdCollectionFromXml(IcdCollectionModel *model, const QString &xml, const ModelManagement management)
{
    Q_ASSERT(model);
    if (!model) {
        LOG_ERROR_FOR("IcdIO", "fromXml: No model");
        return false;
    }
    if (xml.isEmpty()) {
        model->clear();
        return true;
    }

    QDomDocument root;
    root.setContent(xml);
    QDomElement element = root.firstChildElement(Constants::XML_MAINTAG);
    if (element.isNull()) {
        LOG_ERROR_FOR("IcdIO", "No XML main tag");
        return false;
    }

    // Get XML db version
    QString dbVersion = element.attribute(Constants::XML_ATTRIB_DATABASEVERSION);

    // Check XML db version with actual db version
    if (dbVersion!=icdBase()->getDatabaseVersion()) {
        // TODO: Update XML if needed
        LOG_FOR("IcdIO", QString("XML version (%1) different from db version (%2).")
                .arg(dbVersion).arg(icdBase()->getDatabaseVersion()));
    }

    // Manage model insertion mode
    if (management == ReplaceModelContent) {
        model->clearCollection();
    }

    // Read the document
    element = element.firstChildElement();
    for (; !element.isNull(); element = element.nextSiblingElement()) {
        // Tag == IcdCode --> add code
        if (element.tagName().compare(Constants::XML_TAG_CODE)==0) {
            model->addCode(element.attribute(Constants::XML_ATTRIB_DATABASECODESID).toInt());
        } else if (element.tagName().compare(Constants::XML_TAG_CODEASSOCIATION)==0) {
            // Tag == IcdAssociation --> add children
            int mainSid = 0;
            QVector<int> allSids;
            for (QDomElement child = element.firstChildElement(Constants::XML_TAG_CODE); !child.isNull(); child = child.nextSiblingElement()) {
                if (child.attribute(Constants::XML_ATTRIB_TYPEINASSOCIATION).compare(Constants::XML_VALUE_ASSO_ISMAIN)==0) {
                    mainSid = child.attribute(Constants::XML_ATTRIB_DATABASECODESID).toInt();
                } else {
                    allSids << child.attribute(Constants::XML_ATTRIB_DATABASECODESID).toInt();
                }
            }
            if (mainSid==0) {
                LOG_ERROR_FOR("IcdIO", "Wrong association, no main code");
                continue;
            }
            for(int i=0; i<allSids.count();++i) {
                model->addAssociation(icdBase()->getAssociation(mainSid, allSids.at(i)));
            }
        }
    }

    return true;
}

/** Return the HTML output of an ICD::IcdCollectionModel \e model */
QString IcdIO::icdCollectionToHtml(const IcdCollectionModel *model)
{
    Q_ASSERT(model);
    if (!model) {
        LOG_ERROR_FOR("IcdIO", "toHtml: No model");
        return QString();
    }
    QString html;
    if (model->isCollectionSimpleList()) {
        html += "<p style=\"font-weigt:bold\">Simple ICD10 code list.</p>";
    } else {
        html += "<p style=\"font-weigt:bold\">Full ICD10 code collection.</p>";
    }
    for(int i=0; i < model->rowCount(); ++i) {
        html += d->modelRowToHtml(model, i, QModelIndex());
    }

    // Adding maintag + dbversion + date
    html = QString("%1 %2 %3")
           .arg(Constants::HTML_HEADER)
           .arg(html)
           .arg(Constants::HTML_FOOTER);

    return html;
}

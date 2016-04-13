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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef XMLFORMCONTENTREADER_H
#define XMLFORMCONTENTREADER_H

#include <xmlioplugin/xmlformname.h>
#include <formmanagerplugin/formioquery.h>

#include <QString>
#include <QStringList>
#include <QCache>
#include <QHash>
#include <QDomDocument>
#include <QDomElement>

/**
 * \file xmlformcontentreader.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 10 Nov 2011
*/

namespace Category {
class CategoryItem;
}

namespace Form {
class FormItem;
class FormMain;
class IFormWidgetFactory;
class FormIODescription;
class FormIOQuery;
}

namespace XmlForms {
namespace Internal {

class XmlFormContentReader
{
    XmlFormContentReader();
public:
    static XmlFormContentReader *instance();
    ~XmlFormContentReader();
    bool initialize();

    void clearCache();
    QStringList cachedForms() {return m_DomDocFormCache.keys();}

    void refreshPluginFactories();

    bool isInCache(const QString &formAbsPath) const;
    QDomDocument *fromCache(const QString &formAbsPath) const;

    void warnXmlReadError(bool muteUserWarnings, const QString &file, const QString &msg, const int line = -1, const int col = -1) const;

    QString lastError() const {return m_Error.join("\n");}

    bool checkFileContent(const QString &formUidOrFullAbsPath, const QString &contents) const;

    QMultiHash<QString, QString> readUuidEquivalence(const QDomDocument *doc) const;
    Form::FormIODescription *readXmlDescription(const QDomElement &xmlDescr, const XmlFormName &form);
    Form::FormIODescription *readFileInformation(const XmlFormName &form, const Form::FormIOQuery &query = Form::FormIOQuery());

    QList<Form::FormIODescription *> getFormFileDescriptions(const Form::FormIOQuery &query);

    bool loadForm(const XmlFormName &form, Form::FormMain *rootForm);

    bool loadElement(Form::FormItem *item, QDomElement &rootElement, const XmlFormName &form);
    bool createElement(Form::FormItem *item, QDomElement &element, const XmlFormName &form);

    bool populateValues(Form::FormItem *item, const QDomElement &root, const XmlFormName &form);
    bool populateScripts(Form::FormItem *item, const QDomElement &root, const XmlFormName &form);
    bool addFile(const QDomElement &element, const XmlFormName &form, Form::FormItem *item = 0);

    bool createItemWidget(Form::FormItem *item, QWidget *parent = 0);
    bool createFormWidget(Form::FormMain *form);
    bool createWidgets(const Form::FormMain *rootForm);
    bool setTabOrder(Form::FormMain *rootForm, const QDomElement &root);

private:
    static XmlFormContentReader *m_Instance;
    QHash<QString, Form::IFormWidgetFactory *> m_PlugsFactories;
    mutable QStringList m_Error;
    bool m_Mute, m_Initialized;
    Form::FormMain *m_ActualForm;

    // Caching some data for speed improvements
    mutable QCache<QString, QDomDocument> m_DomDocFormCache;

    // XML helpers
    QHash<QString, int> m_ScriptsTypes;
    QHash<QString, int> m_ValuesTypes;
    QHash<QString, int> m_SpecsTypes;
    QHash<QString, int> m_PatientData;
};

}  // End namespace Internal
}  // End namespace XmlForms

#endif // XMLFORMCONTENTREADER_H

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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef XMLFORMCONTENTREADER_H
#define XMLFORMCONTENTREADER_H

#include <xmlioplugin/xmlformname.h>

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
 * \date 05 Sept 2011
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

    void clearCache();

    void refreshPluginFactories();

    bool isInCache(const QString &formUid) const;
    QDomDocument *fromCache(const QString &formUid) const;

    void warnXmlReadError(bool muteUserWarnings, const QString &file, const QString &msg, const int line = -1, const int col = -1) const;

    QString lastError() const {return m_Error.join("\n");}

    bool checkFormFileContent(const QString &formUidOrFullAbsPath, const QString &contents) const;

    Form::FormIODescription *readXmlDescription(const QDomElement &xmlDescr, const QString &formUid);
    Form::FormIODescription *readFileInformations(const QString &formUidOrFullAbsPath);

    QList<Form::FormIODescription *> getFormFileDescriptions(const Form::FormIOQuery &query);

    bool loadForm(const XmlFormName &form, Form::FormMain *rootForm);

    bool loadElement(Form::FormItem *item, QDomElement &rootElement, const XmlFormName &form);
    bool createElement(Form::FormItem *item, QDomElement &element, const XmlFormName &form);

    bool populateValues(Form::FormItem *item, const QDomElement &root, const XmlFormName &form);
    bool populateScripts(Form::FormItem *item, const QDomElement &root, const XmlFormName &form);

    bool createItemWidget(Form::FormItem *item, QWidget *parent = 0);
    bool createFormWidget(Form::FormMain *form);
    bool createWidgets(const Form::FormMain *rootForm);

    // PMHx categories
    bool loadPmhCategories(const QString &uuidOrAbsPath);
    bool createCategory(const QDomElement &element, Category::CategoryItem *parent);

    // Some database
    QString saveFormToDatabase(const XmlFormName &form, const int type, const QString &content = QString::null, const QString &modeName = QString::null);
    QString getFileContentFromDatabase(const XmlFormName &form, const int type, const QString &fileNameOrModeName);
    QString readExtraFile(const XmlFormName &form, const QString &fileName);

private:
    static XmlFormContentReader *m_Instance;
    QHash<QString, Form::IFormWidgetFactory *> m_PlugsFactories;
    mutable QStringList m_Error;
    bool m_Mute;
    Form::FormMain *m_ActualForm;

    // Caching some data for speed improvements
    mutable QHash<QString, bool> m_ReadableForms;
    mutable QCache<QString, QDomDocument> m_DomDocFormCache;

    // XML helpers
    QHash<QString, int> m_ScriptsTypes;
    QHash<QString, int> m_ValuesTypes;
    QHash<QString, int> m_SpecsTypes;
    QHash<QString, int> m_PatientDatas;
};

}  // End namespace Internal
}  // End namespace XmlForms

#endif // XMLFORMCONTENTREADER_H

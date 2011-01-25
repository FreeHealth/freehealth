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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef XMLFORMIO_H
#define XMLFORMIO_H

#include <formmanagerplugin/iformio.h>

#include <QDomNode>
#include <QObject>

/**
 * \file xmlformio.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 08 June 2010
*/

namespace Form {
class FormItem;
class FormMain;
class IFormWidgetFactory;
}

namespace XmlForms {

//namespace Internal {
//class XmlIOPrivate;
//} // End Internal

class XmlFormIO : public Form::IFormIO
{
     Q_OBJECT
public:
    XmlFormIO(const QString &absFileName, QObject *parent=0);
    ~XmlFormIO();

    QString name() const {return "XmlFormIO";}

    void muteUserWarnings(bool state) {m_Mute = state;}
    QStringList fileFilters() const;

    void warnXmlReadError(bool muteUserWarnings, const QString &file, const QString &msg, const int line = 0, const int col = 0) const;

    QString managedFileExtension() const { return QString("xml"); }
    bool canReadFile() const;
    bool setFileName(const QString &absFileName);

    bool readFileInformations();
    QString formAuthor() const {return m_Author;}
    QString formVersion() const {return m_Version;}
    QString formDescription(const QString &lang = Trans::Constants::ALL_LANGUAGE) const;
    void formDescriptionToTreeWidget(QTreeWidget *tree, const QString &lang = Trans::Constants::ALL_LANGUAGE) const;


    bool loadForm();
    bool saveForm(QObject *treeRoot) { Q_UNUSED(treeRoot); return true; }

    static QString lastestXmlVersion();

    QString lastError() const {return m_Error.join("\n");}

private:
    bool loadForm(const QString &file, Form::FormMain *rootForm);
    bool loadElement(Form::FormItem *item, QDomElement &rootElement);
    bool createElement(Form::FormItem *item, QDomElement &element);
    bool createItemWidget(Form::FormItem *item, QWidget *parent = 0);
    bool createFormWidget(Form::FormMain *form);
    bool createWidgets();

private:
     mutable QString m_AbsFileName;
     mutable QStringList m_Error;
     mutable QDomDocument m_MainDoc;
     mutable QString m_Author, m_Version;
     mutable QHash<QString, QString> m_Desc;
     bool m_Mute;
     Form::FormMain *m_ActualForm;
};

}  // End XmlForms

#endif  //  XMLFORMIO_H

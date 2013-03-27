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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ITEMPLATES_H
#define ITEMPLATES_H

#include <templatesplugin/templates_exporter.h>
#include <templatesplugin/constants.h>

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QHash>

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

/**
 * \file itemplates.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.2.3
 * \date 10 Jan 2010
*/

namespace Templates {

class TEMPLATES_EXPORT ITemplate
{
public:
    ITemplate() { setId(-1); setParentId(-1); }
    ITemplate(const QHash<int, QVariant> &hashData) : m_Data(hashData) {}
//    ITemplate(const QHash<int, QVariant> &hashData, ITemplate *parent) :
//            m_Id(-1), m_ParentId(parent->id()), m_Data(hashData) {}

    virtual ~ITemplate() {}

    virtual bool isValid() const {return true;}

    virtual QVariant data(int column) const {return m_Data.value(column, QVariant());}
    virtual bool setData(int column, const QVariant &value) {m_Data.insert(column, value); return true;}
    virtual QHash<int, QVariant> data() const {return m_Data;}
    virtual bool replaceData(const QHash<int, QVariant> &newData) {m_Data.clear(); m_Data = newData; return true;}

    virtual void setId(const int id) {m_Data.insert(Constants::Data_Id, id);}
    virtual int id() const {return m_Data.value(Constants::Data_Id).toInt();}
    virtual void setParentId(const int id) {m_Data.insert(Constants::Data_ParentId, id);}
    virtual int parentId() const {return m_Data.value(Constants::Data_ParentId).toInt();}
    virtual QString uuid() const {return data(Constants::Data_Uuid).toString();}
    virtual QString ownerUuid() const {return data(Constants::Data_UserUuid).toString();}
    virtual QString label() const {return data(Constants::Data_Label).toString();}
    virtual QString content() const {return data(Constants::Data_Content).toString();}
    virtual QString summary() const {return data(Constants::Data_Summary).toString();}
    virtual QStringList contentMimeTypes() const {return data(Constants::Data_ContentMimeTypes).toStringList();}

    virtual QMimeData *toMimeData();
    virtual bool fromMimeData(QMimeData *);
    virtual QString serialize();
    virtual bool deserialize(const QString &serialized);

private:
    QHash<int, QVariant> m_Data;
};


}  // end namespace Templates

Q_DECLARE_METATYPE(Templates::ITemplate)

#endif // ITEMPLATES_H

/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef ITEMPLATES_H
#define ITEMPLATES_H

#include <templatesplugin/templates_exporter.h>
#include <templatesplugin/constants.h>

#include <QString>
#include <QVariant>
#include <QHash>

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

/**
 * \file itemplates.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.3
 * \date 10 Jan 2009
*/

namespace Templates {

class TEMPLATES_EXPORT ITemplate
{
public:
    ITemplate() { setId(-1); setParentId(-1); }
    ITemplate(const QHash<int, QVariant> &datas) : m_Datas(datas) {}
//    ITemplate(const QHash<int, QVariant> &datas, ITemplate *parent) :
//            m_Id(-1), m_ParentId(parent->id()), m_Datas(datas) {}

    virtual ~ITemplate() {}

    virtual void setId(const int id) {m_Datas.insert(Constants::Data_Id, id);}
    virtual int id() const {return m_Datas.value(Constants::Data_Id).toInt();}
    virtual void setParentId(const int id) {m_Datas.insert(Constants::Data_ParentId, id);}
    virtual int parentId() const {return m_Datas.value(Constants::Data_ParentId).toInt();}
    virtual bool isValid() const {return true;}

    virtual QVariant data(int column) const {return m_Datas.value(column, QVariant());}
    virtual bool setData(int column, const QVariant &value) {m_Datas.insert(column, value); return true;}
    virtual QHash<int, QVariant> datas() const {return m_Datas;}

    virtual QString uuid() const {return data(Constants::Data_Uuid).toString();}
    virtual QString ownerUuid() const {return data(Constants::Data_UserUuid).toString();}
    virtual QString label() const {return data(Constants::Data_Label).toString();}
    virtual QString content() const {return data(Constants::Data_Content).toString();}
    virtual QString summary() const {return data(Constants::Data_Summary).toString();}

    virtual QMimeData *toMimeData();
    virtual bool fromMimeData(QMimeData *data);
    virtual QString serialize();
    virtual bool deserialize(const QString &serialized);

private:
    QHash<int, QVariant> m_Datas;
};


}  // end namespace Templates

Q_DECLARE_METATYPE(Templates::ITemplate)

#endif // ITEMPLATES_H

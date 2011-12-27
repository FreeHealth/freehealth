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
 ***************************************************************************/
#include "scriptwrappers.h"
#include "scriptpatientwrapper.h"
#include "scriptmanager.h"

#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <utils/global.h>
#include <utils/log.h>

using namespace Script;
//using namespace Internal;

static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::IScriptManager *scriptManager() { return Core::ICore::instance()->scriptManager(); }

static Form::FormItem *getFormItem(const QString &ns, const QString &uuid)
{
    foreach(Form::FormMain *main, formManager()->forms()) {
        if (main->uuid().startsWith(ns)) {
            QList<Form::FormItem *> items = main->flattenFormItemChildren();
            for(int i=0; i < items.count(); ++i) {
                Form::FormItem *item = items.at(i);
                if (item->uuid().compare(uuid, Qt::CaseInsensitive)) {
                    return item;
                }
            }
        }
    }
    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    FORM MANAGER    ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FormManagerScriptWrapper::FormManagerScriptWrapper(QObject *parent) :
    m_LogItemSearch(false)
{
    connect(formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(updateItemWrappers()));
    updateItemWrappers();
}

bool FormManagerScriptWrapper::areLoaded() const
{
    return (formManager()->forms().count()>0);
}

void FormManagerScriptWrapper::usingNamespace(const QString &ns) const
{
    m_NS = ns;
    QStringList separators;
    separators << "::" << ":" << "." << ".." << "__";
    foreach(const QString &sep, separators) {
        if (m_NS.contains(sep) && !m_NS.endsWith(sep)) {
            m_NS += sep;
            break;
        }
    }
}

void FormManagerScriptWrapper::updateItemWrappers()
{
    m_Items.clear();
    foreach(Form::FormItem *main, formManager()->forms()) {
        const QList<Form::FormItem*> items = main->flattenFormItemChildren();
        for(int i=0; i < items.count();++i) {
//            qWarning() << "LLLLLLL" << items.at(i)->uuid() << items.at(i)->objectName();
            FormItemScriptWrapper *w = new FormItemScriptWrapper(this);
            w->setFormItem(items.at(i));
            m_Items << scriptManager()->addScriptObject(w);
        }
    }
}

QScriptValue FormManagerScriptWrapper::item(const QString &uuid)
{
    QString fullUuid = m_NS + uuid;
    for(int i=0;i<m_Items.count();++i) {
        const QScriptValue &val = m_Items.at(i);
        const QString &uuid = val.property("uuid").toString();
        if (uuid.compare(fullUuid, Qt::CaseInsensitive)==0) {
            if (m_LogItemSearch)
                qWarning() << "FOUND ITEM" << fullUuid << uuid;
            return val;
        }
    }
    return QScriptValue(QString("No item found"));
}


/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////    FORMITEMS    /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FormItemScriptWrapper::FormItemScriptWrapper(QObject *parent) :
    m_Item(0)
{
    setObjectName("FormItemScriptWrapper");
}

void FormItemScriptWrapper::setFormItem(Form::FormItem *item)
{
    m_Item = item;
}

QString FormItemScriptWrapper::uuid() const
{
    if (m_Item)
        return m_Item->uuid();
    return QString::null;
}

QString FormItemScriptWrapper::type() const
{
    if (m_Item)
        return m_Item->spec()->pluginName();
    return QString::null;
}

QVariant FormItemScriptWrapper::currentValue() const
{
    if (m_Item) {
        if (m_Item->itemDatas()) {
            return m_Item->itemDatas()->data(0, Form::IFormItemData::CalculationsRole);
        }
    }
    return QVariant(QString());
}

QVariant FormItemScriptWrapper::currentText() const
{
    if (m_Item) {
        if (m_Item->itemDatas()) {
            return m_Item->itemDatas()->data(0, Qt::DisplayRole);
        }
    }
    return QVariant();
}

void FormItemScriptWrapper::setCurrentText(const QVariant &text) const
{
    if (m_Item) {
        if (m_Item->itemDatas()) {
            m_Item->itemDatas()->setData(0, text);
        }
    }
}

bool FormItemScriptWrapper::isChecked() const
{
    if (m_Item) {
        if (m_Item->itemDatas()) {
            return (m_Item->itemDatas()->data(0, Qt::CheckStateRole).toInt()==Qt::Checked);
        }
    }
    return false;
}

void FormItemScriptWrapper::setChecked(const bool check)
{
    if (m_Item) {
        if (m_Item->itemDatas()) {
            if (check)
                m_Item->itemDatas()->setData(0, Qt::Checked, Qt::CheckStateRole);
            else
                m_Item->itemDatas()->setData(0, Qt::Unchecked, Qt::CheckStateRole);
        }
    }
}

QVariant FormItemScriptWrapper::currentUuid() const
{
    if (m_Item) {
        if (m_Item->itemDatas()) {
            QVariant result = m_Item->itemDatas()->data(Form::IFormItemData::ID_CurrentUuid);
            return result;
        }
    }
    return QVariant();
}

void FormItemScriptWrapper::setCurrentUuid(const QVariant &uuid)
{
    if (m_Item) {
        if (m_Item->itemDatas()) {
            if (!m_Item->itemDatas()->setData(Form::IFormItemData::ID_CurrentUuid, uuid))
                LOG_ERROR("Unable to setCurrentUuid, FormItem " + m_Item->uuid());
        }
    }
}

void FormItemScriptWrapper::setEnabled(const bool enable)
{
    if (m_Item) {
        m_Item->formWidget()->setEnabled(enable);
    }
}

bool FormItemScriptWrapper::isEnabled() const
{
    if (m_Item) {
        return m_Item->formWidget()->isEnabled();
    }
    return false;
}


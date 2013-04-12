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
 ***************************************************************************/
#include "scriptwrappers.h"
#include "scriptpatientwrapper.h"
#include "scriptmanager.h"

#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>
#include <coreplugin/translators.h>

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/formcollection.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QLocale>

using namespace Script;
using namespace Internal;

static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Core::IScriptManager *scriptManager() { return Core::ICore::instance()->scriptManager(); }
static inline Core::Translators *translators() { return Core::ICore::instance()->translators(); }

//static Form::FormItem *getFormItem(const QString &ns, const QString &uuid)
//{
//    foreach(Form::FormMain *main, formManager().allEmptyRootForms()) {
//        if (main->uuid().startsWith(ns)) {
//            QList<Form::FormItem *> items = main->flattenFormItemChildren();
//            for(int i=0; i < items.count(); ++i) {
//                Form::FormItem *item = items.at(i);
//                if (item->uuid().compare(uuid, Qt::CaseInsensitive)) {
//                    return item;
//                }
//            }
//        }
//    }
//    return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    FORM MANAGER    ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FormManagerScriptWrapper::FormManagerScriptWrapper(QObject *parent) :
    QObject(parent),
    m_LogItemSearch(false)
{
    connect(translators(), SIGNAL(languageChanged()), this, SIGNAL(languageChanged()));
//    recreateItemWrappers();
}

void FormManagerScriptWrapper::recreateItemWrappers()
{
    qDeleteAll(m_Wrappers);
    m_Wrappers.clear();
    m_Items.clear();
    foreach(Form::FormItem *main, formManager().allEmptyRootForms()) {
        const QList<Form::FormItem*> items = main->flattenFormItemChildren();
        for(int i=0; i < items.count(); ++i) {
            FormItemScriptWrapper *w = new FormItemScriptWrapper(this);
            w->setFormItem(items.at(i));
            m_Items.insert(items.at(i)->uuid(), scriptManager()->addScriptObject(w));
            m_Wrappers << w;
        }
    }
}

void FormManagerScriptWrapper::updateSubFormItemWrappers(const QString &uuid)
{
    Q_UNUSED(uuid);
    // Get subform EmptyRootForms
    const Form::FormCollection &coll = formManager().subFormCollection(uuid);
    const QStringList &uuids = m_Items.keys();

    // Create all ScriptFormItemWrapper
    foreach(Form::FormItem *main, coll.emptyRootForms()) {
        if (main->uuid() != uuid)
            continue;

        foreach(Form::FormItem *item, main->flattenFormItemChildren()) {
            if (uuids.contains(item->uuid())) {
                // Remove item
                m_Items.remove(item->uuid());
            }
            // Create && insert item
            FormItemScriptWrapper *w = new FormItemScriptWrapper(this);
            m_Wrappers << w;
            w->setFormItem(item);
            m_Items.insert(item->uuid(), scriptManager()->addScriptObject(w));
        }
    }
}

QString FormManagerScriptWrapper::currentLanguage() const
{
    return QLocale().name().left(2).toLower();
}

bool FormManagerScriptWrapper::areLoaded() const
{
    return (formManager().allEmptyRootForms().count()>0);
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

QScriptValue FormManagerScriptWrapper::item(const QString &uuid)
{
    QString fullUuid = m_NS + uuid;
    return m_Items.value(fullUuid, QScriptValue(QString("No item found")));
}

QString FormManagerScriptWrapper::extractFormFile(const QString &uuid, const QString &fileName)
{
    return formManager().extractFormFileToTmpPath(uuid, fileName);
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////    FORMITEMS    /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FormItemScriptWrapper::FormItemScriptWrapper(QObject *parent) :
    QObject(parent),
    m_Item(0)
{
    setObjectName("FormItemScriptWrapper");
}

FormItemScriptWrapper::~FormItemScriptWrapper()
{
//    if (m_Item)
//        qWarning() << "~FormItemScriptWrapper" << m_Item->uuid();
//    else
//        qWarning() << "~FormItemScriptWrapper 0x0 ???";
}

void FormItemScriptWrapper::setFormItem(Form::FormItem *item)
{
    m_Item = item;
}

Form::FormItem *FormItemScriptWrapper::item() const
{
    return m_Item;
}

bool FormItemScriptWrapper::isValid() const
{
    return (m_Item!=0);
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
        if (m_Item->itemData()) {
            QVariant toReturn = m_Item->itemData()->data(0, Form::IFormItemData::CalculationsRole);
            if (toReturn.isNull() || !toReturn.isValid())
                return QVariant(QString());
            return toReturn;
        }
    }
    return QVariant(QString());
}

QVariant FormItemScriptWrapper::currentText() const
{
    if (m_Item) {
        if (m_Item->itemData()) {
            return m_Item->itemData()->data(0, Qt::DisplayRole);
        }
    }
    return QVariant();
}

void FormItemScriptWrapper::setCurrentText(const QVariant &text) const
{
    if (m_Item) {
        if (m_Item->itemData()) {
            m_Item->itemData()->setData(0, text);
        }
    }
}

bool FormItemScriptWrapper::isChecked() const
{
    if (m_Item) {
        if (m_Item->itemData()) {
            return (m_Item->itemData()->data(0, Qt::CheckStateRole).toInt()==Qt::Checked);
        }
    }
    return false;
}

void FormItemScriptWrapper::setChecked(const bool check)
{
    if (m_Item) {
        if (m_Item->itemData()) {
            if (check)
                m_Item->itemData()->setData(0, 2, Qt::CheckStateRole);
            else
                m_Item->itemData()->setData(0, 0, Qt::CheckStateRole);
        }
    }
}

QVariant FormItemScriptWrapper::currentUuid() const
{
    if (m_Item) {
        if (m_Item->itemData()) {
            QVariant result = m_Item->itemData()->data(Form::IFormItemData::ID_CurrentUuid);
            return result;
        }
    }
    return QVariant();
}

void FormItemScriptWrapper::setCurrentUuid(const QVariant &uuid)
{
    if (m_Item) {
        if (m_Item->itemData()) {
            if (!m_Item->itemData()->setData(Form::IFormItemData::ID_CurrentUuid, uuid))
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

QWidget *FormItemScriptWrapper::ui() const
{
    if (m_Item) {
        return m_Item->formWidget();
    }
    return 0;
}

QStringList FormItemScriptWrapper::childrenUuid() const
{
    if (!m_Item)
        return QStringList();
    QStringList uuids;
    foreach(Form::FormItem *item, m_Item->flattenFormItemChildren()) {
        uuids.append(item->uuid());
    }
    return uuids;
}

/////////////////////////////////////////////////////////////////////////////////////////


/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
 ***************************************************************************/
#include "iformitem.h"

#include <coreplugin/icore.h>
#include <coreplugin/uniqueidmanager.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitemdata.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/global.h>
#include <translationutils/multilingualclasstemplate.h>
#include <translationutils/constanttranslations.h>

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QGridLayout>
#include <QHeaderView>
#include <QMap>
#include <QDebug>

using namespace Form;
using namespace Form::Internal;

inline static Form::FormManager *formManager() { return Form::FormManager::instance(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  FormItemIdentifiants  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FormItemIdentifiants::setUuid(const QString &uuid)
{
    id = formManager()->uuidManager()->uniqueIdentifier(uuid);
    m_Uuid=uuid;
}

QString FormItemIdentifiants::uuid() const
{
    return m_Uuid;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  FormItemScripts  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ScriptsBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l)
    {
        new QTreeWidgetItem(l, QStringList() << "OnLoad" << m_Scripts.value(0) );
        new QTreeWidgetItem(l, QStringList() << "PostLoad" << m_Scripts.value(1) );
        new QTreeWidgetItem(l, QStringList() << "OnDemand" << m_Scripts.value(2) );
        new QTreeWidgetItem(l, QStringList() << "OnValueChanged" << m_Scripts.value(3) );
        new QTreeWidgetItem(l, QStringList() << "OnValueRequiered" << m_Scripts.value(4) );
        new QTreeWidgetItem(l, QStringList() << "OnDependentValueChanged" << m_Scripts.value(5) );
    }

    QHash<int, QString> m_Scripts;
    QHash<int, int> m_States;
};

namespace Form {
namespace Internal {
class FormItemScriptsPrivate : public Trans::MultiLingualClass<ScriptsBook>
{
public:
    FormItemScriptsPrivate() {}
    ~FormItemScriptsPrivate() {}

    QString categoryForTreeWiget() const {return QString("Scripts");}
};

} // End Internal
} // End Form

FormItemScripts::FormItemScripts(
        const QString &lang,
        const QString &onLoad,
        const QString &postLoad,
        const QString &onDemand,
        const QString &onValChanged,
        const QString &onValRequiered,
        const QString &onDependentValuesChanged ) : d(new FormItemScriptsPrivate)
{
    ScriptsBook *s = d->createLanguage(lang.left(2));
    s->m_Scripts.insert(Script_OnLoad, onLoad);
    s->m_Scripts.insert(Script_PostLoad, postLoad);
    s->m_Scripts.insert(Script_OnDemand, onDemand);
    s->m_Scripts.insert(Script_OnValueChanged, onValChanged);
    s->m_Scripts.insert(Script_OnValueRequiered, onValRequiered);
    s->m_Scripts.insert(Script_OnDependentValueChanged, onDependentValuesChanged);
}

FormItemScripts::~FormItemScripts()
{
    if (d) delete d;
    d=0;
}

void FormItemScripts::setScript(const int type, const QString &script, const QString &lang)
{
    ScriptsBook *s = 0;
    if (d->hasLanguage(lang))
        s = d->getLanguage(lang);
    else
        s = d->createLanguage(lang);
    s->m_Scripts.insert(type,script);
}

QString FormItemScripts::script(const int type, const QString &lang) const
{
    /** \todo use languages */
    ScriptsBook *s = d->getLanguage(lang);
    if (!s)
        return QString::null;
    return s->m_Scripts.value(type);
}

void FormItemScripts::toTreeWidget(QTreeWidgetItem *tree)
{
    d->toTreeWidget(tree);
}

void FormItemScripts::warn() const
{
    ScriptsBook *s = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
    Utils::quickDebugDialog(
            QStringList()
            << QString("Script_OnLoad\n") + s->m_Scripts.value(Script_OnLoad)
            << QString("Script_PostLoad\n") + s->m_Scripts.value(Script_PostLoad)
            << QString("Script_OnDemand\n") + s->m_Scripts.value(Script_OnDemand)
            << QString("Script_OnValueChanged\n") + s->m_Scripts.value(Script_OnValueChanged)
            << QString("Script_OnValueRequiered\n") + s->m_Scripts.value(Script_OnValueRequiered)
            << QString("Script_OnDependentValueChanged\n") + s->m_Scripts.value(Script_OnDependentValueChanged)
            );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  FormItemValueReferences  ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Form {
namespace Internal {

class ValuesBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l)
    {
        QTreeWidgetItem *i = 0;
        i = new QTreeWidgetItem(l, QStringList() << "Possibles");
        foreach(int id, m_Possible.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Possible.value(id).toString() );
        }

        i = new QTreeWidgetItem(l, QStringList() << "Numerical");
        foreach(int id, m_Numerical.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Numerical.value(id).toString() );
        }

        i = new QTreeWidgetItem(l, QStringList() << "Script");
        foreach(int id, m_Script.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Script.value(id).toString() );
        }
    }

public:
    QMap<int, QVariant> m_Possible;
    QMap<int, QVariant> m_Script;
    QMap<int, QVariant> m_Numerical;
    QMap<int, QVariant> m_Uuid;
    QVariant m_Default;
};

class FormItemValuesPrivate : public Trans::MultiLingualClass<ValuesBook>
{
public:
    FormItemValuesPrivate() {}
    ~FormItemValuesPrivate() {}

    QString categoryForTreeWiget() const {return QString("Value References");}
public:
    bool m_Optionnal;
};
}  // End Internal
}  // End Form

FormItemValues::FormItemValues(QObject *parent) :
        QObject(parent), d(new Form::Internal::FormItemValuesPrivate)
{
}

FormItemValues::~FormItemValues()
{
    if (d) delete d;
    d=0;
}

/**
  \brief Defines a value \e val for one \e type at one specified \e id in a virtual list, for one \e language.
  \e language must be a valid local language name in 2 chars encoding.
*/
void FormItemValues::setValue(int type, const int id, const QVariant &val, const QString &language)
{
    ValuesBook *values = d->createLanguage(language);
    switch (type)
    {
    case Value_Uuid :      values->m_Uuid.insert(id,val); break;
    case Value_Numerical : values->m_Numerical.insert(id,val); break;
    case Value_Script :    values->m_Script.insert(id,val); break;
    case Value_Possible :  values->m_Possible.insert(id,val); break;
    case Value_Default :   values->m_Default = val; break;
    case Value_Dependency : break;
    }
}

void FormItemValues::setDefaultValue(const QVariant &val, const QString &lang)
{
    ValuesBook *values = d->createLanguage(lang);
    values->m_Default = val;
}

QVariant FormItemValues::defaultValue(const QString &lang)
{
    ValuesBook *values = d->getLanguage(lang);
    if (values)
        return values->m_Default;
    return 0;
}

//void FormItemValues::setSelectedValue(const QVariant &val)
//{
//    d->m_SelectedValue = val;
//}
//
//QVariant FormItemValues::selectedValue() const
//{
//    return d->m_SelectedValue;
//}

bool FormItemValues::isOptionnal() const
{
    return d->m_Optionnal;
}

void FormItemValues::setOptionnal(bool state)
{
    d->m_Optionnal = state;
}

/**
  \brief Returns a QStringList of values asked sorted by id number. \e typeOfValue represents the enumerator.
*/
QStringList FormItemValues::values(const int typeOfValues) const
{
    ValuesBook *values = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
    if (!values)
        return QStringList();
    QMap<int, QVariant> map;
    switch (typeOfValues)
    {
    case Value_Uuid :      map = values->m_Uuid; break;
    case Value_Numerical : map = values->m_Numerical; break;
    case Value_Script :    map = values->m_Script; break;
    case Value_Possible :  map = values->m_Possible; break;
    case Value_Default :   return QStringList() << values->m_Default.toString(); break;
    case Value_Dependency : return QStringList();
    }
    QStringList list;
    foreach(int i, map.keys()) {
        list.append(map.value(i).toString());
    }
    return list;
}


void FormItemValues::toTreeWidget(QTreeWidgetItem *tree) const
{
    d->toTreeWidget(tree);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  FormItem  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormItem::~FormItem()
{
    if (m_Spec) {
        delete m_Spec;
        m_Spec = 0;
    }
    if (m_Scripts) {
        delete m_Scripts;
        m_Scripts = 0;
    }
    if (m_ItemDatas) {
        delete m_ItemDatas;
        m_ItemDatas = 0;
    }
}

void FormItem::addExtraData(const QString &id, const QString &data)
{
    if (m_ExtraDatas.keys().indexOf(id,Qt::CaseInsensitive)!=-1) {
        QString add = m_ExtraDatas.value(id) + ";" + data;
        m_ExtraDatas.insert(id, add);
    } else {
        m_ExtraDatas.insert(id,data);
    }
}


FormItem *FormItem::createChildItem(const QString &uuid)
{
    Form::FormItem *i = new Form::FormItem(this);
    if (!uuid.isEmpty())
        i->setUuid(uuid);
    return i;
}

void FormItem::languageChanged()
{
    qWarning() << "FormItem language changed" << uuid();
}

void FormPage::languageChanged()
{
    qWarning() << "FormPage language changed" << uuid();
}


//FormPage *FormPage::createPage(const QString &uuid)
//{
//    FormPage *p = new FormPage(this);
//    if (!uuid.isEmpty())
//        p->setUuid(uuid);
//    return p;
//}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  FormMain  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormPage *FormMain::createPage(const QString &uuid)
{
    FormPage *p = new FormPage(this);
    if (!uuid.isEmpty())
        p->setUuid(uuid);
    return p;
}

FormMain::~FormMain()
{
    /** \todo this is buggy */
//    if (m_DebugPage)
//        ExtensionSystem::PluginManager::instance()->removeObject(m_DebugPage);
}

void FormMain::clear()
{
    foreach(FormItem *it, this->formItemChildren()) {
        if (it->itemDatas())
            it->itemDatas()->clear();
    }
}

inline static void itemToTree(FormItem *item, QTreeWidgetItem *tree)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(tree, QStringList() << item->spec()->pluginName() << item->spec()->label());
    QFont bold;
    bold.setBold(true);
    i->setFont(0,bold);
    item->valueReferences()->toTreeWidget(i);
    item->scripts()->toTreeWidget(i);
    foreach(FormItem *c, item->formItemChildren()) {
        itemToTree(c, i);
    }
}

void FormMain::toTreeWidget(QTreeWidget *tree)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(tree, QStringList() << tr("Form : ") + spec()->label());
    QFont bold;
    bold.setBold(true);
    i->setFont(0,bold);
    spec()->toTreeWidget(i);
    valueReferences()->toTreeWidget(i);
    scripts()->toTreeWidget(i);
    foreach(FormItem *c, formItemChildren()) {
        itemToTree(c, i);
    }
}

void FormMain::createDebugPage()
{
    m_DebugPage = new FormMainDebugPage(this, this);
    ExtensionSystem::PluginManager::instance()->addObject(m_DebugPage);
}


void FormMain::languageChanged()
{
    qWarning() << "FormMain language changed" << uuid();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  FormItemSpec  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpecsBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *i)
    {
        new QTreeWidgetItem(i, QStringList() << "Authors" << m_Specs.value(Form::FormItemSpec::Spec_Author).toString() );
        new QTreeWidgetItem(i, QStringList() << "License" << m_Specs.value(Form::FormItemSpec::Spec_License).toString() );
        new QTreeWidgetItem(i, QStringList() << "version" << m_Specs.value(Form::FormItemSpec::Spec_Version).toString() );
        new QTreeWidgetItem(i, QStringList() << "biblio" << m_Specs.value(Form::FormItemSpec::Spec_Bibliography).toString() );
        new QTreeWidgetItem(i, QStringList() << "Description" << m_Specs.value(Form::FormItemSpec::Spec_Description).toString() );
        new QTreeWidgetItem(i, QStringList() << "category" << m_Specs.value(Form::FormItemSpec::Spec_Category).toString() );
        new QTreeWidgetItem(i, QStringList() << "creationDate" << m_Specs.value(Form::FormItemSpec::Spec_CreationDate).toString() );
        new QTreeWidgetItem(i, QStringList() << "LastModification" << m_Specs.value(Form::FormItemSpec::Spec_LastModified).toString() );
        new QTreeWidgetItem(i, QStringList() << "Plugin Name" << m_Specs.value(Form::FormItemSpec::Spec_Plugin).toString() );
        new QTreeWidgetItem(i, QStringList() << "IconFileName" << m_Specs.value(Form::FormItemSpec::Spec_IconFileName).toString() );
    }

    QHash<int, QVariant> m_Specs;
};

namespace Form {
namespace Internal {
class FormItemSpecPrivate : public Trans::MultiLingualClass<SpecsBook>
{
public:
    FormItemSpecPrivate() {}
    ~FormItemSpecPrivate() {}

    QString categoryForTreeWiget() const {return QString("Specs");}
};

} // End Internal
} // End Form

FormItemSpec::FormItemSpec() :
        d(new Form::Internal::FormItemSpecPrivate)
{
}

FormItemSpec::~FormItemSpec()
{
    if (d) delete d;
    d=0;
}

/**
  \brief Defines a value \e val for one \e type at one specified \e id in a virtual list, for one \e language.
  \e language must be a valid local language name in 2 chars encoding.
*/
void FormItemSpec::setValue(int type, const QVariant &val, const QString &language)
{
//    qWarning() << "SPEC" << type << val << language;
    SpecsBook *values = d->createLanguage(language);
    values->m_Specs.insert(type,val);
}

QVariant FormItemSpec::value(const int type, const QString &lang) const
{
    SpecsBook *values = d->getLanguage(lang);
    if (!values)
        return QString();
    QVariant val = values->m_Specs.value(type);
    if (val.isNull() && (lang.compare(Trans::Constants::ALL_LANGUAGE)<0)) {
        val = value(type, Trans::Constants::ALL_LANGUAGE);
    }
    return values->m_Specs.value(type);
}

void FormItemSpec::toTreeWidget(QTreeWidgetItem *tree) const
{
    d->toTreeWidget(tree);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  FormMainDebugPage  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormMainDebugPage::FormMainDebugPage(FormMain *form, QObject *parent) :
        IDebugPage(parent), m_Form(form)
{
    setObjectName("FormMainDebugPage_" + m_Form->uuid());
    m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    tree->setColumnCount(2);
    layout->addWidget(tree);
}

FormMainDebugPage::~FormMainDebugPage()
{
    if (m_Widget)
        delete m_Widget;
}

QString FormMainDebugPage::name() const
{ return objectName(); }

QString FormMainDebugPage::category() const
{ return tr("Forms : ") + m_Form->spec()->label(); }

void FormMainDebugPage::refreshContents()
{
    tree->clear();
    m_Form->toTreeWidget(tree);
    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}

QWidget *FormMainDebugPage::widget()
{
    return m_Widget;
}

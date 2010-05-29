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
#include "formmanager.h"
#include "iformitem.h"
#include "iformio.h"
#include "formplaceholder.h"

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/modemanager/modemanager.h>

#include <utils/global.h>
#include <utils/log.h>

#include <extensionsystem/pluginmanager.h>

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QStackedLayout>
#include <QSplitter>
#include <QWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPointer>
#include <QMap>
#include <QApplication>


using namespace Form;
using namespace Form::Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


namespace Form {
namespace Constants {
    const char * const XML_FORM_GENERAL_TAG = "FormXmlContent";
}

namespace Internal {
class FormManagerPrivate
{
public:
    FormManagerPrivate(FormManager *parent) :
            m_Holder(0), m_ActualEpisode(-1), q(parent)
    {}

    ~FormManagerPrivate()
    {
        // DO NOT DELETE m_UuidManager
        if (m_Holder) {
            delete m_Holder;
            m_Holder = 0;
        }
        /** \todo Delete FormItem ?? */
    }

public:
    QPointer<FormPlaceHolder> m_Holder;
    Core::UniqueIDManager *m_UuidManager;
    QMap<int, Form::FormMain *> m_MappedForms;

    /** \todo create a EpisodeData class */
    int m_ActualEpisode;
    QString m_ActualEpisode_FormUid;


private:
    FormManager *q;
};
} // End Internal
} // End Form


FormManager *FormManager::m_Instance = 0;

FormManager *FormManager::instance()
{
    if (!m_Instance) {
        m_Instance = new FormManager(qApp);
    }
    return m_Instance;
}


FormManager::FormManager(QObject *parent)
        : QObject(parent), d(new Form::Internal::FormManagerPrivate(this))
{
    setObjectName("Form::FormManager");
    /** \todo Need to modify UID code to create a new private uid */
    d->m_UuidManager = Core::ICore::instance()->uniqueIDManager();
    d->m_Holder = new FormPlaceHolder;
}

FormManager::~FormManager()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void FormManager::activateMode()
{
    modeManager()->activateMode(Core::Constants::MODE_PATIENT_FILE);
    d->m_Holder->formTree()->expandAll();
}

Core::UniqueIDManager *FormManager::uuidManager() const
{
    return d->m_UuidManager;
}

FormMain *FormManager::createForm(const QString &uuid, FormMain *parent)
{
    qWarning() << "FormManager Creating Form" << uuid;
    FormMain *f = 0;
    if (parent)
        f = new FormMain(parent);
    else
        f = new FormMain(this);
    if (!uuid.isEmpty())
        f->setUuid(uuid);
    d->m_MappedForms.insert(d->m_UuidManager->uniqueIdentifier(f->uuid()), f);
    return f;
}

QList<FormMain*> FormManager::forms() const
{
    return d->m_MappedForms.values();
}

bool FormManager::hasForm(const QString &uuid) const
{
    if (!d->m_UuidManager->hasUniqueIdentifier(uuid))
        return false;
    return true;
}

FormMain *FormManager::form(const QString &uuid) const
{
    if (!hasForm(uuid))
        return 0;
    int id = d->m_UuidManager->uniqueIdentifier(uuid);
    return d->m_MappedForms.value(id, 0);
}

bool FormManager::loadFile(const QString &filename, const QList<Form::IFormIO *> &iolist)
{
    if (filename.isEmpty())
        return false;

    // get all form readers (IFormIO)
    Form::IFormIO *reader = 0;
    QList<Form::IFormIO *> list;
    if (iolist.isEmpty())
         list = pluginManager()->getObjects<Form::IFormIO>();
    else
        list = iolist;

    // try to read form
    foreach(Form::IFormIO *io, list) {
        if (io->setFileName(filename) && io->canReadFile()) {
            if (io->loadForm())
                reader = io;
        }
    }
    if (!reader)
        return false;

    // populate FormPlaceHolder with new values
    d->m_Holder->reset();

    return true;
}

FormPlaceHolder *FormManager::formPlaceHolder() const
{
    return d->m_Holder;
}

bool FormManager::translateForms()
{
    // Translate the tree
//    translateTreeItem( m_Tree->topLevelItem( 0 ) );
//    m_Tree->resizeColumnToContents( LabelColumn );
//    retranslateUi();
    return true;
}

bool FormManager::activateEpisode(const int id, const QString &formUid, const QString &xmlcontent)
{
    // save actual episode if needed
    if (d->m_ActualEpisode!=-1) {
        if (!saveEpisode(d->m_ActualEpisode, d->m_ActualEpisode_FormUid))
            Utils::Log::addError(this, "Unable to save actual episode before editing a new one");
    }

    // stores the actual episode id
    d->m_ActualEpisode = id;
    d->m_ActualEpisode_FormUid = formUid;

    // clear actual form
    FormMain *form = this->form(formUid);
    if (!form)
        return false;
    form->clear();

    qWarning() << "FormManager::activateEpisode" << id << formUid << xmlcontent;

    if (xmlcontent.isEmpty())
        return true;

    // read the xml'd content
    QHash<QString, QString> datas;
    if (!Utils::readXml(xmlcontent, Form::Constants::XML_FORM_GENERAL_TAG, datas, false)) {
        Utils::Log::addError(this, QString("Error while reading EpisodeContent %2:%1").arg(__LINE__).arg(__FILE__));
        return false;
    }

    // put datas into the FormItems of the form
    // XML content ==
    // <formitemuid>value</formitemuid>
    QHash<QString, FormItem *> items;
    foreach(FormItem *it, form->formItemChildren()) {
        /** \todo check nested items */
        items.insert(it->uuid(), it);
    }

//    qWarning() << items;
//    qWarning() << datas;

    foreach(const QString &s, datas.keys()) {
        FormItem *it = items.value(s, 0);
        if (!it) {
            qWarning() << "FormManager::activateForm :: ERROR : no item :" << s;
            continue;
        }
        if (it->itemDatas())
            it->itemDatas()->setStorableData(datas.value(s));
        else
            qWarning() << "FormManager::activateForm :: ERROR : no itemData :" << s;
    }
    return true;
}

bool FormManager::saveEpisode(const int id, const QString &formUid)
{
    // check each item --> isModified ?
    FormMain *form = this->form(formUid);
    if (!form)
        return false;

    bool formIsModified = false;
    QHash<QString, FormItem *> items;
    foreach(FormItem *it, form->formItemChildren()) {
        /** \todo check nested items */
        if (it->itemDatas()) {
            if (it->itemDatas()->isModified()) {
                qWarning() << it->uuid() << "is modified";
                formIsModified = true;
            }
            items.insert(it->uuid(), it);
        }
    }

    // no changes == nothing to do
    if (!formIsModified)
        return true;

    // ask user what to do
    if (!settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, false).toBool()) {
        bool yes = Utils::yesNoMessageBox(tr("Save episode ?"),
                                          tr("The actual episode has been modified. Do you want to save changes in your database ?\n"
                                             "Answering 'No' will cause definitve data lose."),
                                          "", tr("Save episode"));
        if (!yes)
            return true;
    }

    // create the XML episode file
    QHash<QString, QString> datas;
    foreach(FormItem *it, items) {
        datas.insert(it->uuid(), it->itemDatas()->storableData().toString());
    }

    QString xml = Utils::createXml(Form::Constants::XML_FORM_GENERAL_TAG, datas, 2, false);

    // save the XML episode file in database

    return true;
}

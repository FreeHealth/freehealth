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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "episodemodel.h"
#include "episodebase.h"
#include "constants_db.h"
#include "constants_settings.h"
#include "episodedata.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/icorelistener.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>
#include <formmanagerplugin/iformitemdata.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QSqlTableModel>
//#include <QFutureWatcher>
//#include <QFuture>
//#include <QtConcurrentRun>

enum { base64MimeDatas = true  };

#ifdef DEBUG
enum {
    WarnDragAndDrop = false,
    WarnReparentItem = false,
    WarnDatabaseSaving = false,
    WarnFormAndEpisodeRetreiving = false,
    WarnLogChronos = false
   };
#else
enum {
    WarnDragAndDrop = false,
    WarnReparentItem = false,
    WarnDatabaseSaving = false,
    WarnFormAndEpisodeRetreiving = false,
    WarnLogChronos = false
   };
#endif


//TODO: When currentpatient change --> read all last episodes of forms and feed the patient model of the PatientDataRepresentation

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
//static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }

namespace {
    // TODO: create an Utils::GenericTreeItem \sa Templates::TemplateModel, PMH::PmhCategoryModel
    class EpisodeModelTreeItem
    {
    public:
        EpisodeModelTreeItem(EpisodeModelTreeItem *parent = 0) :
                m_Parent(parent),
                m_IsEpisode(false),
                m_IsModified(false)
//                m_Datas(datas)
        {
//            setData(EpisodeModel::UserUuid, user()->uuid());
//            setIsEpisode(datas.value(EpisodeModel::IsEpisode).toBool());
        }
        ~EpisodeModelTreeItem() { qDeleteAll(m_Children); }

        // Genealogy management
        EpisodeModelTreeItem *child(int number) { return m_Children.value(number); }
        int childCount() const { return m_Children.count(); }
        int columnCount() const { return EpisodeModel::MaxData; }
        EpisodeModelTreeItem *parent() { return m_Parent; }
        void setParent(EpisodeModelTreeItem *parent) { m_Parent = parent; }
        bool appendChild(EpisodeModelTreeItem *child)
        {
            if (!m_Children.contains(child))
                m_Children.append(child);
            return true;
        }
        bool insertChild(const int row, EpisodeModelTreeItem *child)
        {
            if (row > m_Children.count())
                return false;
            m_Children.insert(row, child);
            return true;
        }
        int childNumber() const
        {
            if (m_Parent)
                return m_Parent->m_Children.indexOf(const_cast<EpisodeModelTreeItem*>(this));
            return 0;
        }
        void sortChildren()
        {
            qSort(m_Children.begin(), m_Children.end(), EpisodeModelTreeItem::lessThan);
        }

        // For category only tree
        int childCategoryCount() const
        {
            int n = 0;
            foreach(EpisodeModelTreeItem *c, this->m_Children) {
                if (!c->isEpisode())
                    ++n;
            }
            return n;
        }

        EpisodeModelTreeItem *categoryChild(int number)
        {
            QList<EpisodeModelTreeItem *> cat;
            foreach(EpisodeModelTreeItem *c, this->m_Children) {
                if (!c->isEpisode())
                    cat << c;
            }
            return cat.value(number);
        }

        int categoryChildNumber() const
        {
            if (m_Parent) {
                QList<EpisodeModelTreeItem *> cat;
                foreach(EpisodeModelTreeItem *c, m_Parent->m_Children) {
                    if (!c->isEpisode())
                        cat << c;
                }
                return cat.indexOf(const_cast<EpisodeModelTreeItem*>(this));
            }
            return 0;
        }

        // For tree management
        void setIsEpisode(bool isEpisode) {m_IsEpisode = isEpisode; }
        bool isEpisode() const {return m_IsEpisode;}

        // For database management
        void setModified(bool state)
        {
            m_IsModified = state;
            if (!state)
                m_DirtyRows.clear();
        }
        bool isModified() const {return m_IsModified;}
//        void setNewlyCreated(bool state) {setData(EpisodeModel::IsNewlyCreated, state); }
//        bool isNewlyCreated() const {return data(EpisodeModel::IsNewlyCreated).toBool();}

        bool removeChild(EpisodeModelTreeItem *child)
        {
            if (m_Children.contains(child)) {
                m_Children.removeAll(child);
                return true;
            }
            return false;
        }

        bool removeEpisodes()
        {
            foreach(EpisodeModelTreeItem *item, m_Children) {
                if (item->isEpisode()) {
                    m_Children.removeAll(item);
                    delete item;
                    item = 0;
                }
            }
            return true;
        }

        // For data management
        QVariant data(const int column) const
        {
            Q_UNUSED(column);
//            if (column==EpisodeModel::Label) {
//                if (!m_IsEpisode) {
//                    int nb = 0;
//                    for(int i = 0; i < m_Children.count(); ++i) {
//                        if (m_Children.at(i)->isEpisode())
//                            ++nb;
//                    }
//                    if (nb)
//                        return QString("%1 (%2)").arg(m_Datas.value(column).toString()).arg(nb);
//                }
//            }
//            return m_Datas.value(column, QVariant());
            return QVariant();
        }

        bool setData(int column, const QVariant &value)
        {
            Q_UNUSED(column);
            Q_UNUSED(value);
    //        qWarning()<< data(column) << value << (data(column)==value);
//            if (data(column)==value)
//                return true;
//            m_Datas.insert(column, value);
//            if (column==EpisodeModel::IsEpisode) {
//                m_IsEpisode=value.toBool();
//            }
//            m_IsModified = true;
//            if (!m_DirtyRows.contains(column))
//                m_DirtyRows.append(column);
            return true;
        }

        QVector<int> dirtyRows() const
        {
            return m_DirtyRows;
        }

        // For sort functions
        static bool lessThan(EpisodeModelTreeItem *item1, EpisodeModelTreeItem *item2)
        {
            // category goes first
            // then sort by name
            bool sameType = (((item1->isEpisode()) && (item2->isEpisode())) || ((!item1->isEpisode()) && (!item2->isEpisode())));
            if (sameType)
                return item1->data(EpisodeModel::Label).toString() < item2->data(EpisodeModel::Label).toString();
            return item2->isEpisode();
        }

    private:
        EpisodeModelTreeItem *m_Parent;
        QList<EpisodeModelTreeItem*> m_Children;
        QVector<int> m_DirtyRows;
        bool m_IsEpisode, m_IsModified;
        QHash<int, QVariant> m_Datas;
    };
}

namespace Form {
namespace Internal {

    EpisodeModelCoreListener::EpisodeModelCoreListener(Form::EpisodeModel *parent) :
            Core::ICoreListener(parent)
    {
        Q_ASSERT(parent);
        m_EpisodeModel = parent;
    }
    EpisodeModelCoreListener::~EpisodeModelCoreListener() {}

    bool EpisodeModelCoreListener::coreAboutToClose()
    {
//        qWarning() << Q_FUNC_INFO;
        m_EpisodeModel->submit();
        return true;
    }

    EpisodeModelPatientListener::EpisodeModelPatientListener(Form::EpisodeModel *parent) :
            Core::IPatientListener(parent)
    {
        Q_ASSERT(parent);
        m_EpisodeModel = parent;
    }
    EpisodeModelPatientListener::~EpisodeModelPatientListener() {}

    bool EpisodeModelPatientListener::currentPatientAboutToChange()
    {
        qWarning() << Q_FUNC_INFO;
        m_EpisodeModel->submit();
        return true;
    }


//    EpisodeModelUserListener::EpisodeModelUserListener(Form::EpisodeModel *parent) :
//            UserPlugin::IUserListener(parent)
//    {
//        Q_ASSERT(parent);
//        m_EpisodeModel = parent;
//    }
//    EpisodeModelUserListener::~EpisodeModelUserListener() {}

//    bool EpisodeModelUserListener::userAboutToChange()
//    {
//        qWarning() << Q_FUNC_INFO;
//        m_EpisodeModel->submit();
//        return true;
//    }
//    bool EpisodeModelUserListener::currentUserAboutToDisconnect() {return true;}

class EpisodeModelPrivate
{
public:
    EpisodeModelPrivate(EpisodeModel *parent) :
        m_RootItem(0),
        m_ShowLastEpisodes(0),
        m_FormTreeCreated(false),
        m_ReadOnly(false),
        m_ActualEpisode(0),
        m_CoreListener(0),
        m_PatientListener(0),
        m_AddLastEpisodeIndex(true),
        m_RecomputeLastEpisodeSynthesis(true),
        q(parent)
    {
    }

    ~EpisodeModelPrivate ()
    {
        qDeleteAll(m_Episodes);
        m_Episodes.clear();
    }

    bool isEpisode(EpisodeModelTreeItem *item) { return (m_EpisodeItems.key(item, 0)!=0); }
    bool isForm(EpisodeModelTreeItem *item) { return (m_FormItems.key(item, 0)!=0); }

    void createFormTree()
    {
        if (m_FormTreeCreated)
            return;

        if (m_RootItem) {
            delete m_RootItem;
            m_RootItem = 0;
            m_ShowLastEpisodes = 0;
        }

        // create root item
        m_RootItem = new EpisodeModelTreeItem(0);
        m_FormUids.clear();

        // getting Forms
        if (WarnFormAndEpisodeRetreiving)
            LOG_FOR(q, "Getting Forms");

        // add the form synthesis item
        if (m_AddLastEpisodeIndex) {
            m_ShowLastEpisodes = new EpisodeModelTreeItem(m_RootItem);
            m_RootItem->appendChild(m_ShowLastEpisodes);
        }

        // create one item per form
        foreach(Form::FormMain *form, m_RootForm->flattenFormMainChildren()) {
            EpisodeModelTreeItem *item = new EpisodeModelTreeItem(0);
            m_FormItems.insert(form, item);
            m_FormUids << form->uuid();
        }
        // reparent items
        foreach(Form::FormMain *f, m_RootForm->flattenFormMainChildren()) {
            EpisodeModelTreeItem *it = m_FormItems.value(f);
            if (f->formParent() != m_RootForm) {
                it->setParent(m_FormItems.value(f->formParent()));
                it->parent()->appendChild(it);
            } else {
                it->setParent(m_RootItem);
                m_RootItem->appendChild(it);
            }
            it->setModified(false);
        }
        m_FormTreeCreated = true;
    }

    /** Removes all episodes in EpisodeModelTreeItems */
    void deleteEpisodes(EpisodeModelTreeItem *item)
    {
        if (!item)
            return;
        if (isEpisode(item)) {
            item->parent()->removeChild(item);
            foreach(Form::Internal::EpisodeData  *dataChild, m_EpisodeItems.keys(item))
                m_EpisodeItems.remove(dataChild);
            delete item;
            return;
        }
        for(int i = 0; i < item->childCount(); ++i) {
            EpisodeModelTreeItem *child = item->child(i);
            foreach(Form::Internal::EpisodeData  *dataChild, m_EpisodeItems.keys(child))
                m_EpisodeItems.remove(dataChild);
        }
        item->removeEpisodes();
        int nb = item->childCount();
        for(int i = 0; i < nb; ++i) {
            deleteEpisodes(item->child(i));
        }
    }

    /** Clear the EpisodeModelTreeItems of episode and repopulate with freshly extracted episodes from database */
    void refreshEpisodes()
    {
        QTime chrono;
        if (WarnLogChronos)
            chrono.start();

        // make sure that all actual episodes are saved into database
        if (!saveEpisode(m_ActualEpisode, m_ActualEpisode_FormUid))
            LOG_ERROR_FOR(q, "Unable to save actual episode");

        // delete old episodes
        deleteEpisodes(m_RootItem);
        m_ActualEpisode = 0;
        m_ActualEpisode_FormUid = "";
        qDeleteAll(m_Episodes);
        m_Episodes.clear();
        m_EpisodeItems.clear();

        // get Episodes
        // TODO: code here : add limit to episode retreiving.
        EpisodeBaseQuery query;
        query.setPatientUid(patient()->uuid());
        query.setValidEpisodes(true);
        query.setDeletedEpisodes(false);
        query.setFormUids(m_FormUids);
        m_Episodes = episodeBase()->getEpisodes(query);
        if (WarnFormAndEpisodeRetreiving)
            LOG_FOR(q, "Getting Episodes (refresh): " + QString::number(m_Episodes.count()));

        // create EpisodeModelTreeItems and parent them
        for(int i = 0; i < m_Episodes.count(); ++i) {
            EpisodeData *episode = m_Episodes.at(i);
            // find episode's form parent
            EpisodeModelTreeItem *formParent = 0;
            foreach(Form::FormMain *form, m_FormItems.keys()) {
                EpisodeModelTreeItem *parent = m_FormItems.value(form);
                if (episode->data(EpisodeData::FormUuid).toString() == form->uuid()) {
                    formParent = parent;
                    break;
                }
            }
            if (!formParent) {
                qWarning() << "no valid formUid" << episode->data(EpisodeData::FormUuid).toString();
                continue;
            }
//                formParent = m_RootItem;
            EpisodeModelTreeItem *item = new EpisodeModelTreeItem(formParent);
            item->setParent(formParent);
            formParent->appendChild(item);

//            qWarning() << episode->data(EpisodeData::FormUuid).toString() << formParent;

            m_EpisodeItems.insert(episode, item);
        }
        if (WarnLogChronos)
            Utils::Log::logTimeElapsed(chrono, q->objectName(), "refreshEpisodes()");
    }

    EpisodeModelTreeItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            EpisodeModelTreeItem *item = static_cast<EpisodeModelTreeItem*>(index.internalPointer());
            if (item)
                return item;
        }
        return m_RootItem;
    }

    // TODO: code here : limit memory usage by getting/saving XmlContent separately from base()->getEpisodes()
    void getEpisodeContent(EpisodeData *episode)
    {
        if (episode->data(EpisodeData::Id).toInt()<0)
            return;
        if (episode->data(EpisodeData::IsXmlContentPopulated).toBool())
            return;
        episodeBase()->getEpisodeContent(episode);
    }

    QString createXmlEpisode(const QString &formUid)
    {
        // TODO: code here : use a QDomDocument
        FormMain *form = m_RootForm->formMainChild(formUid);
        if (!form)
            return QString::null;
//        bool formIsModified = false;

        QHash<QString, FormItem *> items;
        foreach(FormItem *it, form->flattenedFormItemChildren()) {
            // TODO: check nested items?
            if (it->itemData()) {
//                if (it->itemDatas()->isModified()) {
//                    qWarning() << it->uuid() << "is modified";
//                    formIsModified = true;
//                }
                items.insert(it->uuid(), it);
            }
        }

//        if (formIsModified) {
            // create the XML episode file
            QHash<QString, QString> datas;
            foreach(FormItem *it, items) {
                datas.insert(it->uuid(), it->itemData()->storableData().toString());
            }
            return Utils::createXml(Form::Constants::XML_FORM_GENERAL_TAG, datas, 2, false);
//        }

//        return QString();
    }

    bool saveEpisode(EpisodeModelTreeItem *item, const QString &formUid)
    {
        if (!item)
            return true;
        if (formUid.isEmpty()) {
            LOG_ERROR_FOR("EpisodeModel", "No formUid");
            return false;
        }

        EpisodeData *episode = m_EpisodeItems.key(item);
        FormMain *form = 0;
        foreach(FormMain *f, m_FormItems.keys()) {
            if (f->uuid()==formUid) {
                form = f;
                break;
            }
        }

        if (episode && form) {
            episode->setData(EpisodeData::XmlContent, createXmlEpisode(formUid));
            episode->setData(EpisodeData::IsXmlContentPopulated, true);
            episode->setData(EpisodeData::Label, form->itemData()->data(IFormItemData::ID_EpisodeLabel));
            episode->setData(EpisodeData::UserDate, form->itemData()->data(IFormItemData::ID_EpisodeDateTime));
            LOG_FOR("EpisodeModel", "Save episode: " + episode->data(EpisodeData::Label).toString());
            if (!settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, true).toBool()) {
                bool yes = Utils::yesNoMessageBox(QCoreApplication::translate("EpisodeModel", "Save episode?"),
                                                  QCoreApplication::translate("EpisodeModel", "The actual episode has been modified. Do you want to save changes in your database?\n"
                                                     "Answering 'No' will cause definitve data lose."),
                                                  "", QCoreApplication::translate("EpisodeModel", "Save episode"));
                if (!yes) {
                    return false;
                }
            }

            // inform the patient model
            foreach(FormItem *it, form->flattenedFormItemChildren()) {
                if (!it->itemData())
                    continue;
//                qWarning() << "Feeding patientModel data with" << it->patientDataRepresentation() << it->itemDatas()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole);
                patient()->setValue(it->patientDataRepresentation(), it->itemData()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole));
            }

            // save episode to database
            return episodeBase()->saveEpisode(episode);
        }
        return false;
    }

    void feedFormWithEpisodeContent(Form::FormMain *form, EpisodeModelTreeItem *item, bool feedPatientModel = false)
    {
        EpisodeData *episode = m_EpisodeItems.key(item);
        feedFormWithEpisodeContent(form, episode, feedPatientModel);
    }

    void feedFormWithEpisodeContent(Form::FormMain *form, EpisodeData *episode, bool feedPatientModel = false)
    {
        QTime chrono;
        if (WarnLogChronos)
            chrono.start();

        getEpisodeContent(episode);
        const QString &xml = episode->data(EpisodeData::XmlContent).toString();
        if (xml.isEmpty()) {
            return;
        }

        // read the xml'd content
        QHash<QString, QString> datas;
        if (!Utils::readXml(xml, Form::Constants::XML_FORM_GENERAL_TAG, datas, false)) {
            LOG_ERROR_FOR(q, QString("Error while reading EpisodeContent"));
            return;
        }

        // put datas into the FormItems of the form
        // XML content ==
        // <formitemuid>value</formitemuid>
        QHash<QString, FormItem *> items;
        foreach(FormItem *it, form->flattenedFormItemChildren()) {
            items.insert(it->uuid(), it);
        }

//        qWarning() << Q_FUNC_INFO << feedPatientModel << form->uuid();
        form->clear();
        form->itemData()->setData(IFormItemData::ID_EpisodeDateTime, episode->data(EpisodeData::UserDate));
        form->itemData()->setData(IFormItemData::ID_EpisodeLabel, episode->data(EpisodeData::Label));
        const QString &username = user()->fullNameOfUser(episode->data(EpisodeData::UserCreatorUuid)); //value(Core::IUser::FullName).toString();
        if (username.isEmpty())
            form->itemData()->setData(IFormItemData::ID_UserName, "No user");
        else
            form->itemData()->setData(IFormItemData::ID_UserName, username);

        // feed the formitemdatas for this form and get the data for the patientmodel
        foreach(FormItem *it, items.values()) {
            if (!it) {
                qWarning() << "FormManager::activateForm :: ERROR: no item: " << items.key(it);
                continue;
            }
            if (!it->itemData())
                continue;

            it->itemData()->setStorableData(datas.value(it->uuid()));
            if (feedPatientModel) {
//                qWarning() << "Feeding patientModel data with" << it->patientDataRepresentation() << it->itemDatas()->data(it->patientDataRepresentation(), IFormItemData::ID_ForPatientModel);
                if (it->patientDataRepresentation() >= 0) {
                    if (WarnLogChronos)
                        Utils::Log::logTimeElapsed(chrono, q->objectName(), "feedFormWithEpisodeContent: feed patient model: " + it->uuid());
                    patient()->setValue(it->patientDataRepresentation(), it->itemData()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole));
                }
            }
        }
        if (WarnLogChronos)
            Utils::Log::logTimeElapsed(chrono, q->objectName(), "feedFormWithEpisodeContent");

    }

    void getLastEpisodes(bool andFeedPatientModel = true)
    {
        qWarning() << "GetLastEpisode (feedPatientModel=" <<andFeedPatientModel << ")";
        if (patient()->uuid().isEmpty())
            return;

        QTime chrono;
        if (WarnLogChronos)
            chrono.start();

        foreach(Form::FormMain *form, m_FormItems.keys()) {
//            if (andFeedPatientModel) {
//                bool hasPatientDatas = false;
//                // test all children FormItem for patientDataRepresentation
//                foreach(Form::FormItem *item, form->flattenedFormItemChildren()) {
//                    if (item->itemDatas()) {
//                        if (item->patientDataRepresentation()!=-1) {
//                            hasPatientDatas = true;
//                            break;
//                        }
//                    }
//                }
//                if (!hasPatientDatas)
//                    continue;
//            }

            // get the form's XML content for the last episode, feed it with the XML code
            EpisodeModelTreeItem *formItem = m_FormItems.value(form);
            if (!formItem->childCount()) {
                continue;  // No episodes
            }

            // find last episode of the form
            EpisodeData *lastOne = 0;
            for(int i=0; i < m_Episodes.count(); ++i) {
                EpisodeData *episode = m_Episodes.at(i);
                if (episode->data(EpisodeData::FormUuid).toString()==form->uuid()) {
                    if (!lastOne) {
                        lastOne = episode;
                        continue;
                    }
                    if (lastOne->data(EpisodeData::UserDate).toDateTime() < episode->data(EpisodeData::UserDate).toDateTime()) {
                        lastOne = episode;
                    }
                }
            }

            // feed episode and activate it
            if (lastOne) {
                feedFormWithEpisodeContent(form, lastOne, andFeedPatientModel);
            }
        }
        if (WarnLogChronos)
            Utils::Log::logTimeElapsed(chrono, q->objectName(), "getLastEpisodes");
    }

public:
    FormMain *m_RootForm;
    EpisodeModelTreeItem *m_RootItem, *m_ShowLastEpisodes;
    QString m_UserUuid, m_LkIds, m_CurrentPatient, m_CurrentForm;
    bool m_FormTreeCreated, m_ReadOnly;
    QStringList m_FormUids;

    QMap<Form::FormMain *, EpisodeModelTreeItem *> m_FormItems;
    QMap<Form::Internal::EpisodeData *, EpisodeModelTreeItem *> m_EpisodeItems;
    QList<Form::Internal::EpisodeData *> m_Episodes;

    // TODO: code here : remove m_ActualEpisode, m_ActualEpisode_FormUid
    EpisodeModelTreeItem *m_ActualEpisode;
    QString m_ActualEpisode_FormUid;

    EpisodeModelCoreListener *m_CoreListener;
    EpisodeModelPatientListener *m_PatientListener;

    bool m_AddLastEpisodeIndex, m_RecomputeLastEpisodeSynthesis;

//    QFutureWatcher<void> *m_GetLastEpisodeWatcher;

private:
    EpisodeModel *q;
};
}
}

EpisodeModel::EpisodeModel(FormMain *rootEmptyForm, QObject *parent) :
        QAbstractItemModel(parent), d(new Internal::EpisodeModelPrivate(this))
{
    Q_ASSERT(rootEmptyForm);
    setObjectName("EpisodeModel");
    d->m_RootForm = rootEmptyForm;

    // Autosave feature
    //    Core Listener
    d->m_CoreListener = new Internal::EpisodeModelCoreListener(this);
    pluginManager()->addObject(d->m_CoreListener);

    //    User Listener

    //    Patient change listener
    d->m_PatientListener = new Internal::EpisodeModelPatientListener(this);
    pluginManager()->addObject(d->m_PatientListener);

    // concurrent programming
//    d->m_GetLastEpisodeWatcher = new QFutureWatcher<void>;
//    connect(d->m_GetLastEpisodeWatcher, SIGNAL(finished()), this, SLOT(lastEpisodeRetrieved()));

    init();
}

void EpisodeModel::init(bool addLastEpisodeIndex)
{
    d->m_UserUuid = user()->uuid();
    d->m_AddLastEpisodeIndex = addLastEpisodeIndex;
    d->m_CurrentPatient = patient()->uuid();
    d->createFormTree();

    onUserChanged();

    // connect the save action
    Core::Command * cmd = actionManager()->command(Core::Constants::A_FILE_SAVE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(submit()));

    onPatientChanged();

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    connect(user(), SIGNAL(userChanged()), this, SLOT(onUserChanged()));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onPatientChanged()));
}

void EpisodeModel::refreshFormTree()
{
    d->m_FormTreeCreated = false;
    d->createFormTree();
    d->refreshEpisodes();
    d->getLastEpisodes(true);
    reset();
}

EpisodeModel::~EpisodeModel()
{
    if (d->m_CoreListener) {
        pluginManager()->removeObject(d->m_CoreListener);
    }
    if (d->m_PatientListener) {
        pluginManager()->removeObject(d->m_PatientListener);
    }
    if (d) {
        delete d;
        d=0;
    }
}

void EpisodeModel::onCoreDatabaseServerChanged()
{
    d->m_FormTreeCreated = false;
    d->createFormTree();
    d->refreshEpisodes();
    d->getLastEpisodes(true);
    reset();
}

void EpisodeModel::onUserChanged()
{
    d->m_UserUuid = user()->uuid();
    // TODO: code here
//    QList<int> ids = episodeBase()->retrievePractionnerLkIds(uuid);
//    d->m_LkIds.clear();
//    foreach(int i, ids)
//        d->m_LkIds.append(QString::number(i) + ",");
//    d->m_LkIds.chop(1);
//    d->refreshEpisodes();
}

void EpisodeModel::onPatientChanged()
{
    d->m_CurrentPatient = patient()->uuid();
    d->refreshEpisodes();
    // trying multithread
//    if (d->m_GetLastEpisodeWatcher->isRunning()) {
//        d->m_GetLastEpisodeWatcher->cancel();
//    }
    // Start the computation.
//    QFuture<int> future = QtConcurrent::run(getLastEpisodes, true);
//    d->m_GetLastEpisodeWatcher->setFuture(future);

    d->getLastEpisodes(true);
    d->m_RecomputeLastEpisodeSynthesis = false;
    reset();
}

QModelIndex EpisodeModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

     EpisodeModelTreeItem *parentItem = d->getItem(parent);
     EpisodeModelTreeItem *childItem = 0;
     childItem = parentItem->child(row);
     if (childItem) { // && childItem != d->m_RootItem) {
         return createIndex(row, column, childItem);
     }
     return QModelIndex();
 }

QModelIndex EpisodeModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     EpisodeModelTreeItem *childItem = d->getItem(index);
     EpisodeModelTreeItem *parentItem = childItem->parent();

     if (parentItem == d->m_RootItem)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

int EpisodeModel::rowCount(const QModelIndex &parent) const
{
    EpisodeModelTreeItem *item = d->getItem(parent);
    if (item) {
        return item->childCount();
    }
    return 0;
}

int EpisodeModel::columnCount(const QModelIndex &) const
{
    return MaxData;
}

QVariant EpisodeModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    if (item.column() == EmptyColumn1)
        return QVariant();
    if (item.column() == EmptyColumn2)
        return QVariant();

    EpisodeModelTreeItem *it = d->getItem(item);
    if (it==d->m_RootItem) {
        return QVariant();
    }

    if (it==d->m_ShowLastEpisodes) {
        switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            if (item.column() == FormUuid)
                return Constants::PATIENTLASTEPISODES_UUID;
            if (item.column() == Label)
                return QApplication::translate(Constants::FORM_TR_CONTEXT, Constants::SHOWPATIENTLASTEPISODES_TEXT);
            break;
        case Qt::FontRole:
        {
            QFont f;
            f.fromString(settings()->value(Constants::S_EPISODEMODEL_FORM_FONT).toString());
            return f;
        }
        case Qt::DecorationRole:
            return theme()->icon(Core::Constants::ICONPATIENTSYNTHESIS);
        case Qt::ForegroundRole:
            return QColor(settings()->value(Constants::S_EPISODEMODEL_FORM_FOREGROUND).toString());
        }
        return QVariant();
    }

    EpisodeData *episode = d->m_EpisodeItems.key(it, 0);
    FormMain *form = d->m_FormItems.key(it, 0);

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
    {
        switch (item.column()) {
        case Label:
            if (episode) {
                QHash<QString, QString> tokens;
                tokens.insert(Constants::T_LABEL, episode->data(EpisodeData::Label).toString());
                tokens.insert(Constants::T_SMALLDATE, QLocale().toString(episode->data(EpisodeData::UserDate).toDate(), settings()->value(Constants::S_EPISODEMODEL_SHORTDATEFORMAT, tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL)).toString()));
                tokens.insert(Constants::T_FULLDATE, QLocale().toString(episode->data(EpisodeData::UserDate).toDate(), settings()->value(Constants::S_EPISODEMODEL_SHORTDATEFORMAT, tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL)).toString()));
                QString s = settings()->value(Constants::S_EPISODELABELCONTENT).toString();
                Utils::replaceTokens(s, tokens);
                return s;
            }
            if (form)
                return form->spec()->label();
            break;
        case IsValid:
            if (episode)
                return episode->data(EpisodeData::IsValid);
            if (form)
                return true;
            break;
            //           case  Summary,
            //           case  FullContent,
            //           case  Id,
            //           case  UserUuid,
            //           case  PatientUuid,
        case  FormUuid:
            if (episode)
                return episode->data(EpisodeData::FormUuid);
            if (form)
                return form->uuid();
            break;
//        case IsNewlyCreated:
        case IsEpisode:
            return (episode!=0);
        case XmlContent:
            if (episode) {
                if (!episode->data(EpisodeData::IsXmlContentPopulated).toBool())
                    d->getEpisodeContent(episode);
                return episode->data(EpisodeData::XmlContent);
            }
            break;
        }
        break;
    }
    case Qt::ToolTipRole :
    {
        switch (item.column()) {
        case Label:
            if (episode)
                return QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                        .arg(QLocale().toString(episode->data(EpisodeData::UserDate).toDate(), settings()->value(Constants::S_EPISODEMODEL_SHORTDATEFORMAT, tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL)).toString()).replace(" ", "&nbsp;"))
                        .arg(episode->data(EpisodeData::Label).toString().replace(" ", "&nbsp;"))
                        .arg(user()->fullNameOfUser(episode->data(EpisodeData::UserCreatorUuid).toString()) + "<br/>" +
                             tr("Created: ") +  QLocale().toString(episode->data(EpisodeData::CreationDate).toDate(), settings()->value(Constants::S_EPISODEMODEL_LONGDATEFORMAT, tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL)).toString()));
            if (form)
                return form->spec()->label();
            break;
        }
        break;
    }
    case Qt::ForegroundRole :
    {
        if (episode) {
            return QColor(settings()->value(Constants::S_EPISODEMODEL_EPISODE_FOREGROUND, "darkblue").toString());
        } else if (form) {
            if (it->parent() == d->m_RootItem) {
                if (settings()->value(Constants::S_USESPECIFICCOLORFORROOTS).toBool()) {
                    return QColor(settings()->value(Constants::S_FOREGROUNDCOLORFORROOTS, "darkblue").toString());
                }
            }
            return QColor(settings()->value(Constants::S_EPISODEMODEL_FORM_FOREGROUND, "#000").toString());
        }
        break;
    }
    case Qt::FontRole :
    {
        if (form) {
            QFont f;
            f.fromString(settings()->value(Constants::S_EPISODEMODEL_FORM_FONT).toString());
            return f;
        } else {
            QFont f;
            f.fromString(settings()->value(Constants::S_EPISODEMODEL_EPISODE_FONT).toString());
            return f;
        }
        return QFont();
    }
    case Qt::DecorationRole :
    {
        if (form) {
            QString icon = form->spec()->value(FormItemSpec::Spec_IconFileName).toString();
            icon.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::SmallPixmapPath));
            if (QFileInfo(icon).isRelative())
                icon.append(qApp->applicationDirPath());
            return QIcon(icon);
        }
        break;
    }
    }
    return QVariant();
}

bool EpisodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (d->m_ReadOnly)
        return false;

    if (!index.isValid())
        return false;

    EpisodeModelTreeItem *it = d->getItem(index);
    if (it==d->m_RootItem)
        return false;

    EpisodeData *episode = d->m_EpisodeItems.key(it, 0);

    if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        if (episode) {
            switch (index.column()) {
            case Label: episode->setData(EpisodeData::Label, value); break;
            case Date: episode->setData(EpisodeData::UserDate, value); break;
            case IsValid: episode->setData(EpisodeData::IsValid, value); break;
            case FormUuid: episode->setData(EpisodeData::FormUuid, value); break;
            case XmlContent: episode->setData(EpisodeData::XmlContent, value); episode->setData(EpisodeData::IsXmlContentPopulated, value); break;
            }
        }

        Q_EMIT dataChanged(index, index);
    }
    return true;
}

Qt::ItemFlags EpisodeModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/** Not implemented */
QVariant EpisodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

/** Insert new episodes to the \e parent. The parent must be a form. */
bool EpisodeModel::insertRows(int row, int count, const QModelIndex &parent)
{
//    qWarning() << "insertRows" << row << count << parent.data();
    if (d->m_ReadOnly)
        return false;

    if (!parent.isValid())
        return false;

    EpisodeModelTreeItem *parentItem = d->getItem(parent);
    if (!parentItem)
        return false;

    FormMain *form = formForIndex(parent);
    if (!form)
        return false;
    const QString &formUid = form->uuid();

    beginInsertRows(parent, row, row + count);
    for(int i = 0; i < count; ++i) {
        // create the episode
        Internal::EpisodeData *episode = new Internal::EpisodeData;
        episode->setData(Internal::EpisodeData::Label, tr("New episode"));
        episode->setData(Internal::EpisodeData::FormUuid, formUid);
        episode->setData(Internal::EpisodeData::UserCreatorUuid, user()->uuid());
        episode->setData(Internal::EpisodeData::PatientUuid, patient()->uuid());
        episode->setData(Internal::EpisodeData::CreationDate, QDateTime::currentDateTime());
        episode->setData(Internal::EpisodeData::UserDate, QDateTime::currentDateTime());
        episode->setData(Internal::EpisodeData::IsValid, true);
        // TODO: code here : create an episode modification to store the user creator?

        // create the tree item
        EpisodeModelTreeItem *it = new EpisodeModelTreeItem(parentItem);
        parentItem->insertChild(row+i, it);

        // link episode/item
        d->m_EpisodeItems.insert(episode, it);
        d->m_Episodes.append(episode);
    }
    endInsertRows();
    return true;
}

/** Not implemented */
bool EpisodeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    //    qWarning() << "removeRows" << row << count;
    if (d->m_ReadOnly)
        return false;

    return true;
}

/** Return true is the \e index is an episode. */
bool EpisodeModel::isEpisode(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    EpisodeModelTreeItem *it = d->getItem(index);
    if (it==d->m_RootItem)
        return false;

    EpisodeData *episode = d->m_EpisodeItems.key(it, 0);

    return (episode);
}

/** Return true is the \e index only owns one unique episode. It is supposed that the \e index points to a form */
bool EpisodeModel::isUniqueEpisode(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    EpisodeModelTreeItem *it = d->getItem(index);
    if (it==d->m_RootItem)
        return false;

    FormMain *form = d->m_FormItems.key(it, 0);
    if (form)
        return form->episodePossibilities()==FormMain::UniqueEpisode;

    return false;
}

/** Return true is the \e index does not own episodes. It is supposed that the \e index points to a form */
bool EpisodeModel::isNoEpisode(const QModelIndex &index)
{
    if (!index.isValid())
        return false;
    EpisodeModelTreeItem *it = d->getItem(index);
    if (it==d->m_RootItem)
        return false;

    FormMain *form = d->m_FormItems.key(it, 0);
    if (form)
        return form->episodePossibilities()==FormMain::NoEpisode;

    return false;
}

/** Define the whole model read mode */
void EpisodeModel::setReadOnly(const bool state)
{
    d->m_ReadOnly = state;
}

/** Return true if the whole model is in a read only mode */
bool EpisodeModel::isReadOnly() const
{
    return d->m_ReadOnly;
}

/** Return true if the model has unsaved data */
bool EpisodeModel::isDirty() const
{
    return false;
}

bool EpisodeModel::isLastEpisodeIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    EpisodeModelTreeItem *it = d->getItem(index);
    if (it==d->m_ShowLastEpisodes)
        return true;
    return false;
}

/** Return the Form::FormMain pointer corresponding to the \e index. The returned pointer must not be deleted */
Form::FormMain *EpisodeModel::formForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    QModelIndex idx = index;
    while (idx.isValid()) {
        EpisodeModelTreeItem *it = d->getItem(idx);
        if (it==d->m_RootItem)
            return 0;
        FormMain *form = d->m_FormItems.key(it, 0);
        if (form)
            return form;
        idx = idx.parent();
    }
    return 0;
}

static QModelIndex formIndex(const QString &formUid, const QModelIndex &parent, const Form::EpisodeModel *model)
{
    // Test parent
    if (model->isForm(parent)) {
        if (parent.data().toString()==formUid) {
            return model->index(parent.row(), 0, parent.parent());
        }
    }
    // Test its children
    for(int i = 0; i < model->rowCount(parent); ++i) {
        QModelIndex item = model->index(i, EpisodeModel::FormUuid, parent);
        QModelIndex ret = formIndex(formUid, item, model);
        if (ret.isValid())
            return model->index(ret.row(), 0, ret.parent());
    }
    return QModelIndex();
}

/** Return the QModelIndex corresponding to the form with the specified \e formUid, or return an invalid index. */
QModelIndex EpisodeModel::indexForForm(const QString &formUid) const
{
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex ret = formIndex(formUid, index(i, EpisodeModel::FormUuid), this);
        if (ret.isValid()) {
            return ret;
        }
    }
    return QModelIndex();
}

/** Save the whole model. \sa isDirty() */
bool EpisodeModel::submit()
{
    // No active patient ?
    if (patient()->uuid().isEmpty())
        return false;

    // save actual episode if needed
    if (d->m_ActualEpisode) {
        if (!d->saveEpisode(d->m_ActualEpisode, d->m_ActualEpisode_FormUid)) {
            LOG_ERROR("Unable to save actual episode before editing a new one");
        }
    }
    return true;
}

bool EpisodeModel::activateEpisode(const QModelIndex &index, const QString &formUid) //, const QString &xmlcontent)
{
    qWarning() << "activateEpisode" << formUid;
    // submit actual episode
    if (!d->saveEpisode(d->m_ActualEpisode, d->m_ActualEpisode_FormUid)) {
        LOG_ERROR("Unable to save actual episode before editing a new one");
    }

    d->m_RecomputeLastEpisodeSynthesis = true;

    if (!index.isValid()) {
        d->m_ActualEpisode = 0;
        return false;
    }

    // stores the actual episode id
    EpisodeModelTreeItem *it = d->getItem(index);
    if (it==d->m_RootItem)
        return false;

    EpisodeData *episode = d->m_EpisodeItems.key(it, 0);
    FormMain *form = formForIndex(index);
    if (!episode) {
        d->m_ActualEpisode = 0;
        return false;
    }
    d->m_ActualEpisode = it;
    d->m_ActualEpisode_FormUid = formUid;

    // TODO: this form code should not appear in a model's method according to strict MVC pattern.
    //    Extract it to the form parts and maybe link with Signal/Slots?
    // clear actual form and fill episode data
    if (!form)
        return false;
    form->clear();
    form->itemData()->setData(IFormItemData::ID_EpisodeDateTime, episode->data(EpisodeData::UserDate));
    form->itemData()->setData(IFormItemData::ID_EpisodeLabel, episode->data(EpisodeData::Label));
    const QString &username = user()->fullNameOfUser(episode->data(EpisodeData::UserCreatorUuid)); //value(Core::IUser::FullName).toString();
    if (username.isEmpty())
        form->itemData()->setData(IFormItemData::ID_UserName, tr("No user"));
    else
        form->itemData()->setData(IFormItemData::ID_UserName, username);

    // TODO: move this part into a specific member of the private part
    d->getEpisodeContent(episode);
    const QString &xml = episode->data(EpisodeData::XmlContent).toString();
    if (xml.isEmpty())
        return true;

    // read the xml'd content
    QHash<QString, QString> datas;
    if (!Utils::readXml(xml, Form::Constants::XML_FORM_GENERAL_TAG, datas, false)) {
        LOG_ERROR(QString("Error while reading EpisodeContent %2:%1").arg(__LINE__).arg(__FILE__));
        return false;
    }

    // put datas into the FormItems of the form
    // XML content ==
    // <formitemuid>value</formitemuid>
    QHash<QString, FormItem *> items;
    foreach(FormItem *it, form->flattenedFormItemChildren()) {
        items.insert(it->uuid(), it);
        // Add fieldEquivalence
//        foreach(const QString &uuid, it->equivalentUuid()) {
//            items.insert(uuid, it);
//        }
    }

//    qWarning() << "ITEMS" << items;

    foreach(const QString &s, datas.keys()) {
        FormItem *it = items.value(s, 0);
        if (!it) {
            qWarning() << "FormManager::activateForm :: ERROR: no item: " << s;
            continue;
        }
        if (it->itemData())
            it->itemData()->setStorableData(datas.value(s));
        else
            qWarning() << "FormManager::activateForm :: ERROR: no itemData: " << s;
    }
    return true;
}

/** Save the episode pointed by the \e index to the database. */
bool EpisodeModel::saveEpisode(const QModelIndex &index, const QString &formUid)
{
    return d->saveEpisode(d->getItem(index), formUid);
}

/** Return the HTML formatted synthesis of all the last recorded episodes for each forms in the model. */
QString EpisodeModel::lastEpisodesSynthesis() const
{
    QTime chrono;
    if (WarnLogChronos)
        chrono.start();

    if (d->m_RecomputeLastEpisodeSynthesis) {
        // submit actual episode
        if (!d->saveEpisode(d->m_ActualEpisode, d->m_ActualEpisode_FormUid)) {
            LOG_ERROR("Unable to save actual episode before editing a new one");
        }
        d->m_ActualEpisode = 0;
        d->m_ActualEpisode_FormUid.clear();

        d->getLastEpisodes(false);
    }
    if (WarnLogChronos)
        Utils::Log::logTimeElapsed(chrono, objectName(), "Compute last episode Part 1");

    QString html;
    foreach(FormMain *f, d->m_RootForm->firstLevelFormMainChildren()) {
        if (!f) {
            LOG_ERROR("??");
            continue;
        }
        html += f->printableHtml();
    }

    if (WarnLogChronos)
        Utils::Log::logTimeElapsed(chrono, objectName(), "Compute last episode Part 2 (getting html code)");

    return html;
}


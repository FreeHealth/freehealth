/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "episodemodel.h"
#include "episodebase.h"
#include "constants_db.h"
#include "constants_settings.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>
#include <formmanagerplugin/iformitemdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/icorelistener.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>

enum { base64MimeDatas = true  };

#ifdef DEBUG
enum {
    WarnDragAndDrop = false,
    WarnReparentItem = false,
    WarnDatabaseSaving = false
   };
#else
enum {
    WarnDragAndDrop = false,
    WarnReparentItem = false,
    WarnDatabaseSaving = false
   };
#endif


/**
  \todo When currentpatient change --> read all last episodes of forms and feed the patient model of the
  PatientDataRepresentation
*/



using namespace Form;
using namespace Trans::ConstantTranslations;

static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
//static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline QString currentUserUuid() {return user()->value(Core::IUser::Uuid).toString();}

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }


namespace {

    /** \todo create an Utils::GenericTreeItem \sa Templates::TemplateModel, PMH::PmhCategoryModel */
    class TreeItem
    {
    public:
        TreeItem(const QHash<int, QVariant> &datas, TreeItem *parent = 0) :
                m_Parent(parent),
                m_IsEpisode(false),
                m_IsModified(false),
                m_Datas(datas)
        {
            setData(EpisodeModel::UserUuid, currentUserUuid());
            setIsEpisode(datas.value(EpisodeModel::IsEpisode).toBool());
        }
        ~TreeItem() { qDeleteAll(m_Children); }

        // Genealogy management
        TreeItem *child(int number) { return m_Children.value(number); }
        int childCount() const { return m_Children.count(); }
        int columnCount() const { return EpisodeModel::MaxData; }
        TreeItem *parent() { return m_Parent; }
        void setParent(TreeItem *parent) { m_Parent = parent; }
        bool addChildren(TreeItem *child)
        {
            if (!m_Children.contains(child))
                m_Children.append(child);
            return true;
        }
        bool insertChild(const int row, TreeItem *child)
        {
            if (row > m_Children.count())
                return false;
            m_Children.insert(row, child);
            return true;
        }
        int childNumber() const
        {
            if (m_Parent)
                return m_Parent->m_Children.indexOf(const_cast<TreeItem*>(this));
            return 0;
        }
        void sortChildren()
        {
            qSort(m_Children.begin(), m_Children.end(), TreeItem::lessThan);
        }

        // For category only tree
        int childCategoryCount() const
        {
            int n = 0;
            foreach(TreeItem *c, this->m_Children) {
                if (!c->isEpisode())
                    ++n;
            }
            return n;
        }

        TreeItem *categoryChild(int number)
        {
            QList<TreeItem *> cat;
            foreach(TreeItem *c, this->m_Children) {
                if (!c->isEpisode())
                    cat << c;
            }
            return cat.value(number);
        }

        int categoryChildNumber() const
        {
            if (m_Parent) {
                QList<TreeItem *> cat;
                foreach(TreeItem *c, m_Parent->m_Children) {
                    if (!c->isEpisode())
                        cat << c;
                }
                return cat.indexOf(const_cast<TreeItem*>(this));
            }
            return 0;
        }

        // For tree management
        void setIsEpisode(bool isEpisode) {m_IsEpisode = isEpisode; setData(EpisodeModel::IsEpisode, isEpisode); }
        bool isEpisode() const {return m_IsEpisode;}

        // For database management
        void setModified(bool state)
        {
            m_IsModified = state;
            if (!state)
                m_DirtyRows.clear();
        }
        bool isModified() const {return m_IsModified;}
        void setNewlyCreated(bool state) {setData(EpisodeModel::IsNewlyCreated, state); }
        bool isNewlyCreated() const {return data(EpisodeModel::IsNewlyCreated).toBool();}

        bool removeChild(TreeItem *child)
        {
            if (m_Children.contains(child)) {
                m_Children.removeAll(child);
                return true;
            }
            return false;
        }

        bool removeEpisodes()
        {
            foreach(TreeItem *item, m_Children) {
                if (item->isEpisode()) {
                    m_Children.removeAll(item);
                    delete item;
                }
            }
            return true;
        }

        // For data management
        QVariant data(const int column) const
        {
            return m_Datas.value(column, QVariant());
        }

        bool setData(int column, const QVariant &value)
        {
    //        qWarning()<< data(column) << value << (data(column)==value);
            if (data(column)==value)
                return true;
            m_Datas.insert(column, value);
            if (column==EpisodeModel::IsEpisode) {
                m_IsEpisode=value.toBool();
            }
            m_IsModified = true;
            if (!m_DirtyRows.contains(column))
                m_DirtyRows.append(column);
            return true;
        }

        QVector<int> dirtyRows() const
        {
            return m_DirtyRows;
        }

        // For sort functions
        static bool lessThan(TreeItem *item1, TreeItem *item2)
        {
            // category goes first
            // then sort by name
            bool sameType = (((item1->isEpisode()) && (item2->isEpisode())) || ((!item1->isEpisode()) && (!item2->isEpisode())));
            if (sameType)
                return item1->data(EpisodeModel::Label).toString() < item2->data(EpisodeModel::Label).toString();
            return item2->isEpisode();
        }

    private:
        TreeItem *m_Parent;
        QList<TreeItem*> m_Children;
        QVector<int> m_DirtyRows;
        bool m_IsEpisode, m_IsModified;
        QHash<int, QVariant> m_Datas;
    };


}

namespace Form {
namespace Internal {

class EpisodesCoreListener : public Core::ICoreListener
{
    Q_OBJECT
public:
    EpisodesCoreListener(Form::EpisodeModel *parent) : Core::ICoreListener(parent)
    {
        Q_ASSERT(parent);
        m_Model = parent;
    }
    ~EpisodesCoreListener() {}

    bool coreAboutToClose()
    {
        if (m_Model->isDirty()) {
            bool yes = Utils::yesNoMessageBox(tr("Save episodes."),
                                              tr("Some datas are not actually saved into database."
                                                 "Do you want to save them ?\n Answering 'No' will cause definitive data lose."),
                                              "");
            if (yes) {
                return m_Model->submit();
            } else {
                m_Model->revert();
                return true;
            }
        }
        return false;
    }

private:
    Form::EpisodeModel *m_Model;
};


class EpisodeModelPrivate
{
public:
    EpisodeModelPrivate(EpisodeModel *parent) :
            m_Sql(0), m_RootItem(0),
            m_FormTreeCreated(false),
            m_ReadOnly(false),
            m_ActualEpisode(0),
            q(parent)
    {
    }

    ~EpisodeModelPrivate ()
    {
        if (m_Sql) {
            delete m_Sql;
            m_Sql = 0;
        }
    }

    void createFormTree()
    {
        if (m_FormTreeCreated)
            return;

        if (m_RootItem) {
            delete m_RootItem;
            m_RootItem = 0;
        }

        // create root item
        QHash<int, QVariant> datas;
        datas.insert(EpisodeModel::Label, "ROOT_ITEM");
        m_RootItem = new TreeItem(datas, 0);
        m_RootItem->setIsEpisode(false);

        // getting Forms
        LOG_FOR(q, "Getting Forms");
        // create one item per form
        formsItems.clear();
        foreach(Form::FormMain *f, m_RootForm->flattenFormMainChildren()) {
            datas.clear();
            datas.insert(EpisodeModel::FormUuid, f->uuid());
            datas.insert(EpisodeModel::Label, f->spec()->label());
            QString icon = f->spec()->value(FormItemSpec::Spec_IconFileName).toString();
            icon.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::SmallPixmapPath));
            if (QFileInfo(icon).isRelative())
                icon.append(qApp->applicationDirPath());
            datas.insert(EpisodeModel::Icon, QIcon(icon));
            TreeItem *it = new TreeItem(datas, 0);
            it->setIsEpisode(false);
            formsItems.insert(f, it);
        }
        // reparent items
        foreach(Form::FormMain *f, m_RootForm->flattenFormMainChildren()) {
            TreeItem *it = formsItems.value(f);
            if (f->formParent() != m_RootForm) {
                it->setParent(formsItems.value(f->formParent()));
                it->parent()->addChildren(it);
            } else {
                it->setParent(m_RootItem);
                m_RootItem->addChildren(it);
            }
            it->setModified(false);
        }
        m_FormTreeCreated = true;
    }

    void deleteEpisodes(TreeItem *item)
    {
        if (!item)
            return;
        if (item->isEpisode()) {
            item->parent()->removeChild(item);
            delete item;
            return;
        }
        item->removeEpisodes();
        int nb = item->childCount();
        for(int i = 0; i < nb; ++i) {
            deleteEpisodes(item->child(i));
        }
    }

    void refreshEpisodes()
    {
        // make sure that all actual episodes are saved into database
        if (!saveEpisode(m_ActualEpisode, m_ActualEpisode_FormUid))
            LOG_ERROR_FOR(q, "Unable to save actual episode");

        // delete old episodes
        deleteEpisodes(m_RootItem);
        m_ActualEpisode = 0;
        m_ActualEpisode_FormUid = "";

        // get Episodes
        LOG_FOR(q, "Getting Episodes (refresh)");
        QSqlQuery query(episodeBase()->database());
        QList<TreeItem *> episodes;

        /** \todo add LIMIT to SQL command */
        QHash<int, QString> where;
        where.insert(Constants::EPISODES_PATIENT_UID, QString("='%1'").arg(m_CurrentPatient));
        foreach(Form::FormMain *f, formsItems.keys()) {
            TreeItem *parent = formsItems.value(f);
            where.insert(Constants::EPISODES_FORM_PAGE_UID, QString("='%1'").arg(f->uuid()));
            QString req = episodeBase()->select(Form::Constants::Table_EPISODES,
                                                QList<int>() << Constants::EPISODES_ID
                                                << Constants::EPISODES_DATE
                                                << Constants::EPISODES_LABEL,
                                                where);
            int limit;
            f->episodePossibilities()==FormMain::UniqueEpisode ? limit=1 : limit=5;
            req += QString(" ORDER BY %1 ASC LIMIT %2;")
                   .arg(episodeBase()->fieldName(Constants::Table_EPISODES, Constants::EPISODES_DATE))
                   .arg(limit);
            query.exec(req);
            if (query.isActive()) {
                QHash<int, QVariant> datas;
                QList<TreeItem *> items;
                while (query.next()) {
                    datas.insert(EpisodeModel::Id, query.value(0));
                    datas.insert(EpisodeModel::Date, query.value(1));
                    datas.insert(EpisodeModel::FormUuid, f->uuid());
                    datas.insert(EpisodeModel::Label, query.value(2));
                    TreeItem *it = new TreeItem(datas, 0);
                    it->setIsEpisode(true);
                    it->setModified(false);
                    episodes.insert(datas.value(EpisodeModel::Id).toInt(), it);
                    items.prepend(it);
                    datas.clear();
                }
                // reparent items in reverse order (items.prepend -- not append) to keep the SQL order
                int zz = 0;
                foreach(TreeItem *it, items) {
                    it->setParent(parent);
                    parent->insertChild(zz, it);
                    ++zz;
                }
            } else {
                LOG_QUERY_ERROR_FOR(q, query);
            }
            query.finish();
        }
//        sortEpisodes();
    }

    TreeItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if (item) return item;
        }
        return m_RootItem;
    }

    void getXmlContent(TreeItem *item)
    {
//        qWarning() << "getXmlContent" << item->data(EpisodeModel::Label);
        /** \todo create a QCache system to avoid memory overusage */
        if (item->isNewlyCreated())
            return;
        QHash<int, QString> where;
        where.insert(Constants::EPISODE_CONTENT_EPISODE_ID, QString("=%1").arg(item->data(EpisodeModel::Id).toString()));
        QString req = episodeBase()->select(Constants::Table_EPISODE_CONTENT, Constants::EPISODE_CONTENT_XML, where);
        QSqlQuery query(episodeBase()->database());
        query.exec(req);
        if (query.isActive()) {
            if (query.next()) {
                item->setData(EpisodeModel::XmlContent, query.value(0));
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
        }
        query.finish();
    }

    bool saveFullEpisode(TreeItem *item, const QString &formUid)
    {
        bool ok = true;
        // first, save Table_EPISODE
        QSqlQuery query(episodeBase()->database());
        query.prepare(episodeBase()->prepareInsertQuery(Constants::Table_EPISODES));
        query.bindValue(Constants::EPISODES_ID, QVariant());
        query.bindValue(Constants::EPISODES_PATIENT_UID, m_CurrentPatient);
        query.bindValue(Constants::EPISODES_LK_TOPRACT_LKID, m_LkIds.toInt());
        query.bindValue(Constants::EPISODES_FORM_PAGE_UID, formUid);
        query.bindValue(Constants::EPISODES_LABEL, item->data(EpisodeModel::Label));
        query.bindValue(Constants::EPISODES_DATE, item->data(EpisodeModel::Date));
        query.bindValue(Constants::EPISODES_DATEOFCREATION, item->data(EpisodeModel::Date));
        query.bindValue(Constants::EPISODES_DATEOFMODIFICATION, QVariant());
        query.bindValue(Constants::EPISODES_DATEOFVALIDATION, QVariant());
        query.bindValue(Constants::EPISODES_VALIDATED, QVariant());
        if (!query.exec()) {
            ok = false;
            LOG_QUERY_ERROR_FOR(q, query);
        }
        item->setData(EpisodeModel::Id, query.lastInsertId());
        query.finish();

        // second, save xmlcontent
        query.prepare(episodeBase()->prepareInsertQuery(Constants::Table_EPISODE_CONTENT));
        query.bindValue(Constants::EPISODE_CONTENT_ID, QVariant());
        query.bindValue(Constants::EPISODE_CONTENT_EPISODE_ID, item->data(EpisodeModel::Id));
        query.bindValue(Constants::EPISODE_CONTENT_XML, item->data(EpisodeModel::XmlContent));
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR(q, query);
            ok = false;
        }
        query.finish();

        if (ok) {
            item->setNewlyCreated(false);
            item->setModified(false);
        }

        return ok;
    }

    bool saveEpisode(TreeItem *item, const QString &fuid)
    {
        if (!item)
            return true;

        QString formUid = fuid;
        if (formUid.isEmpty())
            formUid = item->data(EpisodeModel::FormUuid).toString();

        qWarning() << "saveEpisode" << item << formUid;
        // check each item --> isModified ? isNewlyCreated ?
        /** \todo isNewlyCreated */
        FormMain *form = m_RootForm->formMainChild(formUid);
        if (!form)
            return false;

        bool formIsModified = false;
        QHash<QString, FormItem *> items;
        if (item->isNewlyCreated()) {
            saveFullEpisode(item, formUid);
        } else {
            foreach(FormItem *it, form->flattenFormItemChildren()) {
                /** \todo check nested items */
                if (it->itemDatas()) {
                    if (it->itemDatas()->isModified()) {
                        qWarning() << it->uuid() << "is modified";
                        formIsModified = true;
                    }
                    items.insert(it->uuid(), it);
                }
            }
        }

        // no changes == nothing to do
        if (formIsModified) {
            // ask user what to do
            if (!settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, false).toBool()) {
                bool yes = Utils::yesNoMessageBox(QApplication::tr("EpisodeModel", "Save episode ?"),
                                                  QApplication::tr("EpisodeModel", "The actual episode has been modified. Do you want to save changes in your database ?\n"
                                                     "Answering 'No' will cause definitve data lose."),
                                                  "", QApplication::tr("EpisodeModel", "Save episode"));
                if (!yes) {
                    saveEpisodeHeaders(form, item);
                    return true;
                }
            }

            // create the XML episode file
            QHash<QString, QString> datas;
            foreach(FormItem *it, items) {
                datas.insert(it->uuid(), it->itemDatas()->storableData().toString());
            }

            QString xml = Utils::createXml(Form::Constants::XML_FORM_GENERAL_TAG, datas, 2, false);

            item->setData(EpisodeModel::XmlContent, xml);
            //        qWarning() << "SAVE" << item->data(EpisodeModel::XmlContent);

            // save the XML episode into temporary file
            /** \todo save modified xmlcontent to the item (into a temp db ?), create a cache for the xmlcontent. */
            // Utils::saveStringToFile(xml, m_TmpFile, Utils::AppendToFile, Utils::DontWarnUser);
            QSqlQuery query(episodeBase()->database());
            QHash<int, QString> where;
            where.insert(Constants::EPISODE_CONTENT_EPISODE_ID, "="+QString::number(item->data(EpisodeModel::Id).toInt()));
            // first check that the episodeId already exists
            int count = episodeBase()->count(Constants::Table_EPISODE_CONTENT, Constants::EPISODE_CONTENT_EPISODE_ID, episodeBase()->getWhereClause(Constants::Table_EPISODE_CONTENT, where));
            if (count) {
                query.prepare(episodeBase()->prepareUpdateQuery(Constants::Table_EPISODE_CONTENT, Constants::EPISODE_CONTENT_XML, where));
                query.bindValue(0, xml);
            } else {
                query.prepare(episodeBase()->prepareInsertQuery(Constants::Table_EPISODE_CONTENT));
                query.bindValue(Constants::EPISODE_CONTENT_ID, QVariant());
                query.bindValue(Constants::EPISODE_CONTENT_EPISODE_ID, item->data(EpisodeModel::Id).toInt());
                query.bindValue(Constants::EPISODE_CONTENT_XML, xml);
            }
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
            }
            query.finish();
        }

        saveEpisodeHeaders(form, item);
        item->setModified(false);

//        foreach(FormItem *it, form->formItemChildren()) {
//            /** \todo check nested items */
//            if (it->itemDatas()) {
//                    it->itemDatas()->
//                        qWarning() << it->uuid() << "is modified";
//                        formIsModified = true;
//                    }
//                    items.insert(it->uuid(), it);
//                }
//            }
//        }

        return true;
    }

    void saveEpisodeHeaders(FormMain *form, TreeItem *itemToSave)
    {
        // retreive date/label of episode
        const QString &newLabel = form->itemDatas()->data(0, IFormItemData::ID_EpisodeLabel).toString();
        const QDateTime &newDate = form->itemDatas()->data(0, IFormItemData::ID_EpisodeDate).toDateTime();
        if ((newLabel==itemToSave->data(EpisodeModel::Label).toString()) &&
            (newDate==itemToSave->data(EpisodeModel::Date).toDateTime()))
            return;

        const QString &episodeId = itemToSave->data(EpisodeModel::Id).toString();
        itemToSave->setData(EpisodeModel::Label, newLabel);
        itemToSave->setData(EpisodeModel::Date, newDate);

        /** \todo emit dataChanged signal with the right index */

        // save label and date
        QSqlQuery query(episodeBase()->database());
        QHash<int, QString> where;
        where.insert(Constants::EPISODES_ID, "="+episodeId);
        query.prepare(episodeBase()->prepareUpdateQuery(Constants::Table_EPISODES, Constants::EPISODES_LABEL, where));
        query.bindValue(0, itemToSave->data(EpisodeModel::Label));
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR(q, query);
        }
        query.finish();
        query.prepare(episodeBase()->prepareUpdateQuery(Constants::Table_EPISODES, Constants::EPISODES_DATE, where));
        query.bindValue(0, itemToSave->data(EpisodeModel::Date));
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR(q, query);
        }
        query.finish();
    }

    void feedItemDatasWithXmlContent(Form::FormMain *form, TreeItem *episode, bool feedPatientModel = false)
    {
        getXmlContent(episode);
        const QString &xml = episode->data(EpisodeModel::XmlContent).toString();
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
        foreach(FormItem *it, form->formItemChildren()) {
            /** \todo check nested items */
            items.insert(it->uuid(), it);
        }

        // feed the formitemdatas for this form and get the data for the patientmodel
        foreach(FormItem *it, items.values()) {
            if (!it) {
                qWarning() << "FormManager::activateForm :: ERROR : no item :" << items.key(it);
                continue;
            }
            if (it->itemDatas()) {
                it->itemDatas()->setStorableData(datas.value(it->uuid()));
                if (feedPatientModel) {
                    patient()->setValue(it->patientDataRepresentation(), it->itemDatas()->data(it->patientDataRepresentation(), IFormItemData::ID_ForPatientModel));
                }
            }
            else
                qWarning() << "FormManager::activateForm :: ERROR : no itemData :" << items.key(it);
        }
    }

    void getLastEpisodesAndFeedPatientModel()
    {
        if (m_CurrentPatient.isEmpty())
            return;

        QHash<int, QString> where;
        where.insert(Constants::EPISODES_PATIENT_UID, QString("='%1'").arg(m_CurrentPatient));

        foreach(Form::FormMain *f, formsItems.keys()) {

            // test all children FormItem for patientDataRepresentation
            bool hasPatientDatas = false;
            foreach(Form::FormItem *item, f->formItemChildren()) {
                if (item->itemDatas()) {
                    if (item->patientDataRepresentation()!=-1) {
                        hasPatientDatas = true;
                        break;
                    }
                }
            }
            qWarning() << "<<<<<<<<<<<< test last episode for"<< f->spec()->label() << hasPatientDatas;
            if (!hasPatientDatas)
                continue;

            // get the form's XML content for the last episode, feed it with the XML code
            TreeItem *formItem = formsItems.value(f);
            if (!formItem->childCount()) {
                qWarning() << "<<<<<<<<<<<<   no episode";
                continue;  // No episodes
            }

            // get last episode
            int i = 0;
            TreeItem *lastEpisode = formItem->child(i);
            while (!lastEpisode->isEpisode() && i < formItem->childCount())
                lastEpisode = formItem->child(i);

            qWarning() << "Last episode" << lastEpisode->data(EpisodeModel::Label) << lastEpisode->isEpisode();

            if (lastEpisode->isEpisode()) {
                feedItemDatasWithXmlContent(f, lastEpisode, true);
            }
        }
    }

//    void feedPatientModelWithLastEpisodes()
//    {
//        /** \todo here --> read all last episodes and feed patient model with represented datas */
//        foreach(Form::FormMain *f, formManager()->forms()) {
//            foreach(Form::FormItem *item, f->formItemChildren()) {
//                if (item->itemDatas()) {
//                    if (item->patientDataRepresentation()!=-1)
//                        patient()->setValue(item->patientDataRepresentation(), item->itemDatas()->data(item->patientDataRepresentation(), Form::IFormItemData::ID_ForPatientModel));
//                }
//            }
//        }
//    }

public:
    FormMain *m_RootForm;
    QSqlTableModel *m_Sql;
    TreeItem *m_RootItem;
    QString m_UserUuid, m_LkIds, m_CurrentPatient, m_CurrentForm;
    bool m_FormTreeCreated;
    bool m_ReadOnly;
    QMap<Form::FormMain *, TreeItem *> formsItems;
    TreeItem *m_ActualEpisode;
    QString m_ActualEpisode_FormUid;
    QString m_TmpFile;

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
    init();
}

void EpisodeModel::init()
{
    d->m_UserUuid = currentUserUuid();
    d->m_TmpFile = settings()->path(Core::ISettings::ApplicationTempPath) + "/FreeMedForms_Episodes.xml";
    LOG("Using temporary path " + d->m_TmpFile);
    connect(user(), SIGNAL(userChanged()), this, SLOT(onUserChanged()));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onPatientChanged()));

    d->m_CurrentPatient = patient()->data(Core::IPatient::Uid).toString();

    d->createFormTree();

//    d->connectSqlPatientSignals();
    onUserChanged();

    // connect the save action
    Core::Command * cmd = actionManager()->command(Core::Constants::A_FILE_SAVE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(submit()));

//    setSupportedDragActions(Qt::CopyAction | Qt::MoveAction);
//
//    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

EpisodeModel::~EpisodeModel()
{
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
    reset();
}

void EpisodeModel::onUserChanged()
{
    d->m_UserUuid = currentUserUuid();
    /** \todo code here */
//    QList<int> ids = episodeBase()->retreivePractionnerLkIds(uuid);
//    d->m_LkIds.clear();
//    foreach(int i, ids)
//        d->m_LkIds.append(QString::number(i) + ",");
//    d->m_LkIds.chop(1);
    d->refreshEpisodes();
}

void EpisodeModel::onPatientChanged()
{
    qWarning() << Q_FUNC_INFO;
    d->m_CurrentPatient = patient()->data(Core::IPatient::Uid).toString();
    d->refreshEpisodes();
    d->getLastEpisodesAndFeedPatientModel();
    reset();
    qWarning() << ",,,,,,,";
}

void EpisodeModel::setCurrentFormUuid(const QString &uuid)
{
    d->m_CurrentForm = uuid;
}

QModelIndex EpisodeModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

     TreeItem *parentItem = d->getItem(parent);
     TreeItem *childItem = 0;
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

     TreeItem *childItem = d->getItem(index);
     TreeItem *parentItem = childItem->parent();

     if (parentItem == d->m_RootItem)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

int EpisodeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *item = d->getItem(parent);
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

    TreeItem *it = d->getItem(item);

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
        {
            if (item.column()==Label && it->isEpisode()) {
                return it->data(Date).toDate().toString(settings()->value(Constants::S_EPISODEMODEL_DATEFORMAT, "dd MMM yyyy").toString()) + " - " + it->data(item.column()).toString();
            }
            if (item.column()==XmlContent && it->isEpisode()) {
                if (it->data(XmlContent).isNull()) {
                    d->getXmlContent(it);
                }
                return it->data(XmlContent);
            }
            return it->data(item.column());
        }
    case Qt::ToolTipRole :
        {
            if (item.column()==Label && it->isEpisode()) {
                return QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                        .arg(it->data(Date).toDate().toString(settings()->value(Constants::S_EPISODEMODEL_DATEFORMAT, "dd MMM yyyy").toString()).replace(" ", "&nbsp;"))
                        .arg(it->data(item.column()).toString().replace(" ", "&nbsp;"))
                        .arg(user()->value(Core::IUser::FullName).toString());
            }
//            if (!it->isEpisode()) {
//                return it->data(EpisodeModel::FormToolTip);
//            }
            break;
        }
    case Qt::ForegroundRole :
        {
            if (it->isEpisode()) {
                return QColor(settings()->value(Constants::S_EPISODEMODEL_EPISODE_FOREGROUND, "darkblue").toString());
            } else {
                /** \todo remove this */
                Form::FormMain *form = d->formsItems.key(it);
                if (form) {
                    if (form->episodePossibilities()==FormMain::UniqueEpisode)
                        return QColor("red");
                }
                // End remove
                return QColor(settings()->value(Constants::S_EPISODEMODEL_FORM_FOREGROUND, "#000").toString());
            }
        }
    case Qt::FontRole :
        {
            if (!it->isEpisode()) {
                QFont bold;
                bold.setBold(true);
                return bold;
            }
            return QFont();
        }
    case Qt::DecorationRole :
        {
            return it->data(Icon);
        }
//    case Qt::BackgroundRole :
//        {
//            QColor c;
//            if (it->isTemplate()) {
//                c = QColor(settings()->value(Constants::S_BACKGROUND_TEMPLATES, "white").toString());
//            } else {
//                c = QColor(settings()->value(Constants::S_BACKGROUND_CATEGORIES, "white").toString());
//            }
//            if (Utils::isDebugCompilation()) {
//                if (it->isNewlyCreated()) {
//                    c = QColor(Qt::blue);
//                } else
//                    if (it->isModified()) {
//                    c = QColor(Qt::red);
//                }
//            }
//            if (c.name()=="#ffffff")
//                return QVariant();
//            c.setAlpha(125);
//            return c;
//        }
    }
    return QVariant();
}

bool EpisodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (d->m_ReadOnly)
        return false;

    if (!index.isValid())
        return false;

    TreeItem *it = d->getItem(index);
    if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        it->setData(index.column(), value);
        // emit from all instances
        Q_EMIT dataChanged(index, index);
    }
    return true;
}

Qt::ItemFlags EpisodeModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant EpisodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

bool EpisodeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qWarning() << "insertRows" << row << count << parent.data();
    if (d->m_ReadOnly)
        return false;

    if (!parent.isValid())
        return false;

    TreeItem *parentItem = d->getItem(parent);
    if (!parentItem)
        return false;

    if (parentItem->isEpisode())
        return false;

    beginInsertRows(parent, row, row + count);

    QHash<int, QVariant> datas;
    datas.insert(EpisodeModel::Id, QVariant());
    datas.insert(EpisodeModel::Date, QDateTime::currentDateTime());
    datas.insert(EpisodeModel::FormUuid, d->formsItems.key(parentItem)->uuid());
    datas.insert(EpisodeModel::Label, QVariant());

    TreeItem *it = new TreeItem(datas, parentItem);
    it->setIsEpisode(true);
    it->setModified(false);
    it->setNewlyCreated(true);

    parentItem->insertChild(0, it);

    endInsertRows();

//    activateEpisode();
    return true;
}

bool EpisodeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    //    qWarning() << "removeRows" << row << count;
    if (d->m_ReadOnly)
        return false;

//    TreeItem *parentItem = 0;
//    if (!parent.isValid())
//        parentItem = d->m_RootItem;
//    else
//        parentItem = d->getItem(parent);
//
//    beginRemoveRows(parent, row, row+count-1);
//
//    for(int i=0; i<count; ++i) {
//        TreeItem *item = parentItem->child(row+i);
//        int id = item->id();
//        if (item->isTemplate() && (!d->m_TemplatesToDelete.contains(id)))
//            d->m_TemplatesToDelete.append(id);
//        else if ((!item->isTemplate())  && (!d->m_CategoriesToDelete.contains(id)))
//            d->m_CategoriesToDelete.append(id);
//        parentItem->removeChild(item);
//        delete item;
//        item = 0;
//    }
//
//    endRemoveRows();

    //    qWarning() << "removeRows" << d->m_CategoriesToDelete << d->m_TemplatesToDelete;
    return true;
}

bool EpisodeModel::isEpisode(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    const TreeItem *it = d->getItem(index);
    return it->isEpisode();
}

bool EpisodeModel::isUniqueEpisode(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    TreeItem *item = d->getItem(index);
    FormMain *form = d->m_RootForm->formMainChild(item->data(FormUuid).toString());
    if (form)
        return form->episodePossibilities()==FormMain::UniqueEpisode;
    return false;
}

bool EpisodeModel::isNoEpisode(const QModelIndex &index)
{
    if (!index.isValid())
        return false;
    TreeItem *item = d->getItem(index);
    FormMain *form = d->m_RootForm->formMainChild(item->data(FormUuid).toString());
    if (form)
        return form->episodePossibilities()==FormMain::NoEpisode;
    return false;
}

void EpisodeModel::setReadOnly(const bool state)
{
    d->m_ReadOnly = state;
}

bool EpisodeModel::isReadOnly() const
{
    return d->m_ReadOnly;
}

bool EpisodeModel::isDirty() const
{
    return false;
}

bool EpisodeModel::submit()
{
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
    // submit actual episode
    submit();

    if (!index.isValid()) {
        d->m_ActualEpisode = 0;
        return false;
    }

    // stores the actual episode id
    TreeItem *item = d->getItem(index);
    if (!item->isEpisode()) {
        d->m_ActualEpisode = 0;
        return false;
    }
    d->m_ActualEpisode = item;
    d->m_ActualEpisode_FormUid = formUid;

    // clear actual form and fill episode datas
    FormMain *form = d->m_RootForm->formMainChild(formUid);
    if (!form)
        return false;
    form->clear();
    form->itemDatas()->setData(0, d->m_ActualEpisode->data(Date), IFormItemData::ID_EpisodeDate);
    form->itemDatas()->setData(0, d->m_ActualEpisode->data(Label), IFormItemData::ID_EpisodeLabel);
    const QString &username = user()->value(Core::IUser::FullName).toString();
    if (username.isEmpty())
        form->itemDatas()->setData(0, tr("No user"), IFormItemData::ID_UserName);
    else
        form->itemDatas()->setData(0, username, IFormItemData::ID_UserName);

    qWarning() << "EpisodeModel::activateEpisode" << d->m_ActualEpisode->data(Id).toInt() << formUid;


    /** \todo move this part into a specific member of the private part */
    QString xml = d->m_ActualEpisode->data(XmlContent).toString();
    if (xml.isEmpty()) {
        d->getXmlContent(d->m_ActualEpisode);
        xml = d->m_ActualEpisode->data(XmlContent).toString();
    }

    if (xml.isEmpty())
        return true;

    // read the xml'd content
    QHash<QString, QString> datas;
    if (!Utils::readXml(xml, Form::Constants::XML_FORM_GENERAL_TAG, datas, false)) {
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

bool EpisodeModel::saveEpisode(const QModelIndex &index, const QString &formUid)
{
    return d->saveEpisode(d->getItem(index), formUid);
}

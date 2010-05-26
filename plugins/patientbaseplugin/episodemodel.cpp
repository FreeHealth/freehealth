/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#include "episodemodel.h"
#include "patientbase.h"
#include "constants_db.h"
#include "constants_settings.h"

#include <usermanagerplugin/usermodel.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>

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


using namespace Patients;

using namespace Trans::ConstantTranslations;

static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}
static inline QString currentUserUuid() {return userModel()->currentUserData(UserPlugin::User::Uuid).toString();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }


namespace Patients {
namespace Internal {

class EpisodesCoreListener : public Core::ICoreListener
{
    Q_OBJECT
public:
    EpisodesCoreListener(Patients::EpisodeModel *parent) : Core::ICoreListener(parent)
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
    Patients::EpisodeModel *m_Model;
};


/** \todo create an Utils::GenericTreeItem \sa TemplateModel */
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


class EpisodeModelPrivate
{
public:
    EpisodeModelPrivate(EpisodeModel *parent) :
            m_Sql(0), m_RootItem(0),
            m_FormTreeCreated(false),
            q(parent)
    {
        m_UserUuid = currentUserUuid();
        q->connect(userModel(), SIGNAL(userConnected(QString)), q, SLOT(setCurrentUser(QString)));
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
        }

        // create root item
        QHash<int, QVariant> datas;
        datas.insert(EpisodeModel::Label, "ROOT_ITEM");
        m_RootItem = new TreeItem(datas, 0);
        m_RootItem->setIsEpisode(false);

        // getting Forms
        Utils::Log::addMessage(q, "Getting Forms");
        // create one item per form
        formsItems.clear();
        foreach(Form::FormMain *f, formManager()->forms()) {
            datas.clear();
            datas.insert(EpisodeModel::FormUuid, f->uuid());
            datas.insert(EpisodeModel::Label, f->spec()->label());
            TreeItem *it = new TreeItem(datas, 0);
            it->setIsEpisode(false);
            formsItems.insert(f, it);
        }
        // reparent items
        QHashIterator<Form::FormMain *, TreeItem *> i(formsItems);
        while (i.hasNext()) {
            i.next();
            Form::FormMain *f = i.key();
            TreeItem *it = i.value();
            if (f->formParent()) {
                it->setParent(formsItems.value(f->formParent()));
                it->parent()->addChildren(it);
            } else {
                it->setParent(m_RootItem);
                m_RootItem->addChildren(it);
            }
            it->setModified(false);
        }
//        sortItems();
        m_FormTreeCreated = true;
    }

    void refreshEpisodes()
    {
        /** \todo here */
        // make sure that episode are saved into database

        // delete old episodes

        // get Episodes
        Utils::Log::addMessage(q, "Getting Episodes");
        QSqlQuery query(patientBase()->database());
        QList<TreeItem *> episodes;

        /** \todo add LIMIT to SQL command */
        QHash<int, QString> where;
        where.insert(Constants::EPISODES_PATIENT_UID, QString("='%1'").arg(m_CurrentPatient));
//        where.insert(Constants::EPISODES_DATE, QString(" > %1").arg(QDateTime::currentDateTime().addYears(-1).toString(Qt::ISODate)));

        foreach(Form::FormMain *f, formsItems.keys()) {
            TreeItem *parent = formsItems.value(f);
            where.insert(Constants::EPISODES_FORM_PAGE_UID, QString("='%1'").arg(f->uuid()));
            QString req = patientBase()->select(Patients::Constants::Table_EPISODES,
                                                QList<int>() << Constants::EPISODES_ID
                                                << Constants::EPISODES_DATE
                                                << Constants::EPISODES_LABEL,
                                                where);
            req += QString(" ORDER BY %1 ASC LIMIT 5;").arg(patientBase()->field(Constants::Table_EPISODES, Constants::EPISODES_DATE));
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
                foreach(TreeItem *it, items) {
                    it->setParent(parent);
                    parent->addChildren(it);
                }
            } else {
                Utils::Log::addQueryError(q, query);
            }
            query.finish();
        }
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
        /** \todo create a QCache system to avoid memory overusage */
        QHash<int, QString> where;
        where.insert(Constants::EPISODE_CONTENT_EPISODE_ID, QString("=%1").arg(item->data(EpisodeModel::Id).toString()));
        QString req = patientBase()->select(Patients::Constants::Table_EPISODE_CONTENT, Constants::EPISODE_CONTENT_XML, where);
        QSqlQuery query(patientBase()->database());
        query.exec(req);
        if (query.isActive()) {
            if (query.next()) {
                item->setData(EpisodeModel::XmlContent, query.value(0));
            }
        } else {
            Utils::Log::addQueryError(q, query);
        }
        query.finish();
    }

public:
    QSqlTableModel *m_Sql;
    TreeItem *m_RootItem;
    QString m_UserUuid, m_LkIds, m_CurrentPatient, m_CurrentForm;
    bool m_FormTreeCreated;
    bool m_ReadOnly;
    QHash<Form::FormMain *, TreeItem *> formsItems;

private:
    EpisodeModel *q;
};
}
}


EpisodeModel::EpisodeModel(QObject *parent) :
        QAbstractItemModel(parent), d(new Internal::EpisodeModelPrivate(this))
{
    setObjectName("EpisodeModel");
//    d->connectSqlPatientSignals();
    setCurrentUser(d->m_UserUuid);
    d->createFormTree();
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

void EpisodeModel::setCurrentUser(const QString &uuid)
{
    d->m_UserUuid = uuid;
    QList<int> ids = patientBase()->retreivePractionnerLkIds(uuid);
    d->m_LkIds.clear();
    foreach(int i, ids)
        d->m_LkIds.append(QString::number(i) + ",");
    d->m_LkIds.chop(1);
    d->refreshEpisodes();
}

void EpisodeModel::setCurrentPatient(const QString &uuid)
{
    d->m_CurrentPatient = uuid;
    d->refreshEpisodes();
    reset();
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

     Internal::TreeItem *parentItem = d->getItem(parent);
     Internal::TreeItem *childItem = 0;
     childItem = parentItem->child(row);
     if (childItem) {
         return createIndex(row, column, childItem);
     }
     return QModelIndex();
 }

QModelIndex EpisodeModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     Internal::TreeItem *childItem = d->getItem(index);
     Internal::TreeItem *parentItem = childItem->parent();

     if (parentItem == d->m_RootItem)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

int EpisodeModel::rowCount(const QModelIndex &parent) const
{
    Internal::TreeItem *item = d->getItem(parent);
    if (item) {
        return item->childCount();
    }
    return 0;
}

int EpisodeModel::columnCount(const QModelIndex &parent) const
{
    return MaxData;
}

QVariant EpisodeModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    Internal::TreeItem *it = d->getItem(item);

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
        {
            if (item.column()==Label && it->isEpisode()) {
                return it->data(Date).toDate().toString(settings()->value(Constants::S_EPISODEMODEL_DATEFORMAT, "MM yyyy").toString()) + " - " + it->data(item.column()).toString();
            }
            if (item.column()==XmlContent && it->isEpisode()) {
                if (it->data(XmlContent).isNull()) {
                    d->getXmlContent(it);
                }
                return it->data(XmlContent);
            }
            return it->data(item.column());
        }
//    case Qt::ToolTipRole :
//        {
//            return it->data(Constants::Data_Summary);
//        }
    case Qt::ForegroundRole :
        {
            if (it->isEpisode()) {
                return QColor(settings()->value(Constants::S_EPISODEMODEL_EPISODE_FOREGROUND, "darkblue").toString());
            } else {
                return QColor(settings()->value(Constants::S_EPISODEMODEL_FORM_FOREGROUND, "#000").toString());
            }
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

    Internal::TreeItem *it = d->getItem(index);
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
    //    qWarning() << "insertRows" << row << count << parent.data();
    if (d->m_ReadOnly)
        return false;

//    Internal::TreeItem *parentItem = 0;
//    if (!parent.isValid())
//        parentItem = d->m_RootItem;
//    else
//        parentItem = d->getItem(parent);
//    //    if (!parent.isValid())
//    //        return false;
//    //    Internal::TreeItem *parentItem = d->getItem(parent);
//    QHash<int, QVariant> datas;
//    datas.insert(Constants::Data_Label, tr("New"));
//    datas.insert(Constants::Data_ParentId, parentItem->data(Constants::Data_Id));
//    datas.insert(Constants::Data_CreationDate, QDateTime::currentDateTime());
//    datas.insert(Constants::Data_IsTemplate, false);
//    d->allInstancesBeginInsertRows(parent, row, row+count-1);
//    for(int i=0; i<count; ++i) {
//        Internal::TreeItem *item = new Internal::TreeItem(datas, parentItem);
//        if (!parentItem->insertChild(row+i, item))
//            return false;
//    }
//    d->allInstancesEndInsertRows();
    return true;
}

bool EpisodeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    //    qWarning() << "removeRows" << row << count;
    if (d->m_ReadOnly)
        return false;

//    Internal::TreeItem *parentItem = 0;
//    if (!parent.isValid())
//        parentItem = d->m_RootItem;
//    else
//        parentItem = d->getItem(parent);
//
//    beginRemoveRows(parent, row, row+count-1);
//
//    for(int i=0; i<count; ++i) {
//        Internal::TreeItem *item = parentItem->child(row+i);
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

    const Internal::TreeItem *it = d->getItem(index);
    return it->isEpisode();
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
//    if (d->m_ReadOnly)
//        return false;
//
//    d->saveModelDatas();
//    d->deleteRowsInDatabase();
    return true;
}

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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Templates::TemplatesModel
  Manages the templates database in/out and link to views.
  Data are statically stored so that you can instanciate how many required models
  as you want without consuming RAM.
*/

#include "templatesmodel.h"
#include "templatescore.h"
#include "templatebase.h"
#include "itemplates.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icorelistener.h>
#include <coreplugin/isettings.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/serializer.h>
#include <utils/global.h>

#include <QList>
#include <QColor>
#include <QDir>
#include <QMimeData>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QSqlResult>
#include <QSqlDatabase>

#include <QDebug>

/**
  \todo Add a UUID to templates and categories
  \todo Add user filter, MimeType filter
  \todo Don't get the contents of templates to memory
  \todo Remove the static data. instead use a new member getData() to be called after setCategoryOnly(bool)
  \todo Detect corrupted templates database --> ask user what to do
  \todo Add a coreListener --> save templates database before closing application
  \todo Manage transmission date
*/


using namespace Templates;
using namespace Trans::ConstantTranslations;

enum { base64MimeData = true  };

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

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Templates::TemplatesCore &templateCore() {return Templates::TemplatesCore::instance();}
static inline Templates::Internal::TemplateBase *templateBase() {return templateCore().templateBase();}

namespace Templates {
namespace Internal {

class TemplatesCoreListener : public Core::ICoreListener
{
    Q_OBJECT
public:
    TemplatesCoreListener(Templates::TemplatesModel *parent) : Core::ICoreListener(parent)
    {
        Q_ASSERT(parent);
        m_Model = parent;
    }
    ~TemplatesCoreListener() {}

    bool coreAboutToClose()
    {
        if (m_Model->isDirty()) {
            bool yes = Utils::yesNoMessageBox(tr("Save templates ."),
                                   tr("Some data are not actually saved into database."
                                      "Do you want to save them?\n Answering 'No' will cause definitive data loss."),
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
    Templates::TemplatesModel *m_Model;
};

// FIXME: use a QStandardItemModel instead of an internal specific pointer model
class TreeItem : public Templates::ITemplate
{
public:
    TreeItem(const QHash<int, QVariant> &hashData, TreeItem *parent = 0) :
            ITemplate(hashData),
            m_Parent(parent),
            m_IsTemplate(false),
            m_IsModified(false)
    {
        setData(Constants::Data_UserUuid, "FreeDiams");
        setIsTemplate(hashData.value(Constants::Data_IsTemplate).toBool());
    }
    ~TreeItem() { qDeleteAll(m_Children); }

    // Genealogy management
    TreeItem *child(int number) { return m_Children.value(number); }
    int childCount() const { return m_Children.count(); }
    int columnCount() const { return Constants::Data_Max_Param; }
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
            if (!c->isTemplate())
                ++n;
        }
        return n;
    }
    TreeItem *categoryChild(int number)
    {
        QList<TreeItem *> cat;
        foreach(TreeItem *c, this->m_Children) {
            if (!c->isTemplate())
                cat << c;
        }
        return cat.value(number);
    }
    int categoryChildNumber() const
    {
        if (m_Parent) {
            QList<TreeItem *> cat;
            foreach(TreeItem *c, m_Parent->m_Children) {
                if (!c->isTemplate())
                    cat << c;
            }
            return cat.indexOf(const_cast<TreeItem*>(this));
        }
        return 0;
    }

    // For tree management
    void setIsTemplate(bool isTemplate) {m_IsTemplate = isTemplate; setData(Constants::Data_IsTemplate, isTemplate); }
    bool isTemplate() const {return m_IsTemplate;}

    // For database management
    void setModified(bool state)
    {
        m_IsModified = state;
        if (!state)
            m_DirtyRows.clear();
    }
    bool isModified() const {return m_IsModified;}
    void setNewlyCreated(bool state) {setData(Constants::Data_IsNewlyCreated, state); }
    bool isNewlyCreated() const {return data(Constants::Data_IsNewlyCreated).toBool();}

    bool removeChild(TreeItem *child)
    {
        if (m_Children.contains(child)) {
            m_Children.removeAll(child);
            return true;
        }
        return false;
    }

    // For data management
    bool setData(int column, const QVariant &value)
    {
//        qWarning()<< data(column) << value << (data(column)==value);
        if (data(column)==value)
            return true;
        ITemplate::setData(column, value);
        if (column==Constants::Data_IsTemplate) {
            m_IsTemplate=value.toBool();
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
        bool sameType = (((item1->isTemplate()) && (item2->isTemplate())) || ((!item1->isTemplate()) && (!item2->isTemplate())));
        if (sameType)
            return item1->data(Constants::Data_Label).toString() < item2->data(Constants::Data_Label).toString();
        return item2->isTemplate();
    }

private:
    TreeItem *m_Parent;
    QList<TreeItem*> m_Children;
    QVector<int> m_DirtyRows;
    bool m_IsTemplate, m_IsModified;
};

class TemplatesModelPrivate
{
public:
    TemplatesModelPrivate(Templates::TemplatesModel *parent) :
            q(parent), m_RootItem(0),
            m_ShowOnlyCategories(false),
            m_ReadOnly(false)
    {
        m_Handles.insert(this);
        m_RootItem = m_Tree;
    }

    ~TemplatesModelPrivate()
    {
        m_Handles.remove(this);
        if (m_Handles.count()==0) {
            if (m_Tree) {
                delete m_Tree;
                m_Tree = 0;
                m_RootItem = 0;
            }
            m_ModelDataRetrieved = false;
        }
    }

    // FIXME: create a Core to manage all these members
    // FIXME: reset -> split into beginResetModel && endResetModel
    void allInstancesReset() const
    {
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            pr->q->beginResetModel();
            pr->q->endResetModel();
        }
    }

    void allInstancesEmitDataChangedFrom(const QModelIndex &item)
    {
        // TODO: improve this
//        bool isTemplate = q->isTemplate(item);
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                TemplatesModel *model = pr->q;
                Q_EMIT(model->dataChanged(model->index(item.row(), 0, item.parent()),
                                          model->index(item.row(), Constants::Data_Max_Param, item.parent())));
                }
            }
    }

    void allInstancesBeginInsertRows(const QModelIndex &parent, int first, int last)
    {
        // TODO: improve this
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                QModelIndex idx = pr->q->index(parent.row(), parent.column(), parent.parent());
                pr->q->beginInsertRows(idx, first, last);
            }
        }
    }

    void allInstancesEndInsertRows()
    {
        // TODO: improve this
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                pr->q->endInsertRows();
            }
        }
    }

    void allInstancesBeginRemoveRows(const QModelIndex &parent, int first, int last)
    {
        // TODO: improve this
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                pr->q->beginRemoveRows(parent,first,last);
            }
        }
    }

    void allInstancesEndRemoveRows()
    {
        // TODO: improve this
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                pr->q->endRemoveRows();
                }
            }
    }

    void setupModelData()
    {
        if (m_ModelDataRetrieved)
            return;

        if (m_RootItem) {
            delete m_RootItem;
        }

        // create root item
        QHash<int, QVariant> hashData;
        hashData.insert(Constants::Data_Label, "ROOT");
        hashData.insert(Constants::Data_ParentId, -1);
        m_RootItem = new TreeItem(hashData, 0);
        m_RootItem->setIsTemplate(false);
        m_Tree = m_RootItem;

        // getting categories
        LOG_FOR(q, "Getting Templates Categories");
        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
        if (!DB.isOpen()) {
            if (!DB.open()) {
                Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(Constants::DB_TEMPLATES_NAME)
                                     .arg(DB.lastError().text()),
                                     __FILE__, __LINE__);
                return;
            }
        }

        // get categories
        m_IdToCategory.clear();
        // TODO: Filter user's templates
        DB.transaction();
        QSqlQuery query(DB);
        if (query.exec(templateBase()->select(Templates::Constants::Table_Categories))) {
            QHash<int, QVariant> hashData;
            while (query.next()) {
                hashData.insert(Constants::Data_Id, query.value(Constants::CATEGORIES_ID));
                hashData.insert(Constants::Data_Uuid, query.value(Constants::CATEGORIES_UUID));
                hashData.insert(Constants::Data_UserUuid, query.value(Constants::CATEGORIES_USER_UID));
                hashData.insert(Constants::Data_ParentId, query.value(Constants::CATEGORIES_PARENT_ID));
                hashData.insert(Constants::Data_Label, query.value(Constants::CATEGORIES_LABEL));
                hashData.insert(Constants::Data_Summary, query.value(Constants::CATEGORIES_SUMMARY));
                hashData.insert(Constants::Data_CreationDate, query.value(Constants::CATEGORIES_DATECREATION));
                hashData.insert(Constants::Data_ModifDate, query.value(Constants::CATEGORIES_DATEMODIF));
                m_IdToCategory.insert(hashData.value(Constants::Data_Id).toInt(), new TreeItem(hashData,0));
                hashData.clear();
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            DB.rollback();
            return;
        }
        query.finish();
        foreach(TreeItem *item, m_IdToCategory.values()) {
            // need to be reparented
            item->setParent(m_IdToCategory.value(item->data(Constants::Data_ParentId).toInt(), m_RootItem));
            // add item to the children of its parent
            item->parent()->addChildren(item);
            item->setModified(false);
        }

        // get templates
        LOG_FOR(q, "Getting Templates");
        QList<TreeItem *> templates;
        // TODO: filter user's uuid
        if (query.exec(templateBase()->select(Templates::Constants::Table_Templates))) {
            QHash<int, QVariant> hashData;
            while (query.next()) {
                hashData.insert(Constants::Data_Id, query.value(Constants::TEMPLATE_ID));
                hashData.insert(Constants::Data_Uuid, query.value(Constants::TEMPLATE_UUID));
                hashData.insert(Constants::Data_UserUuid, query.value(Constants::TEMPLATE_USER_UID));
                hashData.insert(Constants::Data_ParentId, query.value(Constants::TEMPLATE_ID_CATEGORY));
                hashData.insert(Constants::Data_Label, query.value(Constants::TEMPLATE_LABEL));
                hashData.insert(Constants::Data_Summary, query.value(Constants::TEMPLATE_SUMMARY));
                hashData.insert(Constants::Data_Content, query.value(Constants::TEMPLATE_CONTENT));
                hashData.insert(Constants::Data_ContentMimeTypes, query.value(Constants::TEMPLATE_CONTENTMIMETYPES));
                hashData.insert(Constants::Data_CreationDate, query.value(Constants::TEMPLATE_DATECREATION));
                hashData.insert(Constants::Data_ModifDate, query.value(Constants::TEMPLATE_DATEMODIF));
                hashData.insert(Constants::Data_ThemedIcon, query.value(Constants::TEMPLATE_THEMEDICON));
                TreeItem *it = new TreeItem(hashData,0);
                it->setIsTemplate(true);
                templates.insert(hashData.value(Constants::Data_Id).toInt(), it);
                hashData.clear();
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            DB.rollback();
            return;
        }
        query.finish();
        DB.commit();

        // add templates to categories
        foreach(TreeItem *item, templates) {
            // need to be reparented
            item->setParent(m_IdToCategory.value(item->data(Constants::Data_ParentId).toInt(),m_RootItem));
            // add item to the children of its parent
            item->parent()->addChildren(item);
            item->setModified(false);
        }
        sortItems();

        m_ModelDataRetrieved = true;
    }

    void saveModelData(const QModelIndex &start = QModelIndex())
    {
        if (WarnDatabaseSaving)
            qWarning() << "saveModelData: " << start.data().toString();

        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
        if (!DB.isOpen()) {
            if (!DB.open()) {
                LOG_ERROR_FOR(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                              .arg(Constants::DB_TEMPLATES_NAME)
                              .arg(DB.lastError().text()));
                return;
            }
        }

        QModelIndex idx = start;
        DB.transaction();
        QSqlQuery query(DB);
        for(int i = 0; i< q->rowCount(start); ++i) {
            idx = q->index(i, 0, start);
            TreeItem *t = getItem(idx);
            if (WarnDatabaseSaving) {
                qWarning() << "   saving" << t->label().leftJustified(50,' ') << "user" << t->ownerUuid().leftJustified(50,' ') << "parent" << t->parent()->label().leftJustified(50,' ');
                qWarning() << "   newly" << t->isNewlyCreated() << "modified" << t->isModified();
            }
            QString req;
            if (t->isNewlyCreated()) {
                if (t->isTemplate()) {
                    query.prepare(templateBase()->prepareInsertQuery(Templates::Constants::Table_Templates));
                    query.bindValue(Constants::TEMPLATE_UUID, t->uuid());
                    query.bindValue(Constants::TEMPLATE_USER_UID, t->ownerUuid());
                    query.bindValue(Constants::TEMPLATE_ID_CATEGORY, t->parentId());
                    query.bindValue(Constants::TEMPLATE_LABEL, t->label());
                    query.bindValue(Constants::TEMPLATE_SUMMARY, t->summary());
                    query.bindValue(Constants::TEMPLATE_CONTENT, t->content());
                    query.bindValue(Constants::TEMPLATE_CONTENTMIMETYPES, t->contentMimeTypes().join(";"));
                    query.bindValue(Constants::TEMPLATE_DATECREATION, t->data(Constants::Data_CreationDate).toDate().toString(Qt::ISODate));
                    query.bindValue(Constants::TEMPLATE_DATEMODIF, t->data(Constants::Data_ModifDate).toDate().toString(Qt::ISODate));
                    query.bindValue(Constants::TEMPLATE_THEMEDICON, t->data(Constants::Data_ThemedIcon).toString());
                    query.bindValue(Constants::TEMPLATE_TRANSMISSIONDATE, QVariant());
                    if (!query.exec()) {
                        LOG_QUERY_ERROR_FOR(q, query);
                        query.finish();
                        DB.rollback();
                        return;
                    } else {
                        t->setNewlyCreated(false);
                        t->setModified(false);
                        allInstancesEmitDataChangedFrom(idx);
                    }
                    // retrieve its id
                    t->setId(query.lastInsertId().toInt());
                } else {
                    query.prepare(templateBase()->prepareInsertQuery(Templates::Constants::Table_Categories));
                    query.bindValue(Constants::CATEGORIES_UUID, t->uuid());
                    query.bindValue(Constants::CATEGORIES_USER_UID, t->ownerUuid());
                    query.bindValue(Constants::CATEGORIES_PARENT_ID, t->parentId());
                    query.bindValue(Constants::CATEGORIES_LABEL, t->label());
                    query.bindValue(Constants::CATEGORIES_SUMMARY, t->summary());
                    query.bindValue(Constants::CATEGORIES_DATECREATION, t->data(Constants::Data_CreationDate).toDate().toString(Qt::ISODate));
                    query.bindValue(Constants::CATEGORIES_DATEMODIF, t->data(Constants::Data_ModifDate).toDate().toString(Qt::ISODate));
                    query.bindValue(Constants::CATEGORIES_THEMEDICON, t->data(Constants::Data_ThemedIcon).toString());
                    query.bindValue(Constants::CATEGORIES_TRANSMISSIONDATE, QVariant());
                    // save category
                    if (!query.exec()) {
                        LOG_QUERY_ERROR_FOR(q, query);
                        query.finish();
                        DB.rollback();
                        return;
                    } else {
                        t->setNewlyCreated(false);
                        t->setModified(false);
                        allInstancesEmitDataChangedFrom(idx);
                    }
                    // retrieve its id
                    t->setId(query.lastInsertId().toInt());
//                    qWarning() << "created" << t->label() << t->id();
                    // inform children of the id
                    for(int i=0; i<t->childCount(); ++i) {
                        t->child(i)->setParentId(t->id());
                    }
                }
            } else if (t->isModified()) {
                if (t->isTemplate()) {
                    QHash<int, QString> where;
                    where.insert(Constants::TEMPLATE_ID, QString("=%1").arg(t->id()));
                    query.prepare(templateBase()->prepareUpdateQuery(Templates::Constants::Table_Templates, where));
                    query.bindValue(Constants::TEMPLATE_ID, t->id());
                    query.bindValue(Constants::TEMPLATE_UUID, t->uuid());
                    query.bindValue(Constants::TEMPLATE_USER_UID, t->ownerUuid());
                    query.bindValue(Constants::TEMPLATE_ID_CATEGORY, t->parentId());
                    query.bindValue(Constants::TEMPLATE_LABEL, t->label());
                    query.bindValue(Constants::TEMPLATE_SUMMARY, t->summary());
                    query.bindValue(Constants::TEMPLATE_CONTENT, t->content());
                    query.bindValue(Constants::TEMPLATE_CONTENTMIMETYPES, t->contentMimeTypes().join(";"));
                    query.bindValue(Constants::TEMPLATE_DATECREATION, t->data(Constants::Data_CreationDate).toDate().toString(Qt::ISODate));
                    query.bindValue(Constants::TEMPLATE_DATEMODIF, t->data(Constants::Data_ModifDate).toDate().toString(Qt::ISODate));
                    query.bindValue(Constants::TEMPLATE_THEMEDICON, t->data(Constants::Data_ThemedIcon).toString());
                    query.bindValue(Constants::TEMPLATE_TRANSMISSIONDATE, QVariant());
                    if (!query.exec()) {
                        LOG_QUERY_ERROR_FOR(q, query);
                        query.finish();
                        DB.rollback();
                        return;
                    }
                    req.clear();
                    query.finish();
                    t->setModified(false);
                    allInstancesEmitDataChangedFrom(idx);
                } else {
                    QHash<int, QString> where;
                    where.insert(Constants::TEMPLATE_ID, QString("=%1").arg(t->id()));
                    query.prepare(templateBase()->prepareUpdateQuery(Templates::Constants::Table_Categories, where));
                    query.bindValue(Constants::CATEGORIES_ID, t->id());
                    query.bindValue(Constants::CATEGORIES_UUID, t->uuid());
                    query.bindValue(Constants::CATEGORIES_USER_UID, t->ownerUuid());
                    query.bindValue(Constants::CATEGORIES_PARENT_ID, t->parentId());
                    query.bindValue(Constants::CATEGORIES_LABEL, t->label());
                    query.bindValue(Constants::CATEGORIES_SUMMARY, t->summary());
                    query.bindValue(Constants::CATEGORIES_DATECREATION, t->data(Constants::Data_CreationDate).toDate().toString(Qt::ISODate));
                    query.bindValue(Constants::CATEGORIES_DATEMODIF, t->data(Constants::Data_ModifDate).toDate().toString(Qt::ISODate));
                    query.bindValue(Constants::CATEGORIES_THEMEDICON, t->data(Constants::Data_ThemedIcon).toString());
                    query.bindValue(Constants::CATEGORIES_TRANSMISSIONDATE, QVariant());
                    if (!query.exec()) {
                        LOG_QUERY_ERROR_FOR(q, query);
                        query.finish();
                        DB.rollback();
                        return;
                    }
                    req.clear();
                    query.finish();
                    t->setModified(false);
                    allInstancesEmitDataChangedFrom(idx);
                }
            }
        }
        query.finish();
        DB.commit();

        // save all its children
        for(int i = 0; i < q->rowCount(start); ++i) {
            saveModelData(q->index(i, 0, start));
        }
    }

    QVector<int> getCategoryChildren(const int idCategory)
    {
        QVector<int> toReturn;
        QString req;
        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
        if (!DB.isOpen()) {
            if (!DB.open()) {
                LOG_ERROR_FOR(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                              .arg(Constants::DB_TEMPLATES_NAME)
                              .arg(DB.lastError().text()));
                return toReturn;
            }
        }
        DB.transaction();
        QHash<int, QString> where;
        where.insert(Constants::CATEGORIES_PARENT_ID, QString("=%1").arg(idCategory));
        req = templateBase()->select(Constants::Table_Categories, Constants::CATEGORIES_ID, where);
        QSqlQuery query(DB);
        if (!query.exec(req)) {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            DB.rollback();
            return toReturn;
        } else {
            while (query.next()) {
                toReturn << query.value(0).toInt();
                toReturn << getCategoryChildren(query.value(0).toInt());
            }
        }
        query.finish();
        DB.commit();
        return toReturn;
    }

    void deleteRowsInDatabase()
    {
        if (m_CategoriesToDelete.isEmpty() && m_TemplatesToDelete.isEmpty())
            return;
//        qWarning() << "deleteRows" << m_CategoriesToDelete << m_TemplatesToDelete;
        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
        if (!DB.isOpen()) {
            if (!DB.open()) {
                LOG_ERROR_FOR(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                              .arg(Constants::DB_TEMPLATES_NAME)
                              .arg(DB.lastError().text()));
                return;
            }
        }
        DB.transaction();
        QSqlQuery query(DB);
        QString req;

        if (m_CategoriesToDelete.count()) {
            req.clear();
            // retrieve all its children categories from db
            QVector<int> children;
            for(int i=0; i<m_CategoriesToDelete.count(); ++i) {
                children << getCategoryChildren(m_CategoriesToDelete.at(i));
                req += QString::number(m_CategoriesToDelete.at(i)) + " , ";
                }
            for(int i=0; i<children.count(); ++i) {
                req += QString::number(children.at(i)) + " , ";
            }
            req.chop(3);

            // The Trigger cleans all children of each categories
//            req = "DELETE FROM `CATEGORIES` WHERE `CATEGORY_ID` IN ( " + req + " )";
            QHash<int, QString> where;
            where.insert(Constants::CATEGORIES_ID, QString(" IN (%1)").arg(req));
            req = templateBase()->prepareDeleteQuery(Constants::Table_Categories, where);

            if (!query.exec(req)) {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                DB.rollback();
                return;
            } else {
                m_CategoriesToDelete.clear();
            }
        }

        req.clear();
        for(int i=0; i<m_TemplatesToDelete.count(); ++i) {
            req += QString::number(m_TemplatesToDelete.at(i)) + " , ";
        }
        req.chop(3);
        if (!req.isEmpty()) {
//            req = "DELETE FROM `TEMPLATES` WHERE `TEMPLATE_ID` IN ( " + req + " )";
            QHash<int, QString> where;
            where.insert(Constants::TEMPLATE_ID, QString(" IN (%1)").arg(req));
            req = templateBase()->prepareDeleteQuery(Constants::Table_Templates, where);
            if (!query.exec(req)) {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                DB.rollback();
                return;
            } else {
                m_TemplatesToDelete.clear();
            }
        }
        query.finish();
        DB.commit();
    }

    void sortItems(TreeItem *root = 0)
    {
        if (!root)
            root = m_RootItem;
        int n = root->childCount();
        root->sortChildren();
        for(int i = 0; i < n; ++i) {
            sortItems(root->child(i));
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

    QModelIndex findIndex(int id, bool findTemplate = true, const QModelIndex &start = QModelIndex())
    {
//        qWarning() << "findIndex from" << start.data().toString() << "searching id" << id;
        QModelIndex idx = start;
        for(int i = 0; i < q->rowCount(start); ++i) {
            idx = q->index(i, 0, start);
            const TreeItem *t = getItem(idx);
//            qWarning() << "   testing" << t->label() << t->id() << id;
            if ((t->id() == id) && (t->isTemplate()==findTemplate))
                return idx;
        }
        // test all its children
        for(int i = 0; i < q->rowCount(start); ++i) {
            idx = findIndex(id, findTemplate, q->index(i, 0, start));
            if (idx.isValid())
                return idx;
        }
        return QModelIndex();
    }

    // For debugging
    void warnTree(Internal::TreeItem *root, int indent = 0)
    {
        QString space;
        space.fill(' ',indent);
        qWarning() << space << root->data(Constants::Data_Label);
        for(int i=0; i<root->childCount(); ++i) {
            warnTree(root->child(i), indent + 2);
        }
    }

private:
    Templates::TemplatesModel *q;

public:
    TreeItem *m_RootItem;
    bool m_ShowOnlyCategories;
    bool m_ReadOnly;
    static TreeItem *m_Tree;
    static bool m_ModelDataRetrieved;
    static QSet<TemplatesModelPrivate *> m_Handles;
    static QHash<int, TreeItem *> m_IdToCategory;
    static QVector<int> m_TemplatesToDelete, m_CategoriesToDelete;
};

TreeItem *TemplatesModelPrivate::m_Tree = 0;
bool TemplatesModelPrivate::m_ModelDataRetrieved = false;
QSet<TemplatesModelPrivate *> TemplatesModelPrivate::m_Handles;
QHash<int, TreeItem *> TemplatesModelPrivate::m_IdToCategory;
QVector<int> TemplatesModelPrivate::m_TemplatesToDelete;
QVector<int> TemplatesModelPrivate::m_CategoriesToDelete;

}  // End Internal
}  // End Templates


TemplatesModel::TemplatesModel(QObject *parent) :
        QAbstractItemModel(parent),
        d(new Internal::TemplatesModelPrivate(this))
{
    setObjectName("TemplatesModel");
    d->setupModelData();
    // FIXME: Qt5 porting: CHECK THIS -> setSupportedDragActions(Qt::CopyAction | Qt::MoveAction);
#if QT_VERSION < 0x050000
    setSupportedDragActions(Qt::CopyAction | Qt::MoveAction);
#endif
    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

TemplatesModel::~TemplatesModel()
{
    // TODO: improve this with coreListener
    submit();

    if (d) {
        delete d;
        d = 0;
    }
}

void TemplatesModel::onCoreDatabaseServerChanged()
{
    beginResetModel();
    d->m_ModelDataRetrieved = false;
    d->setupModelData();
    endResetModel();
}

bool TemplatesModel::isDirty() const
{
    // TODO: code here
    return false;
}

bool TemplatesModel::setCurrentUser(const QString &uuid)
{
    Q_UNUSED(uuid);
    // TODO: upadte template model filter
    return true;
}

QModelIndex TemplatesModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

     Internal::TreeItem *parentItem = d->getItem(parent);
     Internal::TreeItem *childItem = 0;
     if (d->m_ShowOnlyCategories) {
         childItem = parentItem->categoryChild(row);
     } else {
         childItem = parentItem->child(row);
     }

     if (childItem) {
         return createIndex(row, column, childItem);
     }
     return QModelIndex();
 }

QModelIndex TemplatesModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     Internal::TreeItem *childItem = d->getItem(index);
     Internal::TreeItem *parentItem = childItem->parent();

     if (parentItem == d->m_RootItem)
         return QModelIndex();

     if (d->m_ShowOnlyCategories) {
         return createIndex(parentItem->categoryChildNumber(), 0, parentItem);
     }
     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

bool TemplatesModel::reparentIndex(const QModelIndex &item, const QModelIndex &parent)
{
    if (d->m_ReadOnly)
        return false;

    if (WarnReparentItem)
        if (!item.isValid())
            qWarning() << "TemplatesModel::reparentIndex item invalid";

    if (!item.isValid())
        return false;

    Internal::TreeItem *treeItem = d->getItem(item);
    Internal::TreeItem *treeItemParent = d->getItem(item.parent());
    Internal::TreeItem *treeParent = d->getItem(parent);
    bool isTemplate = treeItem->isTemplate();
    int id = treeItem->id();

    if (WarnReparentItem)
        qWarning() << "\n reparentIndex" << treeItem->label() << treeItem->id()
        << "to" << treeParent->label() << treeParent->id() << (treeItemParent == treeParent);

    if (treeItemParent == treeParent)
        return false;
    if (treeItem == treeParent)
        return false;

    int row = rowCount(parent);
    // add the item
    insertRow(row, parent);
    for(int i=0; i<Constants::Data_Max_Param; ++i) {
        setData(index(row, i, parent), index(item.row(), i, item.parent()).data());
    }
    setData(index(row, Constants::Data_ParentId, parent), treeParent->id());
    QPersistentModelIndex newParent = index(row, 0, parent);

    // append its children
    row = 0;
    while (hasIndex(0, 0, item)) {
        if (WarnReparentItem)
            qWarning() << "reparentIndex row" << row << index(0, 0, item).data().toString()
            << "to" << index(0, 0, parent).data().toString() << "or" << newParent.data().toString();
        reparentIndex(index(0, 0, item), newParent);
        ++row;
    }

    // remove row from the model
    removeRow(item.row(), item.parent());
    // but not from the database
    if (isTemplate) {
        d->m_TemplatesToDelete.remove(d->m_TemplatesToDelete.indexOf(id));
    } else {
        d->m_CategoriesToDelete.remove(d->m_CategoriesToDelete.indexOf(id));
    }

    if (WarnReparentItem)
        qWarning() << "End reparent\n";
    return true;
}

int TemplatesModel::rowCount(const QModelIndex &parent) const
{
    Internal::TreeItem *item = d->getItem(parent);
    if (item) {
        if (d->m_ShowOnlyCategories) {
            return item->childCategoryCount();
        } else {
            return item->childCount();
        }
    }
    return 0;
}

int TemplatesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Constants::Data_Max_Param;
}

bool TemplatesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (d->m_ReadOnly)
        return false;

    if (!index.isValid())
        return false;

    Internal::TreeItem *it = d->getItem(index);
    if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        it->setData(index.column(), value);
        // emit from all instances
        d->allInstancesEmitDataChangedFrom(index);
    }
    return true;
}

QVariant TemplatesModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    const Internal::TreeItem *it = d->getItem(item);

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
        {
            return it->data(item.column());
        }
    case Qt::ToolTipRole :
        {
            return it->data(Constants::Data_Summary);
        }
    case Qt::ForegroundRole :
        {
            if (it->isTemplate()) {
                return QColor(settings()->value(Constants::S_FOREGROUND_TEMPLATES, "#000").toString());
            } else {
                return QColor(settings()->value(Constants::S_FOREGROUND_CATEGORIES, "darkblue").toString());
            }
        }
    case Qt::BackgroundRole :
        {
            QColor c;
            if (it->isTemplate()) {
                c = QColor(settings()->value(Constants::S_BACKGROUND_TEMPLATES, "white").toString());
            } else {
                c = QColor(settings()->value(Constants::S_BACKGROUND_CATEGORIES, "white").toString());
            }
            if (c.name()=="#ffffff")
                return QVariant();
            c.setAlpha(125);
            return c;
        }
    }
    return QVariant();
}

Qt::ItemFlags TemplatesModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

Qt::DropActions TemplatesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool TemplatesModel::insertTemplate(const Templates::ITemplate *t)
{
    // TODO: wrong code
    if (d->m_ReadOnly)
        return false;

    // find parent
    Internal::TreeItem *parent = d->m_IdToCategory.value(t->parentId(), 0);
    if (!parent)
        return false;
    // insertRow in parentIndex
//    QModelIndex parentIndex = d->findIndex(parent->id());
    // setData of newly created row
//    Internal::TreeItem *parentItem = d->getItem(item)->parent();
//    // TODO: manage user
//    Internal::TreeItem *newItem = new Internal::TreeItem(t.data(),parentItem);
//    // TODO: reset all instances of the model
//    d->resetAllInstances();
    return true;
}

bool TemplatesModel::insertRows(int row, int count, const QModelIndex &parent)
{
//    qWarning() << "insertRows" << row << count << parent.data();
    if (d->m_ReadOnly)
        return false;

    Internal::TreeItem *parentItem = 0;
    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = d->getItem(parent);
//    if (!parent.isValid())
//        return false;
//    Internal::TreeItem *parentItem = d->getItem(parent);
    QHash<int, QVariant> hashData;
    hashData.insert(Constants::Data_Label, tr("New"));
    hashData.insert(Constants::Data_ParentId, parentItem->data(Constants::Data_Id));
    hashData.insert(Constants::Data_CreationDate, QDateTime::currentDateTime());
    hashData.insert(Constants::Data_IsTemplate, false);
    d->allInstancesBeginInsertRows(parent, row, row+count-1);
    for(int i=0; i<count; ++i) {
        Internal::TreeItem *item = new Internal::TreeItem(hashData, parentItem);
        if (!parentItem->insertChild(row+i, item))
            return false;
    }
    d->allInstancesEndInsertRows();
    return true;
}

bool TemplatesModel::removeRows(int row, int count, const QModelIndex &parent)
{
//    qWarning() << "removeRows" << row << count;
    if (d->m_ReadOnly)
        return false;

    Internal::TreeItem *parentItem = 0;
    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = d->getItem(parent);

    d->allInstancesBeginRemoveRows(parent, row, row+count-1);

    for(int i=0; i<count; ++i) {
        Internal::TreeItem *item = parentItem->child(row+i);
        int id = item->id();
        if (item->isTemplate() && (!d->m_TemplatesToDelete.contains(id)))
            d->m_TemplatesToDelete.append(id);
        else if ((!item->isTemplate())  && (!d->m_CategoriesToDelete.contains(id)))
            d->m_CategoriesToDelete.append(id);
        parentItem->removeChild(item);
        delete item;
        item = 0;
    }

    d->allInstancesEndRemoveRows();

//    qWarning() << "removeRows" << d->m_CategoriesToDelete << d->m_TemplatesToDelete;
    return true;
}

QStringList TemplatesModel::mimeTypes() const
{
    return QStringList() <<  Constants::MIMETYPE_TEMPLATE;
}

QMimeData *TemplatesModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QString tmp, cat;
    QModelIndexList fullList;
    foreach (const QModelIndex &index, indexes) {
        Internal::TreeItem *it = d->getItem(index);
        if (it->isTemplate())
            tmp += QString::number(it->id()) + " ";
        else
            cat += QString::number(it->id()) + " ";
    }
    tmp.chop(1);
    cat.chop(1);
    if (!tmp.isEmpty()) {
        tmp.prepend("T(");
        tmp.append(")");
    }
    if (!cat.isEmpty()) {
        cat.prepend("C(");
        cat.append(")");
    }
    tmp += cat;
    mimeData->setData(mimeTypes().at(0), tmp.toUtf8());
    if (WarnDragAndDrop)
        qWarning() << "TemplatesModel creating mimeData" << tmp;
    return mimeData;
}

bool TemplatesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column)
    if (d->m_ReadOnly)
        return false;

    if (WarnDragAndDrop)
        qWarning() << "TemplatesModel dropMimeData row" << row << "Action" << action << "parent" << parent.data().toString();

    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat(mimeTypes().at(0)))
        return false;

    QModelIndex parentIndex = parent;
    while (isTemplate(parentIndex)) {
        parentIndex = parentIndex.parent();
    }
//    int beginRow = 0;
//    if (row != -1)
//        beginRow = row;

    QList<QPersistentModelIndex> list = getIndexesFromMimeData(data);
//    QList<QPersistentModelIndex> temp, cat;
//    foreach(const QPersistentModelIndex &idx, list) {
//        if (isTemplate(idx))
//            temp << idx;
//        else
//            cat << idx;
//    }

    if  (action == Qt::MoveAction) {
        foreach(const QPersistentModelIndex &idx, list) {
//            int id = d->getItem(idx)->id();
//            bool isTemplate = d->getItem(idx)->isTemplate();
            // TODO: removes templates children from the temp list
            if (!reparentIndex(idx, parentIndex))
                return false;
        }
    } else if (action == Qt::CopyAction) {
        int parentId = d->getItem(parent)->id();
        foreach(const QPersistentModelIndex &idx, list) {
            int row = rowCount(parent);
            insertRow(row, parent);
            Internal::TreeItem *item = d->getItem(idx);
            Internal::TreeItem *newItem = d->getItem(index(row,0,parent));
            int id = newItem->id();
            newItem->replaceData(item->data());
            newItem->setData(Constants::Data_ParentId, parentId);
            newItem->setIsTemplate(item->isTemplate());
            newItem->setId(id);
        }
    }

//    d->saveModelData();

    return true;
}

QModelIndex TemplatesModel::getTemplateId(const int id)
{
    return d->findIndex(id, true);;
}

QList<QPersistentModelIndex> TemplatesModel::getIndexesFromMimeData(const QMimeData *mime)
{
    if (WarnDragAndDrop)
        qWarning() << "TemplatesModel::getIndexesFromMimeData" << mime->data(mimeTypes().at(0));

    Q_ASSERT(mime);
    QList<QPersistentModelIndex> list;
    if (!mime)
        return list;
    // mimeData looks like "T(1,2)C(3)" T for templates C for categories
    QRegExp rx("(\\d+)+");
    QString s = mime->data(mimeTypes().at(0));
    int catBegin = s.indexOf("C(");
    s = s.mid(catBegin);
    int pos = catBegin;

    // Manage categories
    if (WarnDragAndDrop)
        qWarning() << "Categories" << s;
    while ((pos = rx.indexIn(s, pos)) != -1) {
        list << QPersistentModelIndex(d->findIndex(rx.cap(1).toInt(), false));
        pos += rx.matchedLength();
    }

    // Manage templates
    pos = 0;
    s = mime->data(mimeTypes().at(0));
    if (s.contains("T(")) {
        catBegin = s.indexOf(")")+1;
        s = s.mid(0, catBegin);
        while ((pos = rx.indexIn(s, pos)) != -1) {
            list << QPersistentModelIndex(d->findIndex(rx.cap(1).toInt(), true));
            pos += rx.matchedLength();
        }
    }
    if (WarnDragAndDrop)
        qWarning() << "Templates" << s;

//    foreach(QPersistentModelIndex id, list) {
//        qWarning() << id.data().toString();
//    }

    return list;
}

bool TemplatesModel::isTemplate(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    const Internal::TreeItem *it = d->getItem(index);
    return it->isTemplate();
}

void TemplatesModel::categoriesOnly()
{
    if (!d->m_ShowOnlyCategories) {
        beginResetModel();
        d->m_ShowOnlyCategories = true;
        endResetModel();
    }
}

bool TemplatesModel::isCategoryOnly() const
{
    return d->m_ShowOnlyCategories;
}

void TemplatesModel::setReadOnly(const bool state)
{
    d->m_ReadOnly = state;
}

const ITemplate *TemplatesModel::getTemplate(const QModelIndex &item) const
{
    return d->getItem(item);
}


bool TemplatesModel::submit()
{
    if (d->m_ReadOnly)
        return false;

    d->saveModelData();
    d->deleteRowsInDatabase();
    return true;
}

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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Templates::TemplatesModel
  Manages the templates database in/out and link to views.
  Datas are statically stored so that you can instanciate how many requiered models
  as you want without consuming RAM.
*/

#include "templatesmodel.h"
#include "templatebase.h"
#include "itemplates.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icorelistener.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/serializer.h>
#include <utils/global.h>

#include <QList>
#include <QColor>
#include <QSqlTableModel>
#include <QDir>
#include <QMimeData>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include <QSqlResult>
#include <QDebug>

/**
  \todo Add a UUID to templates and categories
  \todo Add user filter, MimeType filter
  \todo Don't get the contents of templates to memory
  \todo Remove the static datas. instead use a new member getDatas() to be called after setCategoryOnly(bool)
  \todo Detect corrupted templates database --> ask user what to do
  \todo Add a coreListener --> save templates database before closing application
  \todo Manage transmission date
*/


using namespace Templates;
using namespace Trans::ConstantTranslations;

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

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Templates::TemplateBase *templateBase() {return Templates::TemplateBase::instance();}

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
    Templates::TemplatesModel *m_Model;
};


class TreeItem : public Templates::ITemplate
{
public:
    TreeItem(const QHash<int, QVariant> &datas, TreeItem *parent = 0) :
            ITemplate(datas),
            m_Parent(parent),
            m_IsTemplate(false),
            m_IsModified(false)
    {
        setData(Constants::Data_UserUuid, "FreeDiams");
        setHasTemplate(datas.value(Constants::Data_IsTemplate).toBool());
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
    void setHasTemplate(bool isTemplate) {m_IsTemplate = isTemplate; setData(Constants::Data_IsTemplate, isTemplate); }
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
        q->setObjectName("TemplatesModel");
        if (!m_ModelDatasRetreived) {
//            QSqlDatabase DB;
//            DB = QSqlDatabase::addDatabase("QSQLITE" , Constants::DB_TEMPLATES_NAME);
//            if (!databasePath().exists()) {
//                if (!QDir().mkpath(databasePath().absolutePath())) {
//                    tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(databasePath().absolutePath());
//                }
//            }
//            DB.setDatabaseName(QDir::cleanPath(databasePath().absolutePath() + QDir::separator() + Constants::DB_TEMPLATES_FILENAME));
//            if (!DB.open()) {
//                Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
//                                     .arg(Constants::DB_TEMPLATES_NAME)
//                                     .arg(DB.lastError().text()));
//            }
//            // Test if database already created or need to be created
//            if (DB.tables(QSql::Tables).count() == 0) {
//                createDatabase();
//            } else if (DB.tables(QSql::Tables).count() != 3) {
//                /** \todo Corrupted templates database --> ask user what to do */
//            }
            QHash<int, QVariant> datas;
            datas.insert(Constants::Data_Label, "ROOT");
            datas.insert(Constants::Data_ParentId, -1);

            if (!m_Tree) {
                m_Tree = new TreeItem(datas,0);
                m_Tree->setHasTemplate(false);
            }
        }
        m_RootItem = m_Tree;
        m_Handles.insert(this);
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
            m_ModelDatasRetreived = false;
        }
    }

    void allInstancesReset() const
    {
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            pr->q->reset();
        }
    }

    void allInstancesEmitDataChangedFrom(const QModelIndex &item)
    {
        /** \todo improve this */
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
        /** \todo improve this */
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                pr->q->beginInsertRows(parent, first, last);
                }
            }
    }

    void allInstancesEndInsertRows()
    {
        /** \todo improve this */
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                pr->q->endInsertRows();
                }
            }
    }

    void allInstancesBeginRemoveRows(const QModelIndex &parent, int first, int last)
    {
        /** \todo improve this */
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                pr->q->beginRemoveRows(parent,first,last);
                }
            }
    }

    void allInstancesEndRemoveRows()
    {
        /** \todo improve this */
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            if (pr->q->isCategoryOnly() == q->isCategoryOnly()) {
                pr->q->endRemoveRows();
                }
            }
    }

//    QDir databasePath() const
//    {
//        return QDir(settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + Constants::DB_TEMPLATES_NAME);
//    }

//    bool createDatabase() const
//    {
//        Utils::Log::addMessage(q, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
//                               .arg(Constants::DB_TEMPLATES_NAME).arg(databasePath().absolutePath()));
//        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
//        if (!DB.open()) {
//            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
//                                 .arg(Constants::DB_TEMPLATES_NAME)
//                                 .arg(DB.lastError().text()));
//            return false;
//        }
//        QStringList req;
//        req <<  "CREATE TABLE IF NOT EXISTS `TEMPLATES` ("
//                "`TEMPLATE_ID`              INTEGER        PRIMARY KEY AUTOINCREMENT,"
//                "`TEMPLATE_UUID`            varchar(40)    NULL,"
//                "`USER_UUID`                int(11)        NULL,"
//                "`ID_CATEGORY`              int(11)        DEFAULT -1,"
//                "`LABEL`                    varchar(300)   NULL,"
//                "`SUMMARY`                  varchar(500)   NULL,"
//                "`CONTENT`                  blob           NULL,"
//                "`CONTENT_MIMETYPES`        varchar(300)   NULL,"
//                "`DATE_CREATION`            date           NULL,"
//                "`DATE_MODIFICATION`        date           NULL,"
//                "`THEMED_ICON_FILENAME`     varchar(50)    NULL,"
//                "`TRANSMISSION_DATE`        date           NULL"
//                ");";
//        req <<  "CREATE TABLE IF NOT EXISTS `CATEGORIES` ("
//                "`CATEGORY_ID`              INTEGER        PRIMARY KEY AUTOINCREMENT,"
//                "`CATEGORY_UUID`            varchar(40)    NULL,"
//                "`USER_UUID`                int(11)        NULL,"
//                "`PARENT_CATEGORY`          int(11)        DEFAULT -1,"
//                "`LABEL`                    varchar(300)   NULL,"
//                "`SUMMARY`                  varchar(500)   NULL,"
//                "`MIMETYPES`                varchar(300)   NULL,"
//                "`DATE_CREATION`            date           NULL,"
//                "`DATE_MODIFICATION`        date           NULL,"
//                "`THEMED_ICON_FILENAME`     varchar(50)    NULL,"
//                "`TRANSMISSION_DATE`        date           NULL"
//                ");";
//        req <<  "CREATE TABLE IF NOT EXISTS `VERSION` ("
//                "`ACTUAL`                  varchar(10)"
//                ");";
//        req <<  QString("INSERT INTO `VERSION` (`ACTUAL`) VALUES('%1');").arg(Constants::DB_ACTUAL_VERSION);
//        req <<  "CREATE TRIGGER delete_all_category_children AFTER "
//                "DELETE ON `CATEGORIES` "
//                "FOR EACH ROW "
//                "  BEGIN"
//                "    DELETE FROM `CATEGORIES` WHERE `CATEGORIES`.`PARENT_CATEGORY`=old.`CATEGORY_ID`;"
//                "    DELETE FROM `TEMPLATES` WHERE `TEMPLATES`.`ID_CATEGORY`=old.`CATEGORY_ID`;"
//                "  END;";
//        bool toReturn = true;
//        foreach(const QString &r, req) {
//            QSqlQuery query(r,DB);
//            if (query.isActive()) {
//                query.next();
//            } else {
//                Utils::Log::addQueryError(q,query);
//                toReturn = false;
//            }
//        }
//        return toReturn;
//    }

    void setupModelData()
    {
        if (m_ModelDatasRetreived)
            return;

        Utils::Log::addMessage(q, "Getting Templates Categories");
        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
        if (!DB.open()) {
            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(Constants::DB_TEMPLATES_NAME)
                                 .arg(DB.lastError().text()));
            return;
        }

        // get categories
        m_IdToCategory.clear();
//        QString req = "SELECT `CATEGORY_ID`, `CATEGORY_UUID`, `USER_UUID`, "
//                      "`PARENT_CATEGORY`, `LABEL`, `SUMMARY`,`DATE_CREATION`, `DATE_MODIFICATION` "
//                      "FROM `CATEGORIES`";
        /** \todo Filter user's templates */
        QSqlQuery query(templateBase()->select(Templates::Constants::Table_Categories), DB);
        if (query.isActive()) {
            QHash<int, QVariant> datas;
            while (query.next()) {
                datas.insert(Constants::Data_Id, query.value(Constants::CATEGORIES_ID));
                datas.insert(Constants::Data_Uuid, query.value(Constants::CATEGORIES_UUID));
                datas.insert(Constants::Data_UserUuid, query.value(Constants::CATEGORIES_USER_UID));
                datas.insert(Constants::Data_ParentId, query.value(Constants::CATEGORIES_PARENT_ID));
                datas.insert(Constants::Data_Label, query.value(Constants::CATEGORIES_LABEL));
                datas.insert(Constants::Data_Summary, query.value(Constants::CATEGORIES_SUMMARY));
                datas.insert(Constants::Data_CreationDate, query.value(Constants::CATEGORIES_DATECREATION));
                datas.insert(Constants::Data_ModifDate, query.value(Constants::CATEGORIES_DATEMODIF));
                m_IdToCategory.insert(datas.value(Constants::Data_Id).toInt(), new TreeItem(datas,0));
                datas.clear();
            }
        } else {
            Utils::Log::addQueryError(q, query);
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
        Utils::Log::addMessage(q, "Getting Templates");
        QList<TreeItem *> templates;
//        req = "SELECT `TEMPLATE_ID`, `TEMPLATE_UUID`, `USER_UUID`, `ID_CATEGORY`, `LABEL`, "
//                      "`SUMMARY`, `CONTENT`, `CONTENT_MIMETYPES`, `DATE_CREATION`, `DATE_MODIFICATION`, "
//                      "`THEMED_ICON_FILENAME`  FROM `TEMPLATES`";
        /** \todo filter user's uuid */
        query.exec(templateBase()->select(Templates::Constants::Table_Templates));
        if (query.isActive()) {
            QHash<int, QVariant> datas;
            while (query.next()) {
                datas.insert(Constants::Data_Id, query.value(Constants::TEMPLATE_ID));
                datas.insert(Constants::Data_Uuid, query.value(Constants::TEMPLATE_UUID));
                datas.insert(Constants::Data_UserUuid, query.value(Constants::TEMPLATE_USER_UID));
                datas.insert(Constants::Data_ParentId, query.value(Constants::TEMPLATE_ID_CATEGORY));
                datas.insert(Constants::Data_Label, query.value(Constants::TEMPLATE_LABEL));
                datas.insert(Constants::Data_Summary, query.value(Constants::TEMPLATE_SUMMARY));
                datas.insert(Constants::Data_Content, query.value(Constants::TEMPLATE_CONTENT));
                datas.insert(Constants::Data_ContentMimeTypes, query.value(Constants::TEMPLATE_CONTENTMIMETYPES));
                datas.insert(Constants::Data_CreationDate, query.value(Constants::TEMPLATE_DATECREATION));
                datas.insert(Constants::Data_ModifDate, query.value(Constants::TEMPLATE_DATEMODIF));
                datas.insert(Constants::Data_ThemedIcon, query.value(Constants::TEMPLATE_THEMEDICON));
                TreeItem *it = new TreeItem(datas,0);
                it->setHasTemplate(true);
                templates.insert(datas.value(Constants::Data_Id).toInt(), it);
                datas.clear();
            }
        } else {
            Utils::Log::addQueryError(q, query);
        }
        query.finish();
        // add templates to categories
        foreach(TreeItem *item, templates) {
            // need to be reparented
            item->setParent(m_IdToCategory.value(item->data(Constants::Data_ParentId).toInt(),m_RootItem));
            // add item to the children of its parent
            item->parent()->addChildren(item);
            item->setModified(false);
        }
        sortItems();
        m_ModelDatasRetreived = true;
    }

    void saveModelDatas(const QModelIndex &start = QModelIndex())
    {
        if (WarnDatabaseSaving)
            qWarning() << "saveModelDatas :" << start.data().toString();

        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
        if (!DB.open()) {
            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(Constants::DB_TEMPLATES_NAME)
                                 .arg(DB.lastError().text()));
            return;
        }

        QModelIndex idx = start;
        for(int i = 0; i< q->rowCount(start); ++i) {
            idx = q->index(i, 0, start);
            TreeItem *t = getItem(idx);
            if (WarnDatabaseSaving) {
                qWarning() << "   saving" << t->label().leftJustified(50,' ') << "user" << t->ownerUuid().leftJustified(50,' ') << "parent" << t->parent()->label().leftJustified(50,' ');
                qWarning() << "   newly" << t->isNewlyCreated() << "modified" << t->isModified();
            }
            QSqlQuery query(DB);
            QString req;
            if (t->isNewlyCreated()) {
                if (t->isTemplate()) {
//                    query.prepare("INSERT INTO `TEMPLATES` ("
//                                  "`TEMPLATE_UUID`,"
//                                  "`USER_UUID`,"
//                                  "`ID_CATEGORY`,"
//                                  "`LABEL`,"
//                                  "`SUMMARY`,"
//                                  "`CONTENT`,"
//                                  "`CONTENT_MIMETYPES`,"
//                                  "`DATE_CREATION`,"
//                                  "`DATE_MODIFICATION`,"
//                                  "`THEMED_ICON_FILENAME`,"
//                                  "`TRANSMISSION_DATE`"
//                                  ") "
//                                  "VALUES (?,?,?,?,?,?,?,?,?,?,?);");
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
                    query.exec();
                    if (!query.isActive()) {
                        Utils::Log::addQueryError(q, query);
                    } else {
                        t->setNewlyCreated(false);
                        t->setModified(false);
                        allInstancesEmitDataChangedFrom(idx);
                    }
                    // retreive its id
                    t->setId(query.lastInsertId().toInt());
                } else {
//                    query.prepare("INSERT INTO `CATEGORIES` ("
//                                  "`CATEGORY_UUID`,"
//                                  "`USER_UUID`,"
//                                  "`PARENT_CATEGORY`,"
//                                  "`LABEL`,"
//                                  "`SUMMARY`,"
//                                  "`DATE_CREATION`,"
//                                  "`DATE_MODIFICATION`,"
//                                  "`THEMED_ICON_FILENAME`,"
//                                  "`TRANSMISSION_DATE`"
//                                  ") "
//                                  "VALUES (?,?,?,?,?,?,?,?,?);");
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
                    query.exec();
                    if (!query.isActive()) {
                        Utils::Log::addQueryError(q, query);
                    } else {
                        t->setNewlyCreated(false);
                        t->setModified(false);
                        allInstancesEmitDataChangedFrom(idx);
                    }
                    // retreive its id
                    t->setId(query.lastInsertId().toInt());
//                    qWarning() << "created" << t->label() << t->id();
                    // inform children of the id
                    for(int i=0; i<t->childCount(); ++i) {
                        t->child(i)->setParentId(t->id());
                    }
                }
            } else if (t->isModified()) {
                if (t->isTemplate()) {
//                    req = QString("UPDATE `TEMPLATES` SET "
//                          "`TEMPLATE_UUID`= '%1' ,"
//                          "`USER_UUID`= '%2',"
//                          "`ID_CATEGORY`= %3 ,"
//                          "`LABEL`= '%4' ,"
//                          "`CONTENT_MIMETYPES`= '%5',"
//                          "`DATE_CREATION`= '%6',"
//                          "`DATE_MODIFICATION`= '%7',"
//                          "`THEMED_ICON_FILENAME`= '%8',"
//                          "`TRANSMISSION_DATE`= '%9' "
//                          "WHERE (`TEMPLATE_ID`= %10 )")
//                            .arg(t->uuid())
//                            .arg(t->ownerUuid())
//                            .arg(t->parentId())
//                            .arg(t->label())
//                            .arg(t->contentMimeTypes().join(";"))
//                            .arg(t->data(Constants::Data_CreationDate).toDate().toString(Qt::ISODate))
//                            .arg(t->data(Constants::Data_ModifDate).toDate().toString(Qt::ISODate))
//                            .arg(t->data(Constants::Data_ThemedIcon).toString())
//                            .arg(t->data(Constants::Data_TransmissionDate).toDate().toString(Qt::ISODate))
//                            .arg(t->id());
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
                    query.exec();
                    if (!query.isActive()) {
                        Utils::Log::addQueryError(q, query);
                    }
                    req.clear();
                    query.finish();
//                    if (t->dirtyRows().contains(Constants::Data_Summary)) {
//                        query.prepare(QString("UPDATE `TEMPLATES` SET `SUMMARY` = ? WHERE `TEMPLATE_ID`=%1").arg(t->id()));
//                        query.bindValue(0, t->summary());
//                        query.exec();
//                        if (!query.isActive()) {
//                            Utils::Log::addQueryError(q, query);
//                        }
//                        query.finish();
//                    }
//                    if (t->dirtyRows().contains(Constants::Data_Content)) {
//                        query.prepare(QString("UPDATE `TEMPLATES` SET `CONTENT` = ? WHERE `TEMPLATE_ID`=%1").arg(t->id()));
//                        query.bindValue(0, t->content());
//                        query.exec();
//                        if (!query.isActive()) {
//                            Utils::Log::addQueryError(q, query);
//                        }
//                    }
                    t->setModified(false);
                    allInstancesEmitDataChangedFrom(idx);
                } else {
//                    req = QString("UPDATE `CATEGORIES` SET "
//                          "`CATEGORY_UUID`= '%1' ,"
//                          "`USER_UUID`= '%2',"
//                          "`PARENT_CATEGORY`= %3 ,"
//                          "`LABEL`= '%4' ,"
//                          "`DATE_CREATION`= '%5',"
//                          "`DATE_MODIFICATION`= '%6',"
//                          "`THEMED_ICON_FILENAME`= '%7',"
//                          "`TRANSMISSION_DATE`= '%8' "
//                          "WHERE (`CATEGORY_ID`= %9 )")
//                            .arg(t->uuid())
//                            .arg(t->ownerUuid())
//                            .arg(t->parentId())
//                            .arg(t->label())
//                            .arg(t->data(Constants::Data_CreationDate).toDate().toString(Qt::ISODate))
//                            .arg(t->data(Constants::Data_ModifDate).toDate().toString(Qt::ISODate))
//                            .arg(t->data(Constants::Data_ThemedIcon).toString())
//                            .arg(t->data(Constants::Data_TransmissionDate).toDate().toString(Qt::ISODate))
//                            .arg(t->id());
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
                    query.exec();
                    if (!query.isActive()) {
                        Utils::Log::addQueryError(q, query);
                    }
                    req.clear();
                    query.finish();
//                    if (t->dirtyRows().contains(Constants::Data_Summary)) {
//                        query.prepare(QString("UPDATE `CATEGORIES` SET `SUMMARY` = ? WHERE `CATEGORY_ID`=%1").arg(t->id()));
//                        query.bindValue(0, t->summary());
//                        query.exec();
//                        if (!query.isActive()) {
//                            Utils::Log::addQueryError(q, query);
//                        }
//                    }
                    t->setModified(false);
                    allInstancesEmitDataChangedFrom(idx);
                }
            }
        }
        // save all its children
        for(int i = 0; i < q->rowCount(start); ++i) {
            saveModelDatas(q->index(i, 0, start));
        }
    }

    QVector<int> getCategoryChildren(const int idCategory)
    {
        QVector<int> toReturn;
        QString req;
        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
        if (!DB.open()) {
            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(Constants::DB_TEMPLATES_NAME)
                                 .arg(DB.lastError().text()));
            return toReturn;
        }
//        QString("SELECT `CATEGORY_ID` FROM `CATEGORIES`  WHERE `PARENT_CATEGORY`=%1").arg(idCategory);
        QHash<int, QString> where;
        where.insert(Constants::CATEGORIES_PARENT_ID, QString("=%1").arg(idCategory));
        req = templateBase()->select(Constants::Table_Categories, Constants::CATEGORIES_ID, where);
        QSqlQuery query(req, DB);
        if (!query.isActive()) {
            Utils::Log::addQueryError(q, query);
        } else {
            while (query.next()) {
                toReturn << query.value(0).toInt();
                toReturn << getCategoryChildren(query.value(0).toInt());
            }
        }
        query.finish();
        return toReturn;
    }

    void deleteRowsInDatabase()
    {
        if (m_CategoriesToDelete.isEmpty() && m_TemplatesToDelete.isEmpty())
            return;
//        qWarning() << "deleteRows" << m_CategoriesToDelete << m_TemplatesToDelete;
        QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
        if (!DB.open()) {
            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(Constants::DB_TEMPLATES_NAME)
                                 .arg(DB.lastError().text()));
            return;
        }
        QString req;

        if (m_CategoriesToDelete.count()) {
            req.clear();
            // retreive all its children categories from db
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

            QSqlQuery query(req, DB);
            if (!query.isActive())
                Utils::Log::addQueryError(q, query);
            else
                m_CategoriesToDelete.clear();
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
            QSqlQuery query(req, DB);
            if (!query.isActive())
                Utils::Log::addQueryError(q, query);
            else
                m_TemplatesToDelete.clear();
        }
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
    static bool m_ModelDatasRetreived;
    static QSet<TemplatesModelPrivate *> m_Handles;
    static QHash<int, TreeItem *> m_IdToCategory;
    static QVector<int> m_TemplatesToDelete, m_CategoriesToDelete;
};

TreeItem *TemplatesModelPrivate::m_Tree = 0;
bool TemplatesModelPrivate::m_ModelDatasRetreived = false;
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
    setSupportedDragActions(Qt::CopyAction | Qt::MoveAction);
}

TemplatesModel::~TemplatesModel()
{
    /** \todo improve this with coreListener */
    submit();

    if (d) {
        delete d;
        d = 0;
    }
}

bool TemplatesModel::isDirty() const
{
    /** \todo code here  */
    return false;
}

bool TemplatesModel::setCurrentUser(const QString &uuid)
{
    return true;
}

QModelIndex TemplatesModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

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
        qWarning() << "End reparent \n";
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
            if (Utils::isDebugCompilation()) {
                if (it->isNewlyCreated()) {
                    c = QColor(Qt::blue);
                } else
                    if (it->isModified()) {
                    c = QColor(Qt::red);
                }
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
    if (d->m_ReadOnly)
        return false;

    // find parent
    Internal::TreeItem *parent = d->m_IdToCategory.value(t->parentId(), 0);
    if (!parent)
        return false;
    // insertRow in parentIndex
    QModelIndex parentIndex = d->findIndex(parent->id());
    // setDatas of newly created row
//    Internal::TreeItem *parentItem = d->getItem(item)->parent();
//    /** \todo manage user */
//    Internal::TreeItem *newItem = new Internal::TreeItem(t.datas(),parentItem);
//    /** \todo reset all instances of the model */
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
    QHash<int, QVariant> datas;
    datas.insert(Constants::Data_Label, tr("New"));
    datas.insert(Constants::Data_ParentId, parentItem->data(Constants::Data_Id));
    datas.insert(Constants::Data_CreationDate, QDateTime::currentDateTime());
    datas.insert(Constants::Data_IsTemplate, false);
    d->allInstancesBeginInsertRows(parent, row, row+count-1);
    for(int i=0; i<count; ++i) {
        Internal::TreeItem *item = new Internal::TreeItem(datas, parentItem);
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
    int beginRow = 0;
    if (row != -1)
        beginRow = row;

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
            /** \todo removes templates children from the temp list */
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
            newItem->setDatas(item->datas());
            newItem->setData(Constants::Data_ParentId, parentId);
            newItem->setHasTemplate(item->isTemplate());
            newItem->setId(id);
        }
    }

//    d->saveModelDatas();

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
        d->m_ShowOnlyCategories = true;
        reset();
    }
}

bool TemplatesModel::isCategoryOnly() const
{
    return d->m_ShowOnlyCategories;
}

void TemplatesModel::setReadOnly(const bool state)
{
    d->m_ReadOnly = true;
}
const ITemplate *TemplatesModel::getTemplate(const QModelIndex &item) const
{
    return d->getItem(item);
}


bool TemplatesModel::submit()
{
    if (d->m_ReadOnly)
        return false;

    d->saveModelDatas();
    d->deleteRowsInDatabase();
    return true;
}

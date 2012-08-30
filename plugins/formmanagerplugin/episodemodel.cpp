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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
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
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <extensionsystem/pluginmanager.h>

#include <QSqlTableModel>

enum { base64MimeDatas = true  };

#ifdef DEBUG
enum {
    WarnDragAndDrop = false,
    WarnReparentItem = false,
    WarnDatabaseSaving = false,
    WarnLogChronos = false
   };
#else
enum {
    WarnDragAndDrop = false,
    WarnReparentItem = false,
    WarnDatabaseSaving = false,
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
//        qWarning() << Q_FUNC_INFO;
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
        _rootForm(0),
        _readOnly(false),
        _sqlModel(0),
        q(parent)
    {
    }

    ~EpisodeModelPrivate()
    {
    }

    void updateFilter()
    {
        QString filter;
        // Filter valid episodes
        QHash<int, QString> where;
        where.insert(Constants::EPISODES_ISVALID, "=1");
        where.insert(Constants::EPISODES_PATIENT_UID, QString("='%1'").arg(patient()->uuid()));
        _sqlModel->setFilter(episodeBase()->getWhereClause(Constants::Table_EPISODES, where).remove("WHERE"));
        _sqlModel->select();
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
        FormMain *form = _rootForm->formMainChild(formUid);
        if (!form)
            return QString::null;
//        bool formIsModified = false;

        QHash<QString, FormItem *> items;
        foreach(FormItem *it, form->flattenFormItemChildren()) {
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

//    bool saveEpisode(EpisodeModelTreeItem *item, const QString &formUid)
//    {
//        if (!item)
//            return true;
//        if (formUid.isEmpty()) {
//            LOG_ERROR_FOR("EpisodeModel", "No formUid");
//            return false;
//        }

//        EpisodeData *episode = m_EpisodeItems.key(item);
//        FormMain *form = 0;
//        foreach(FormMain *f, m_FormItems.keys()) {
//            if (f->uuid()==formUid) {
//                form = f;
//                break;
//            }
//        }

//        if (episode && form) {
//            episode->setData(EpisodeData::XmlContent, createXmlEpisode(formUid));
//            episode->setData(EpisodeData::IsXmlContentPopulated, true);
//            episode->setData(EpisodeData::Label, form->itemData()->data(IFormItemData::ID_EpisodeLabel));
//            episode->setData(EpisodeData::UserDate, form->itemData()->data(IFormItemData::ID_EpisodeDate));
//            LOG_FOR("EpisodeModel", "Save episode: " + episode->data(EpisodeData::Label).toString());
//            if (!settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, true).toBool()) {
//                bool yes = Utils::yesNoMessageBox(QCoreApplication::translate("EpisodeModel", "Save episode?"),
//                                                  QCoreApplication::translate("EpisodeModel", "The actual episode has been modified. Do you want to save changes in your database?\n"
//                                                     "Answering 'No' will cause definitve data lose."),
//                                                  "", QCoreApplication::translate("EpisodeModel", "Save episode"));
//                if (!yes) {
//                    return false;
//                }
//            }

//            // inform the patient model
//            foreach(FormItem *it, form->flattenFormItemChildren()) {
//                if (!it->itemData())
//                    continue;
////                qWarning() << "Feeding patientModel data with" << it->patientDataRepresentation() << it->itemDatas()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole);
//                patient()->setValue(it->patientDataRepresentation(), it->itemData()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole));
//            }

//            // save episode to database
//            return episodeBase()->saveEpisode(episode);
//        }
//        return false;
//    }

//    void feedFormWithEpisodeContent(Form::FormMain *form, EpisodeModelTreeItem *item, bool feedPatientModel = false)
//    {
//        EpisodeData *episode = m_EpisodeItems.key(item);
//        feedFormWithEpisodeContent(form, episode, feedPatientModel);
//    }

//    void feedFormWithEpisodeContent(Form::FormMain *form, EpisodeData *episode, bool feedPatientModel = false)
//    {
//        QTime chrono;
//        if (WarnLogChronos)
//            chrono.start();

//        getEpisodeContent(episode);
//        const QString &xml = episode->data(EpisodeData::XmlContent).toString();
//        if (xml.isEmpty()) {
//            return;
//        }

//        // read the xml'd content
//        QHash<QString, QString> datas;
//        if (!Utils::readXml(xml, Form::Constants::XML_FORM_GENERAL_TAG, datas, false)) {
//            LOG_ERROR_FOR(q, QString("Error while reading EpisodeContent"));
//            return;
//        }

//        // put datas into the FormItems of the form
//        // XML content ==
//        // <formitemuid>value</formitemuid>
//        QHash<QString, FormItem *> items;
//        foreach(FormItem *it, form->flattenFormItemChildren()) {
//            items.insert(it->uuid(), it);
//        }

////        qWarning() << Q_FUNC_INFO << feedPatientModel << form->uuid();
//        form->clear();
//        form->itemData()->setData(IFormItemData::ID_EpisodeDate, episode->data(EpisodeData::UserDate));
//        form->itemData()->setData(IFormItemData::ID_EpisodeLabel, episode->data(EpisodeData::Label));
//        const QString &username = user()->fullNameOfUser(episode->data(EpisodeData::UserCreatorUuid)); //value(Core::IUser::FullName).toString();
//        if (username.isEmpty())
//            form->itemData()->setData(IFormItemData::ID_UserName, "No user");
//        else
//            form->itemData()->setData(IFormItemData::ID_UserName, username);

//        // feed the formitemdatas for this form and get the data for the patientmodel
//        foreach(FormItem *it, items.values()) {
//            if (!it) {
//                qWarning() << "FormManager::activateForm :: ERROR: no item: " << items.key(it);
//                continue;
//            }
//            if (!it->itemData())
//                continue;

//            it->itemData()->setStorableData(datas.value(it->uuid()));
//            if (feedPatientModel) {
////                qWarning() << "Feeding patientModel data with" << it->patientDataRepresentation() << it->itemDatas()->data(it->patientDataRepresentation(), IFormItemData::ID_ForPatientModel);
//                if (it->patientDataRepresentation() >= 0) {
//                    if (WarnLogChronos)
//                        Utils::Log::logTimeElapsed(chrono, q->objectName(), "feedFormWithEpisodeContent: feed patient model: " + it->uuid());
//                    patient()->setValue(it->patientDataRepresentation(), it->itemData()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole));
//                }
//            }
//        }
//        if (WarnLogChronos)
//            Utils::Log::logTimeElapsed(chrono, q->objectName(), "feedFormWithEpisodeContent");

//    }

//    void getLastEpisodes(bool andFeedPatientModel = true)
//    {
//        qWarning() << "GetLastEpisode (feedPatientModel=" <<andFeedPatientModel << ")";
//        if (patient()->uuid().isEmpty())
//            return;

//        QTime chrono;
//        if (WarnLogChronos)
//            chrono.start();

//        foreach(Form::FormMain *form, m_FormItems.keys()) {
////            if (andFeedPatientModel) {
////                bool hasPatientDatas = false;
////                // test all children FormItem for patientDataRepresentation
////                foreach(Form::FormItem *item, form->flattenFormItemChildren()) {
////                    if (item->itemDatas()) {
////                        if (item->patientDataRepresentation()!=-1) {
////                            hasPatientDatas = true;
////                            break;
////                        }
////                    }
////                }
////                if (!hasPatientDatas)
////                    continue;
////            }

//            // get the form's XML content for the last episode, feed it with the XML code
//            EpisodeModelTreeItem *formItem = m_FormItems.value(form);
//            if (!formItem->childCount()) {
//                continue;  // No episodes
//            }

//            // find last episode of the form
//            EpisodeData *lastOne = 0;
//            for(int i=0; i < m_Episodes.count(); ++i) {
//                EpisodeData *episode = m_Episodes.at(i);
//                if (episode->data(EpisodeData::FormUuid).toString()==form->uuid()) {
//                    if (!lastOne) {
//                        lastOne = episode;
//                        continue;
//                    }
//                    if (lastOne->data(EpisodeData::UserDate).toDateTime() < episode->data(EpisodeData::UserDate).toDateTime()) {
//                        lastOne = episode;
//                    }
//                }
//            }

//            // feed episode and activate it
//            if (lastOne) {
//                feedFormWithEpisodeContent(form, lastOne, andFeedPatientModel);
//            }
//        }
//        if (WarnLogChronos)
//            Utils::Log::logTimeElapsed(chrono, q->objectName(), "getLastEpisodes");
//    }

public:
    FormMain *_rootForm;
    bool _readOnly;
    EpisodeModelCoreListener *m_CoreListener;
    EpisodeModelPatientListener *m_PatientListener;
    QSqlTableModel *_sqlModel;

private:
    EpisodeModel *q;
};
}
}

EpisodeModel::EpisodeModel(FormMain *rootEmptyForm, QObject *parent) :
        QAbstractListModel(parent),
        d(new Internal::EpisodeModelPrivate(this))
{
    Q_ASSERT(rootEmptyForm);
    setObjectName("Form::EpisodeModel");
    d->_rootForm = rootEmptyForm;

    // Autosave feature
    // -> Core Listener
    d->m_CoreListener = new Internal::EpisodeModelCoreListener(this);
    pluginManager()->addObject(d->m_CoreListener);

    // -> User Listener

    // -> Patient change listener
    d->m_PatientListener = new Internal::EpisodeModelPatientListener(this);
    pluginManager()->addObject(d->m_PatientListener);

    // Create the SQL Model
    onCoreDatabaseServerChanged();
}

bool EpisodeModel::initialize()
{

    // TODO: connect save -> move to FormManager
//    // connect the save action
//    Core::Command * cmd = actionManager()->command(Core::Constants::A_FILE_SAVE);
//    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(submit()));

    onUserChanged();
    onPatientChanged();

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    connect(user(), SIGNAL(userChanged()), this, SLOT(onUserChanged()));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onPatientChanged()));
    return true;
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

QString EpisodeModel::formUid() const
{
    return d->_rootForm->uuid();
}

void EpisodeModel::onCoreDatabaseServerChanged()
{
    // Destroy old model and recreate it
    if (d->_sqlModel) {
        disconnect(d->_sqlModel);
        delete d->_sqlModel;
    }
    // Create the SQL Model
    d->_sqlModel = new QSqlTableModel(this, episodeBase()->database());
    d->_sqlModel->setTable(episodeBase()->table(Constants::Table_EPISODES));
    Utils::linkSignalsFromFirstModelToSecondModel(d->_sqlModel, this, false);
    d->updateFilter();
}

void EpisodeModel::onUserChanged()
{
}

void EpisodeModel::onPatientChanged()
{
    d->updateFilter();
    d->_sqlModel->select();
}

int EpisodeModel::rowCount(const QModelIndex &parent) const
{
    return d->_sqlModel->rowCount(parent);
}

int EpisodeModel::columnCount(const QModelIndex &) const
{
    return MaxData;
}

QVariant EpisodeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() == EmptyColumn1)
        return QVariant();
    if (index.column() == EmptyColumn2)
        return QVariant();

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
    {
        int sqlColumn;
        switch (index.column()) {
        case Label: sqlColumn = Constants::EPISODES_LABEL; break;
        case IsValid:  sqlColumn = Constants::EPISODES_ISVALID; break;
        case CreationDate:  sqlColumn = Constants::EPISODES_DATEOFCREATION; break;
        case UserDate:  sqlColumn = Constants::EPISODES_USERDATE; break;
        case UserCreatorName:
        {
            QString userUid = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_USERCREATOR)).toString();
            if (userUid == user()->uuid())
                return tkTr(Trans::Constants::YOU);
            return user()->fullNameOfUser(userUid);
        }
//        case Summary:  sqlColumn = Constants::EPISODES_ISVALID; break;
//        case XmlContent:  sqlColumn = Constants::EPISODES_ISVALID; break;
        case Icon:  sqlColumn = Constants::EPISODES_ISVALID; break;
        }
        if (sqlColumn!=-1) {
            QModelIndex sqlIndex = d->_sqlModel->index(index.row(), sqlColumn);
            return d->_sqlModel->data(sqlIndex, role);
        }
        break;
    }
    case Qt::ToolTipRole :
    {
        // TODO: create a cache?
        QDate createDate = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_DATEOFCREATION)).toDate();
        QDate userDate = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_USERDATE)).toDate();
        QString cDate = QLocale().toString(createDate, settings()->value(Constants::S_EPISODEMODEL_SHORTDATEFORMAT, tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL)).toString());
        QString uDate = QLocale().toString(userDate, settings()->value(Constants::S_EPISODEMODEL_SHORTDATEFORMAT, tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL)).toString());
        QString label = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_LABEL)).toString();
        QString userUid = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_USERCREATOR)).toString();
        return QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br /><span style=\"color:gray;font-size:9pt\">%3</span></p>")
                .arg(uDate.replace(" ", "&nbsp;"))
                .arg(label.replace(" ", "&nbsp;"))
                .arg(user()->fullNameOfUser(userUid) + "<br/>" +
                     tr("Created: ") +  cDate);
    }
//    case Qt::ForegroundRole :
//    {
//        if (episode) {
//            return QColor(settings()->value(Constants::S_EPISODEMODEL_EPISODE_FOREGROUND, "darkblue").toString());
//        } else if (form) {
//            if (it->parent() == d->m_RootItem) {
//                if (settings()->value(Constants::S_USESPECIFICCOLORFORROOTS).toBool()) {
//                    return QColor(settings()->value(Constants::S_FOREGROUNDCOLORFORROOTS, "darkblue").toString());
//                }
//            }
//            return QColor(settings()->value(Constants::S_EPISODEMODEL_FORM_FOREGROUND, "#000").toString());
//        }
//        break;
//    }
//    case Qt::FontRole :
//    {
//        if (form) {
//            QFont f;
//            f.fromString(settings()->value(Constants::S_EPISODEMODEL_FORM_FONT).toString());
//            return f;
//        } else {
//            QFont f;
//            f.fromString(settings()->value(Constants::S_EPISODEMODEL_EPISODE_FONT).toString());
//            return f;
//        }
//        return QFont();
//    }
//    case Qt::DecorationRole :
//    {
//        if (form) {
//            QString icon = form->spec()->value(FormItemSpec::Spec_IconFileName).toString();
//            icon.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::SmallPixmapPath));
//            if (QFileInfo(icon).isRelative())
//                icon.append(qApp->applicationDirPath());
//            return QIcon(icon);
//        }
//        break;
//    }
    }
    return QVariant();
}

bool EpisodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (d->_readOnly)
        return false;

    if (!index.isValid())
        return false;

    if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        int sqlColumn = -1;
        switch (index.column()) {
        case Label: sqlColumn = Constants::EPISODES_LABEL; break;
        case IsValid:  sqlColumn = Constants::EPISODES_ISVALID; break;
        case CreationDate:  sqlColumn = Constants::EPISODES_DATEOFCREATION; break;
        case UserDate:  sqlColumn = Constants::EPISODES_USERDATE; break;
            //            case Summary:  sqlColumn = Constants::EPISODES_; break;
            //        case XmlContent:  sqlColumn = Constants::EPISODES_ISVALID; break;
        case Icon:  sqlColumn = Constants::EPISODES_ISVALID; break;
        }
        if (sqlColumn!=-1) {
            QModelIndex sqlIndex = d->_sqlModel->index(index.row(), sqlColumn);
            bool ok = d->_sqlModel->setData(sqlIndex, value, role);
            if (ok)
                Q_EMIT dataChanged(index, index);
            return ok;
        }
    }
    return false;
}

Qt::ItemFlags EpisodeModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void EpisodeModel::fetchMore(const QModelIndex &parent)
{
    d->_sqlModel->fetchMore(parent);
}

bool EpisodeModel::canFetchMore(const QModelIndex &parent) const
{
    return d->_sqlModel->canFetchMore(parent);
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
    if (d->_readOnly)
        return false;

    return d->_sqlModel->insertRows(row, count, parent);

//    const QString &formUid = d->_rootForm->uuid();

//    beginInsertRows(parent, row, row + count);
//    for(int i = 0; i < count; ++i) {
//        // create the episode
//        Internal::EpisodeData *episode = new Internal::EpisodeData;
//        episode->setData(Internal::EpisodeData::Label, tr("New episode"));
//        episode->setData(Internal::EpisodeData::FormUuid, formUid);
//        episode->setData(Internal::EpisodeData::UserCreatorUuid, user()->uuid());
//        episode->setData(Internal::EpisodeData::PatientUuid, patient()->uuid());
//        episode->setData(Internal::EpisodeData::CreationDate, QDateTime::currentDateTime());
//        episode->setData(Internal::EpisodeData::UserDate, QDateTime::currentDateTime());
//        episode->setData(Internal::EpisodeData::IsValid, true);
//        // TODO: code here : create an episode modification to store the user creator?

//        // create the tree item
//        EpisodeModelTreeItem *it = new EpisodeModelTreeItem(parentItem);
//        parentItem->insertChild(row+i, it);

//        // link episode/item
//        d->m_EpisodeItems.insert(episode, it);
//        d->m_Episodes.append(episode);
//    }
//    endInsertRows();
//    return true;
}

/** Not implemented */
bool EpisodeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);

    if (d->_readOnly)
        return false;

    // TODO: manage episode removal (change the validity of the episode)
    return true;
//    return d->_sqlModel->removeRows(row, count, parent);
}

/** Define the whole model read mode */
void EpisodeModel::setReadOnly(const bool state)
{
    d->_readOnly = state;
}

/** Return true if the whole model is in a read only mode */
bool EpisodeModel::isReadOnly() const
{
    return d->_readOnly;
}

/** Return true if the model has unsaved data */
bool EpisodeModel::isDirty() const
{
    return false;
//    return d->_sqlModel->isDirty();
}

/** Save the whole model. \sa isDirty() */
bool EpisodeModel::submit()
{
    // No active patient ?
    if (patient()->uuid().isEmpty())
        return false;

    return d->_sqlModel->submit();
    // save actual episode if needed
//    if (d->m_ActualEpisode) {
//        if (!d->saveEpisode(d->m_ActualEpisode, d->m_ActualEpisode_FormUid)) {
//            LOG_ERROR("Unable to save actual episode before editing a new one");
//        }
//    }
//    return true;
}

bool EpisodeModel::activateEpisode(const QModelIndex &index, const QString &formUid) //, const QString &xmlcontent)
{
//    qWarning() << "activateEpisode" << formUid;
//    // submit actual episode
//    if (!d->saveEpisode(d->m_ActualEpisode, d->m_ActualEpisode_FormUid)) {
//        LOG_ERROR("Unable to save actual episode before editing a new one");
//    }

//    d->m_RecomputeLastEpisodeSynthesis = true;

//    if (!index.isValid()) {
//        d->m_ActualEpisode = 0;
//        return false;
//    }

//    // stores the actual episode id
//    EpisodeModelTreeItem *it = d->getItem(index);
//    if (it==d->m_RootItem)
//        return false;

//    EpisodeData *episode = d->m_EpisodeItems.key(it, 0);
//    FormMain *form = formForIndex(index);
//    if (!episode) {
//        d->m_ActualEpisode = 0;
//        return false;
//    }
//    d->m_ActualEpisode = it;
//    d->m_ActualEpisode_FormUid = formUid;

//    // TODO: this form code should not appear in a model's method according to strict MVC pattern.
//    //    Extract it to the form parts and maybe link with Signal/Slots?
//    // clear actual form and fill episode data
//    if (!form)
//        return false;
//    form->clear();
//    form->itemData()->setData(IFormItemData::ID_EpisodeDate, episode->data(EpisodeData::UserDate));
//    form->itemData()->setData(IFormItemData::ID_EpisodeLabel, episode->data(EpisodeData::Label));
//    const QString &username = user()->fullNameOfUser(episode->data(EpisodeData::UserCreatorUuid)); //value(Core::IUser::FullName).toString();
//    if (username.isEmpty())
//        form->itemData()->setData(IFormItemData::ID_UserName, tr("No user"));
//    else
//        form->itemData()->setData(IFormItemData::ID_UserName, username);

//    // TODO: move this part into a specific member of the private part
//    d->getEpisodeContent(episode);
//    const QString &xml = episode->data(EpisodeData::XmlContent).toString();
//    if (xml.isEmpty())
//        return true;

//    // read the xml'd content
//    QHash<QString, QString> datas;
//    if (!Utils::readXml(xml, Form::Constants::XML_FORM_GENERAL_TAG, datas, false)) {
//        LOG_ERROR(QString("Error while reading EpisodeContent %2:%1").arg(__LINE__).arg(__FILE__));
//        return false;
//    }

//    // put datas into the FormItems of the form
//    // XML content ==
//    // <formitemuid>value</formitemuid>
//    QHash<QString, FormItem *> items;
//    foreach(FormItem *it, form->flattenFormItemChildren()) {
//        items.insert(it->uuid(), it);
//        // Add fieldEquivalence
////        foreach(const QString &uuid, it->equivalentUuid()) {
////            items.insert(uuid, it);
////        }
//    }

////    qWarning() << "ITEMS" << items;

//    foreach(const QString &s, datas.keys()) {
//        FormItem *it = items.value(s, 0);
//        if (!it) {
//            qWarning() << "FormManager::activateForm :: ERROR: no item: " << s;
//            continue;
//        }
//        if (it->itemData())
//            it->itemData()->setStorableData(datas.value(s));
//        else
//            qWarning() << "FormManager::activateForm :: ERROR: no itemData: " << s;
//    }
    return true;
}

/** Save the episode pointed by the \e index to the database. */
bool EpisodeModel::saveEpisode(const QModelIndex &index, const QString &formUid)
{
    return true;
//    return d->saveEpisode(d->getItem(index), formUid);
}

/** Return the HTML formatted synthesis of all the last recorded episodes for each forms in the model. */
QString EpisodeModel::lastEpisodesSynthesis() const
{
//    QTime chrono;
//    if (WarnLogChronos)
//        chrono.start();

//    if (d->m_RecomputeLastEpisodeSynthesis) {
//        // submit actual episode
//        if (!d->saveEpisode(d->m_ActualEpisode, d->m_ActualEpisode_FormUid)) {
//            LOG_ERROR("Unable to save actual episode before editing a new one");
//        }
//        d->m_ActualEpisode = 0;
//        d->m_ActualEpisode_FormUid.clear();

//        d->getLastEpisodes(false);
//    }
//    if (WarnLogChronos)
//        Utils::Log::logTimeElapsed(chrono, objectName(), "Compute last episode Part 1");

    QString html;
//    foreach(FormMain *f, d->_rootForm->firstLevelFormMainChildren()) {
//        if (!f) {
//            LOG_ERROR("??");
//            continue;
//        }
//        html += f->printableHtml();
//    }

//    if (WarnLogChronos)
//        Utils::Log::logTimeElapsed(chrono, objectName(), "Compute last episode Part 2 (getting html code)");

    return html;
}


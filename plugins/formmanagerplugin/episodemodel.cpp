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
/**
 * \class Form::EpisodeModel
 * Create a model to episode database. Episode model are linked to a specific form.
 * Form::FormManager centralizes all the Form::EpisodeModel for each
 * forms.
 * \sa Form::FormManager::episodeModel()
 */

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
#include <formmanagerplugin/iformwidgetfactory.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <extensionsystem/pluginmanager.h>

#include <QSqlTableModel>
#include <QSqlRecord>

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
//#include <formmanagerplugin/formcore.h>
//static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

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
        _formMain(0),
        _readOnly(false),
        _useCache(true),
        _sqlModel(0),
        q(parent)
    {
    }

    ~EpisodeModelPrivate()
    {
        qDeleteAll(_validationCache.values());
        _validationCache.clear();
    }

    void clearCache()
    {
        _xmlContentCache.clear();
    }

    void updateFilter(const QString &patientUid)
    {
        // Filter valid episodes
        QHash<int, QString> where;
        where.insert(Constants::EPISODES_ISVALID, "=1");
        where.insert(Constants::EPISODES_PATIENT_UID, QString("='%1'").arg(patientUid));
        where.insert(Constants::EPISODES_FORM_PAGE_UID, QString("='%1'").arg(_formMain->uuid()));
        _sqlModel->setFilter(episodeBase()->getWhereClause(Constants::Table_EPISODES, where).remove("WHERE"));
        _sqlModel->setSort(Constants::EPISODES_USERDATE, Qt::AscendingOrder);
        _sqlModel->select();
    }

    bool isEpisodeContentInCache(const QModelIndex &index)
    {
        if (!_useCache)
            return false;
        QModelIndex id = _sqlModel->index(index.row(), Constants::EPISODES_ID);
        return (_xmlContentCache.keys().contains(_sqlModel->data(id).toInt()));
    }

    QString getEpisodeContentFormCache(const QModelIndex &index)
    {
        QModelIndex id = _sqlModel->index(index.row(), Constants::EPISODES_ID);
        return _xmlContentCache.value(_sqlModel->data(id).toInt());
    }

    void storeEpisodeContentInCache(const QModelIndex &index, const QString &xml)
    {
        QModelIndex id = _sqlModel->index(index.row(), Constants::EPISODES_ID);
        _xmlContentCache.insert(_sqlModel->data(id).toInt(), xml);
    }

    QString getEpisodeContent(const QModelIndex &index)
    {
        if (isEpisodeContentInCache(index))
            return getEpisodeContentFormCache(index);
        QModelIndex id = _sqlModel->index(index.row(), Constants::EPISODES_ID);
        QString xml = episodeBase()->getEpisodeContent(_sqlModel->data(id));
        storeEpisodeContentInCache(id, xml);
        return xml;
    }

    bool isEpisodeValidated(const QModelIndex &index)
    {
        QVariant uid = _sqlModel->data(_sqlModel->index(index.row(), Constants::EPISODES_ID)).toString();
        QList<EpisodeValidationData *>val;
        // in cache ?
        if (_validationCache.keys().contains(uid.toInt())) {
            val = _validationCache.values(uid.toInt());
        } else {
            // get from db
            val = episodeBase()->getEpisodeValidations(uid);
            int id = uid.toInt();
            if (val.isEmpty()) {
                _validationCache.insertMulti(id, 0);
                val.append(0);
            } else {
                foreach(EpisodeValidationData *data, val)
                    _validationCache.insertMulti(id, data);
            }
        }
        // episode validated ?
        return (!(val.count()==1 && val.at(0)==0));
    }

    void checkModelContent()
    {
        if (_formMain->episodePossibilities() == Form::FormMain::UniqueEpisode) {
            if (_sqlModel->rowCount() < 1) {
                q->insertRow(0);
            }
        } else if (_formMain->episodePossibilities() == Form::FormMain::NoEpisode) {
            if (_sqlModel->rowCount() > 0)
                LOG_ERROR_FOR(q, QString("NoEpisode Form (%1) with episodes?").arg(_formMain->uuid()));
        }
    }

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
////                bool hasPatientData = false;
////                // test all children FormItem for patientDataRepresentation
////                foreach(Form::FormItem *item, form->flattenFormItemChildren()) {
////                    if (item->itemData()) {
////                        if (item->patientDataRepresentation()!=-1) {
////                            hasPatientData = true;
////                            break;
////                        }
////                    }
////                }
////                if (!hasPatientData)
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
    FormMain *_formMain;
    bool _readOnly, _useCache;
    EpisodeModelCoreListener *m_CoreListener;
    EpisodeModelPatientListener *m_PatientListener;
    QSqlTableModel *_sqlModel;
    QHash<int, QString> _xmlContentCache;
    QMultiHash<int, EpisodeValidationData *> _validationCache;

private:
    EpisodeModel *q;
};
}  // namespace Internal
}  // namespace Form

/** Create a specific episode model for the specified Form::FormMain \e rootEmptyForm */
EpisodeModel::EpisodeModel(FormMain *rootEmptyForm, QObject *parent) :
        QAbstractListModel(parent),
        d(new Internal::EpisodeModelPrivate(this))
{
    Q_ASSERT(rootEmptyForm);
    setObjectName("Form::EpisodeModel");
    d->_formMain = rootEmptyForm;

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

/** Initialize the model */
bool EpisodeModel::initialize()
{
    onUserChanged();
    onCurrentPatientChanged();

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    connect(user(), SIGNAL(userChanged()), this, SLOT(onUserChanged()));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
    return true;
}

/**
 * Define the patient uuid to use for the filter. By default, the Form::EpisodeModel always filter the
 * current patient episodes. In many cases, you do not need to change this behavior.
 */
void EpisodeModel::setCurrentPatient(const QString &uuid)
{
    d->updateFilter(uuid);
}

void EpisodeModel::setUseFormContentCache(bool useCache)
{
    d->_useCache = useCache;
}

/** Return the current form unique identifier linked to this model */
QString EpisodeModel::formUid() const
{
    return d->_formMain->uuid();
}

/** Update the model if the Core sever changed. */
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
    connect(d->_sqlModel, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(populateNewRowWithDefault(int, QSqlRecord&)));
    d->updateFilter(patient()->uuid());
}

/** Reacts on user changes. */
void EpisodeModel::onUserChanged()
{
}

/** Reacts on patient changes. */
void EpisodeModel::onCurrentPatientChanged()
{
    d->clearCache();
    d->updateFilter(patient()->uuid());
    d->_sqlModel->select();
    d->checkModelContent();
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
        case UserDate:  sqlColumn = Constants::EPISODES_USERDATE; break;
        case Label: sqlColumn = Constants::EPISODES_LABEL; break;
        case IsValid:  sqlColumn = Constants::EPISODES_ISVALID; break;
        case CreationDate:  sqlColumn = Constants::EPISODES_DATEOFCREATION; break;
        case UserCreatorName:
        {
            QString userUid = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_USERCREATOR)).toString();
            if (userUid == user()->uuid())
                return tkTr(Trans::Constants::YOU);
            return user()->fullNameOfUser(userUid);
        }
//        case Summary:  sqlColumn = Constants::EPISODES_ISVALID; break;
        case XmlContent:  return d->getEpisodeContent(index); break;
        case Icon: sqlColumn = Constants::EPISODES_ISVALID; break;
        case Uuid: sqlColumn = Constants::EPISODES_ID; break;
        case FormUuid: if (d->_formMain) return d->_formMain->uuid(); else return QString();
        case FormLabel: if (d->_formMain) return d->_formMain->spec()->label();
        }  // switch (column)

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
        if (userUid == user()->uuid())
            userUid = tkTr(Trans::Constants::YOU);
        else
            userUid = user()->fullNameOfUser(userUid);

        return QString("<p align=\"right\">%1&nbsp;-&nbsp;%2<br />"
                       "<span style=\"color:gray;font-size:9pt\">%3<br />%4</span></p>")
                .arg(uDate.replace(" ", "&nbsp;"))
                .arg(label.replace(" ", "&nbsp;"))
                .arg(tkTr(Trans::Constants::CREATED_BY_1).arg(userUid))
                .arg(tkTr(Trans::Constants::ON_THE_1).arg(cDate));
    }
    case Qt::DecorationRole :
    {
        switch (index.column()) {
        case ValidationStateIcon:
        {
            // Scale down the icons to 12x12 or 10x10
            if (d->isEpisodeValidated(index))
                return theme()->icon(Core::Constants::ICONLOCK_BLACKWHITE).pixmap(12,12);
            return theme()->icon(Core::Constants::ICONUNLOCK_BLACKWHITE).pixmap(12,12);
        }
        case FormLabel:
        {
            if (!d->_formMain)
                return QVariant();
            QString iconFile = d->_formMain->spec()->iconFileName();
            iconFile.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::SmallPixmapPath));
            return QIcon(iconFile);
        }
        }  // switch (index.column())
    }
    case Qt::SizeHintRole :
    {
        switch (index.column()) {
        case ValidationStateIcon: return QSize(22, 22);
        }
    }
    }  // switch (role)
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
        case IsValid: sqlColumn = Constants::EPISODES_ISVALID; break;
        case CreationDate: sqlColumn = Constants::EPISODES_DATEOFCREATION; break;
        case UserDate: sqlColumn = Constants::EPISODES_USERDATE; break;
            //            case Summary:  sqlColumn = Constants::EPISODES_; break;
        case XmlContent:
        {
            QModelIndex id = d->_sqlModel->index(index.row(), Constants::EPISODES_ID);
            QVariant episodeId = d->_sqlModel->data(id);
            // update internal cache
            d->_xmlContentCache.insert(episodeId.toInt(), value.toString());
            // send to database
            return episodeBase()->saveEpisodeContent(episodeId, value.toString());
        }
        case Icon: sqlColumn = Constants::EPISODES_ISVALID; break;
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

QVariant EpisodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role!=Qt::DisplayRole)
        return d->_sqlModel->headerData(section, orientation, role);
    if (orientation!=Qt::Horizontal)
        return d->_sqlModel->headerData(section, orientation, role);

    switch (section) {
    case UserDate: return tkTr(Trans::Constants::DATE);
    case Label: return tkTr(Trans::Constants::LABEL);
    case Uuid: return tkTr(Trans::Constants::UNIQUE_IDENTIFIER);
    case IsValid: return tkTr(Trans::Constants::ISVALID);
    case CreationDate: return tkTr(Trans::Constants::CREATION_DATE);
    case UserCreatorName: return tkTr(Trans::Constants::AUTHOR);
    case XmlContent: return tr("Xml content");
    case Icon: return tkTr(Trans::Constants::ICON);
    case EmptyColumn1: return QString();
    case EmptyColumn2: return QString();
    default: break;
    }
    return QVariant();
}

/** Insert new episodes to the \e parent. The parent must be a form. */
bool EpisodeModel::insertRows(int row, int count, const QModelIndex &)
{
    if (d->_readOnly)
        return false;
    bool ok = d->_sqlModel->insertRows(row, count);
    if (!ok) {
        LOG_ERROR("Unable to insert rows: " + d->_sqlModel->lastError().text());
        return false;
    }
    d->_sqlModel->submitAll();
    return true;
}

void EpisodeModel::populateNewRowWithDefault(int row, QSqlRecord &record)
{
    Q_UNUSED(row);
    for(int i = 1; i < record.count(); ++i) {
        record.setNull(i);
        record.setGenerated(i, true);
    }
    record.setValue(Constants::EPISODES_LABEL, tr("New episode"));
    record.setValue(Constants::EPISODES_FORM_PAGE_UID, d->_formMain->uuid());
    record.setValue(Constants::EPISODES_USERCREATOR, user()->uuid());
    record.setValue(Constants::EPISODES_USERDATE, QDateTime::currentDateTime());
    record.setValue(Constants::EPISODES_PATIENT_UID, patient()->uuid());
    record.setValue(Constants::EPISODES_DATEOFCREATION, QDateTime::currentDateTime());
    record.setValue(Constants::EPISODES_ISVALID, 1);
}

/** Invalidate an episode. The episode will stay in database but will not be show in the view (unless you ask for invalid episode not to be filtered). */
bool EpisodeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (d->_readOnly)
        return false;

    beginRemoveRows(parent, row, row+count-1);
    d->_sqlModel->blockSignals(true);
    for(int i = row; i < count; ++i) {
        d->_sqlModel->setData(d->_sqlModel->index(i, Constants::EPISODES_ISVALID), 0);
    }
    d->_sqlModel->blockSignals(false);
    endRemoveRows();
    return true;
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

/** Return true if the model has unsaved data (NOT CODED) */
bool EpisodeModel::isDirty() const
{
    // TODO: code here
    return false;
//    return d->_sqlModel->isDirty();
}

/** Validate an episode using the current user and the current date time. */
bool EpisodeModel::validateEpisode(const QModelIndex &index)
{
    if (!index.isValid())
        return false;
    EpisodeValidationData *validation = new EpisodeValidationData;
    QModelIndex idindex = d->_sqlModel->index(index.row(), Constants::EPISODES_ID);
    QVariant id = d->_sqlModel->data(idindex);
    validation->setData(EpisodeValidationData::EpisodeId, id);
    validation->setData(EpisodeValidationData::ValidationDate, QDateTime::currentDateTime());
    validation->setData(EpisodeValidationData::UserUid, user()->uuid());
    validation->setData(EpisodeValidationData::IsValid, 1);
    d->_validationCache.insertMulti(id.toInt(), validation);
    bool ok = episodeBase()->saveEpisodeValidation(validation);
    Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1));
    return ok;
}

/** Return true if the episode \e index is user validated */
bool EpisodeModel::isEpisodeValidated(const QModelIndex &index) const
{
    return d->isEpisodeValidated(index);
}

bool EpisodeModel::populateFormWithEpisodeContent(const QModelIndex &episode, bool feedPatientModel)
{
    QTime chrono;
    if (WarnLogChronos)
        chrono.start();

    d->_formMain->clear();
    d->_formMain->formWidget()->setEnabled(false);

    const QString &xml = d->getEpisodeContent(episode);

//    qWarning() << xml;

    QHash<QString, FormItem *> items;
    QHash<QString, QString> _data;
    if (!xml.isEmpty()) {
        // read the xml'd content
        if (!Utils::readXml(xml, Form::Constants::XML_FORM_GENERAL_TAG, _data, false)) {
            QModelIndex uid = index(episode.row(), EpisodeModel::Uuid);
            LOG_ERROR(QString("Error while reading episode content (%1)").arg(data(uid).toString()));
            return false;
        }

        // put data into the FormItems of the form
        foreach(FormItem *it, d->_formMain->flattenFormItemChildren()) {
            items.insert(it->uuid(), it);
        }
    }

    // Populate the FormMain item data (username, userdate, label)
    QModelIndex userName = index(episode.row(), EpisodeModel::UserCreatorName);
    QModelIndex userDate = index(episode.row(), EpisodeModel::UserDate);
    QModelIndex label = index(episode.row(), EpisodeModel::Label);
    d->_formMain->itemData()->setData(IFormItemData::ID_EpisodeDate, this->data(userDate));
    d->_formMain->itemData()->setData(IFormItemData::ID_EpisodeLabel, this->data(label));
    d->_formMain->itemData()->setData(IFormItemData::ID_UserName, this->data(userName));
    d->_formMain->itemData()->setStorableData(false); // equal: _formMain->itemData()->setModified(false)

    // Populate FormItem data and the patientmodel
    foreach(FormItem *it, items.values()) {
        if (!it) {
            LOG_ERROR("populateFormWithEpisode :: ERROR: no item: " + items.key(it));
            continue;
        }
        if (!it->itemData())
            continue;

        it->itemData()->setStorableData(_data.value(it->uuid(), QString()));
        if (feedPatientModel && it->patientDataRepresentation() >= 0)
                patient()->setValue(it->patientDataRepresentation(), it->itemData()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole));
    }

    d->_formMain->formWidget()->setEnabled(true);

    // TODO: if episode is validated ==> read-only

    if (WarnLogChronos)
        Utils::Log::logTimeElapsed(chrono, objectName(), "populateFormWithEpisode");
    return true;
}

/** Save the whole model. \sa isDirty() */
bool EpisodeModel::submit()
{
    // No active patient ?
    if (patient()->uuid().isEmpty())
        return false;

    bool ok = d->_sqlModel->submit();
    return ok;
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
//    foreach(FormMain *f, d->_formMain->firstLevelFormMainChildren()) {
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


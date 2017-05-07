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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
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
#include "formcore.h"
#include "formmanager.h"

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
#include <translationutils/trans_msgerror.h>

#include <extensionsystem/pluginmanager.h>

#include <QSqlTableModel>
#include <QSqlRecord>

enum { base64MimeData = true  };

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
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

namespace Form {
namespace Internal {

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

    /* Update the SQL filter based on:
     *   - form uid
     *   - patient uid
     * \e patientUid
     */
    void updateFilter(const QString &patientUid)
    {
        if (_currentPatientUuid == patientUid)
            return;
        _currentPatientUuid = patientUid;

        // Filter valid episodes
        QHash<int, QString> where;
        Utils::FieldList uuid;
        uuid << Utils::Field(Constants::Table_EPISODES,
                             Constants::EPISODES_FORM_PAGE_UID,
                             QString("='%1'").arg(_formMain->uuid()));
        if (!_formMain->spec()->equivalentUuid().isEmpty()) {
            foreach(const QString &eq, _formMain->spec()->equivalentUuid())
                uuid << Utils::Field(Constants::Table_EPISODES,
                                     Constants::EPISODES_FORM_PAGE_UID,
                                     QString("='%1'").arg(eq));
        }
        where.insert(Constants::EPISODES_ISVALID, "=1");
        where.insert(Constants::EPISODES_PATIENT_UID, QString("='%1'").arg(patientUid));

        QString filter = episodeBase()->getWhereClause(Constants::Table_EPISODES, where).remove("WHERE") +
                " AND (" + episodeBase()->getWhereClause(uuid, Utils::Database::OR) + ")";
        _sqlModel->setFilter(filter);
        _sqlModel->setSort(Constants::EPISODES_USERDATETIME, Qt::AscendingOrder);
        _sqlModel->select();
    }

    bool isEpisodeContentInCache(const QModelIndex &index)
    {
        if (!_useCache)
            return false;
        QModelIndex id = _sqlModel->index(index.row(), Constants::EPISODES_ID);
        return (_xmlContentCache.keys().contains(_sqlModel->data(id).toInt()));
    }

    QString getEpisodeContentFromCache(const QModelIndex &index)
    {
        if (!_useCache)
            return QString::null;
        QModelIndex id = _sqlModel->index(index.row(), Constants::EPISODES_ID);
        return _xmlContentCache.value(_sqlModel->data(id).toInt());
    }

    void storeEpisodeContentInCache(const QModelIndex &index, const QString &xml)
    {
        if (!_useCache)
            return;
        QModelIndex id = _sqlModel->index(index.row(), Constants::EPISODES_ID);
        _xmlContentCache.insert(_sqlModel->data(id).toInt(), xml);
    }

    QString getEpisodeContent(const QModelIndex &index)
    {
        if (isEpisodeContentInCache(index))
            return getEpisodeContentFromCache(index);
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
        // If (MedForm has Unique option) AND (MedForm has attribute 'IsIdentity = "true"'):
        // automatically create 1 episode
        if ( (_formMain->episodePossibilities() == Form::FormMain::UniqueEpisode)
             && (_formMain->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool()) ) {
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
    ////                foreach(Form::FormItem *item, form->flattenedFormItemChildren()) {
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
    QSqlTableModel *_sqlModel;
    QHash<int, QString> _xmlContentCache;
    QMultiHash<int, EpisodeValidationData *> _validationCache;
    QModelIndexList _dirtyIndexes;
    QString _currentPatientUuid;

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

    // FIXME: cache system sometimes loses data?
    setUseFormContentCache(false);

    // Create the SQL Model
    onCoreDatabaseServerChanged();
}

EpisodeModel::~EpisodeModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

/** Initialize the model */
bool EpisodeModel::initialize()
{
    onUserChanged();
    onPatientFormLoaded();

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()), Qt::UniqueConnection);
    connect(user(), SIGNAL(userChanged()), this, SLOT(onUserChanged()), Qt::UniqueConnection);
    //    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onPatientFormLoaded()), Qt::UniqueConnection);
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

QString EpisodeModel::currentPatientUuid() const
{
    return d->_currentPatientUuid;
}

void EpisodeModel::setUseFormContentCache(bool useCache)
{
    d->_useCache = useCache;
}

/** Return the current form unique identifier linked to this model */
QString EpisodeModel::formUid() const
{
    if (!d->_formMain)
        return QString::null;
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

/** Reacts on patient form loaded emitted by Form::FormManager. */
void EpisodeModel::onPatientFormLoaded()
{
    beginResetModel();
    d->clearCache();
    d->updateFilter(patient()->uuid());
    //    d->_sqlModel->select();
    d->checkModelContent();
    endResetModel();
}

int EpisodeModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid())
        return 0;
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
        int sqlColumn = -1;
        QVariant data = QVariant();
        switch (index.column()) {
        case UserDateTime:
        {
            sqlColumn = Constants::EPISODES_USERDATETIME;
            QModelIndex sqlIndex = d->_sqlModel->index(index.row(), sqlColumn);
            QString uDateTime = d->_sqlModel->data(sqlIndex, role).toString();
            QDateTime uDateTimeLocal = QDateTime::fromString(uDateTime, Qt::ISODate).toLocalTime();
            data = uDateTimeLocal;
            break;
        }
        case Label: sqlColumn = Constants::EPISODES_LABEL; break;
        case IsValid:  sqlColumn = Constants::EPISODES_ISVALID; break;
        case CreationDateTime:
        {
            sqlColumn = Constants::EPISODES_EPISODECREATIONDATETIME;
            QModelIndex sqlIndex = d->_sqlModel->index(index.row(), sqlColumn);
            QString cDateTime = d->_sqlModel->data(sqlIndex, role).toString();
            QDateTime creationDateTimeUtc = QDateTime::fromString(cDateTime, Qt::ISODate);
            QDateTime creationDateTimeLocal = creationDateTimeUtc.toLocalTime();
            data = creationDateTimeLocal;
            break;
        }
        case UserCreatorName:
        {
            QString userUid = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_USERCREATOR)).toString();
            if (userUid == user()->uuid())
                return tkTr(Trans::Constants::YOU);
            return user()->fullNameOfUser(userUid);
        }
        case Priority: sqlColumn = Constants::EPISODES_PRIORITY; break;
        case XmlContent:  return d->getEpisodeContent(index); break;
        case Icon: sqlColumn = Constants::EPISODES_ISVALID; break;
        case Uuid: sqlColumn = Constants::EPISODES_ID; break;
        case FormUuid:
            if (d->_formMain)
                return d->_formMain->uuid();
            break;
        case FormLabel:
            if (d->_formMain)
                return d->_formMain->spec()->label();
            break;
        }  // switch (column)

        if (sqlColumn!=-1 && data.isNull()) {
            QModelIndex sqlIndex = d->_sqlModel->index(index.row(), sqlColumn);
            return d->_sqlModel->data(sqlIndex, role);
        }
        return data;
        break;
    }
    case Qt::ToolTipRole :
    {
        // TODO create a cache?
        // TODO introduce UTC conversion
        QDateTime createDateTime = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_EPISODECREATIONDATETIME)).toDateTime();
        QDateTime userDateTime = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_USERDATETIME)).toDateTime();
        QString cDateTime = QLocale().toString(createDateTime, settings()->value(Constants::S_EPISODEMODEL_LONGDATEFORMAT, tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL)).toString());
        QString uDateTime = QLocale().toString(userDateTime, settings()->value(Constants::S_EPISODEMODEL_LONGDATEFORMAT, tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL)).toString());
        QString label = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_LABEL)).toString();
        QString userUid = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_USERCREATOR)).toString();
        int priority = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_PRIORITY)).toInt();
        QString icon;
        switch (priority) {
        case High: icon = theme()->iconFullPath(Core::Constants::ICONPRIORITY_HIGH); break;
        case Medium: icon = theme()->iconFullPath(Core::Constants::ICONPRIORITY_MEDIUM); break;
        case Low: icon = theme()->iconFullPath(Core::Constants::ICONPRIORITY_LOW); break;
        }
        if (userUid == user()->uuid())
            userUid = tkTr(Trans::Constants::YOU);
        else
            userUid = user()->fullNameOfUser(userUid);

        // TODO: add validation information in the tooltip
        return QString("<p align=\"right\"><img src=\"%5\">%1&nbsp;-&nbsp;%2<br />"
                       "<span style=\"color:gray;font-size:9pt\">%3<br />%4</span></p>")
                .arg(uDateTime.replace(" ", "&nbsp;"))
                .arg(label.replace(" ", "&nbsp;"))
                .arg(tkTr(Trans::Constants::CREATED_BY_1).arg(userUid))
                .arg(tkTr(Trans::Constants::ON_THE_1).arg(cDateTime))
                .arg(icon);
    }
    case Qt::DecorationRole :
    {
        switch (index.column()) {
        case ValidationStateIcon:
        {
            // Scale down the icons to 12x12 or 10x10
            if (d->isEpisodeValidated(index))
                return theme()->icon(Core::Constants::ICONLOCK_BLACKWHITE);//.pixmap(12,12);
            return theme()->icon(Core::Constants::ICONUNLOCK_BLACKWHITE);//.pixmap(12,12);
        }
        case PriorityIcon:
        {
            // Scale down the icons to 12x12 or 10x10
            int priority = d->_sqlModel->data(d->_sqlModel->index(index.row(), Constants::EPISODES_PRIORITY)).toInt();
            switch (priority) {
            case High: return theme()->icon(Core::Constants::ICONPRIORITY_HIGH);//.pixmap(12,12);
            case Medium: return theme()->icon(Core::Constants::ICONPRIORITY_MEDIUM);//.pixmap(12,12);
            case Low: return theme()->icon(Core::Constants::ICONPRIORITY_LOW);//.pixmap(12,12);
            }
            break;
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
        break;
    }
    case Qt::SizeHintRole :
    {
        switch (index.column()) {
        case ValidationStateIcon: return QSize(22, 22);
        case PriorityIcon: return QSize(22, 22);
        }
        break;
    }
        //    case Qt::BackgroundRole :
        //    {
        //        if (d->_formMain->itemData()) {
        //            QColor red("darkred");
        //            if (d->_formMain->itemData()->isModified()) {
        //                red.setAlpha(120);
        //                return red;
        //            }
        //            foreach(Form::FormItem *item, d->_formMain->flattenedFormItemChildren()) {
        //                if (item->itemData() && item->itemData()->isModified()) {
        //                    red.setAlpha(200);
        //                    return red;
        //                }
        //            }
        //        }
        //        break;
        //    }
    }  // switch (role)
    return QVariant();
}

bool EpisodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (d->_readOnly) {
        qWarning() << "Unable to setData, read-only";
        return false;
    }

    if (!index.isValid()) {
        qWarning() << "Unable to setData, invalid index";
        return false;
    }

    if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        Q_EMIT episodeAboutToChange(index);
        d->_dirtyIndexes << index;
        int sqlColumn = -1;
        switch (index.column()) {
        case Label: sqlColumn = Constants::EPISODES_LABEL; break;
        case IsValid: sqlColumn = Constants::EPISODES_ISVALID; break;
        case CreationDateTime: sqlColumn = Constants::EPISODES_EPISODECREATIONDATETIME; break;
        case UserDateTime: sqlColumn = Constants::EPISODES_USERDATETIME; break;
        case Priority: sqlColumn = Constants::EPISODES_PRIORITY; break;
        case XmlContent:
        {
            QModelIndex id = d->_sqlModel->index(index.row(), Constants::EPISODES_ID);
            QVariant episodeId = d->_sqlModel->data(id);
            // update internal cache
            if (d->_useCache)
                d->_xmlContentCache.insert(episodeId.toInt(), value.toString());
            // send to database
            bool ok = episodeBase()->saveEpisodeContent(episodeId, value.toString());
            if (ok) {
                Q_EMIT dataChanged(index, index);
                Q_EMIT episodeChanged(index);
            }
            return ok;
        }
        case Icon: sqlColumn = Constants::EPISODES_ISVALID; break;
        } // switch

        if (sqlColumn != -1) {
            QModelIndex sqlIndex = d->_sqlModel->index(index.row(), sqlColumn);
            bool ok = d->_sqlModel->setData(sqlIndex, value, role); // also emits dataChanged
            if (index.column()==Priority)
                Q_EMIT dataChanged(this->index(index.row(), int(PriorityIcon)), this->index(index.row(), int(PriorityIcon)));
            Q_EMIT episodeChanged(index);
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
    case ValidationStateIcon: return "V";
    case PriorityIcon: return "P";
    case UserDateTime: return tkTr(Trans::Constants::EPISODE_DATE_TIME);
    case Label: return tkTr(Trans::Constants::LABEL);
    case Uuid: return tkTr(Trans::Constants::UNIQUE_IDENTIFIER);
    case IsValid: return tkTr(Trans::Constants::ISVALID);
    case CreationDateTime: return tkTr(Trans::Constants::CREATION_DATE_TIME);
    case UserCreatorName: return tkTr(Trans::Constants::AUTHOR);
    case XmlContent: return tr("Xml content");
    case Icon: return tkTr(Trans::Constants::ICON);
    case Priority: return tkTr(Trans::Constants::PRIORITY);
    case EmptyColumn1: return QString();
    case EmptyColumn2: return QString();
    default: break;
    }
    return QVariant();
}

/** Insert new episodes to the \e parent. The parent must be a form. */
bool EpisodeModel::insertRows(int row, int count, const QModelIndex &)
{
    if (d->_readOnly) {
        LOG_ERROR("Unable to insertRow, model is read-only");
        return false;
    }
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
    record.clearValues();
    for(int i = 0; i < d->_sqlModel->columnCount(); ++i)
        record.setGenerated(i, true);
    // We need to force the EPISODES_ID in the record (we can not let the db chose the ID value)
    record.setValue(Constants::EPISODES_ID, episodeBase()->max(Constants::Table_EPISODES, Constants::EPISODES_ID).toInt() + 1);
    record.setValue(Constants::EPISODES_LABEL, tr("New episode"));
    record.setValue(Constants::EPISODES_FORM_PAGE_UID, d->_formMain->uuid());
    record.setValue(Constants::EPISODES_USERCREATOR, user()->uuid());
    record.setValue(Constants::EPISODES_USERDATETIME, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    record.setValue(Constants::EPISODES_PATIENT_UID, d->_currentPatientUuid);
    record.setValue(Constants::EPISODES_EPISODECREATIONDATETIME,
                    QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    record.setValue(Constants::EPISODES_ISVALID, 1);
    record.setValue(Constants::EPISODES_PRIORITY, Medium);
}

/** Invalidate an episode. The episode will stay in database but will not be
 * displayed in the view (unless you ask for invalid episode not to be filtered).
 */
bool EpisodeModel::removeRows(int row, int count, const QModelIndex &parent)
{
#ifdef WITH_EPISODE_REMOVAL
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
#else
    return false;
#endif
}

/** Remove all the recorded episode in the model/base */
bool EpisodeModel::removeAllEpisodes()
{
    bool ok = episodeBase()->removeAllEpisodeForForm(d->_formMain->uuid(), patient()->uuid());
    if (ok)
        d->updateFilter(patient()->uuid());
    //        d->_sqlModel->reset();
    return ok;
}

/** Define the whole model read mode */
void EpisodeModel::setReadOnly(bool state)
{
    // TODO: manage a read-only property PER ROW and for the whole model
    d->_readOnly = state;
    // update all itemdata of the parent formmain readonly property
    if (d->_formMain->itemData())
        d->_formMain->itemData()->setReadOnly(state);
    foreach(Form::FormItem *item, d->_formMain->flattenedFormItemChildren()) {
        if (item->itemData())
            item->itemData()->setReadOnly(state);
    }
}

/** Return true if the whole model is in a read only mode */
bool EpisodeModel::isReadOnly() const
{
    return d->_readOnly;
}

/** Return true if the model has unsaved data */
bool EpisodeModel::isDirty() const
{
    return d->_dirtyIndexes.count() > 0;
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
    // FIXME: improve the readonly state to per QModelIndex readonly state -> isReadOnly(index)
    setReadOnly(true);
    Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1));
    return ok;
}

/** Return true if the episode \e index is user validated */
bool EpisodeModel::isEpisodeValidated(const QModelIndex &index) const
{
    return d->isEpisodeValidated(index);
}

/** Remove the episode \e index, and return the error state */
bool EpisodeModel::removeEpisode(const QModelIndex &index)
{
#ifdef WITH_EPISODE_REMOVAL
    if (!index.isValid()) {
        qWarning() << "EpisodeModel::removeEpisode: index is not valid";
        return false;
    }
    beginResetModel();

    // Get some information & log
    QModelIndex uidIndex = d->_sqlModel->index(index.row(), Constants::EPISODES_ID);
    const QVariant &uid = d->_sqlModel->data(uidIndex);
    LOG(QString("Starting episode removal: %1").arg(uid.toString()));

    // Remove episode from database
    bool ok = episodeBase()->removeEpisode(uid);
    // TODO: add a trace in the episode db

    // Force SQL model filter update
    d->_currentPatientUuid.clear();
    d->updateFilter(patient()->uuid());

    // Force reset
    endResetModel();

    return ok;
#else
    return false;
#endif
}

/**
 * Renew the selected episode \e episodeToRenew. This member takes the content of
 * the current episode and use it to create a new one. \n
 * The date is set to the current date, the episode label is cleared,
 * and user creator is set to the current user. \n
 * The Read Only state of the model does not affect this member.\n
 * Returns the model index (column 0) of the new episode or an
 * invalid index in case of an error.
 */
QModelIndex EpisodeModel::renewEpisode(const QModelIndex &episodeToRenew)
{
    const QString &xml = d->getEpisodeContent(episodeToRenew);

    // FIXME: see bool EpisodeModel::validateEpisode(const QModelIndex &index)
    bool ro = d->_readOnly;
    d->_readOnly = false;
    if (!insertRow(rowCount())) {
        LOG_ERROR("Unable to create a new episode");
        return QModelIndex();
    }

    QModelIndex newEpisode = index(rowCount() - 1, 0);
    QModelIndex xmlIndex = index(newEpisode.row(), EpisodeModel::XmlContent);
    if (setData(xmlIndex, xml)) {
        d->_readOnly = ro;
        return newEpisode;
    }
    d->_readOnly = ro;
    return QModelIndex();
}

/**
 * Reset the model.
 */
void EpisodeModel::refreshFilter()
{
    // Force filter updating
    QString patientUid = d->_currentPatientUuid;
    d->_currentPatientUuid.clear();
    d->updateFilter(patientUid);
}

/**
 * Populate the Form::IFormItemData of the parent Form::FormMain pointer
 * with the content of the episode. If the index is not valid, returns false.
 *
 * If \e feedPatientModel is set to true and if the Form::IFormItemData has a
 * patient data representation, patient model will be automatically populated
 * with the value of the item data.
 * \sa Form::IFormItemData, Form::FormItem::patientDataRepresentation()
 */
bool EpisodeModel::populateFormWithEpisodeContent(const QModelIndex &episode, bool feedPatientModel)
{
    QTime chrono;
    if (WarnLogChronos)
        chrono.start();
    d->_formMain->clear();
    if (!episode.isValid()) {
        // Nothing to do but clear the form
        LOG(QString("populateWithEpisodeContent: index not valid, clearing form: %1").arg(d->_formMain->uuid()));
        return true;
    }

    d->_formMain->formWidget()->setEnabled(false);
    const QString &xml = d->getEpisodeContent(episode);

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
        foreach(FormItem *it, d->_formMain->flattenedFormItemChildren()) {
            if (!it->itemData())
                continue;
            items.insert(it->uuid(), it);
        }
    }

    // Populate the FormMain item data (username, userdatetime, label)
    QModelIndex userName = index(episode.row(), EpisodeModel::UserCreatorName);
    QModelIndex userDateTime = index(episode.row(), EpisodeModel::UserDateTime);
    QModelIndex label = index(episode.row(), EpisodeModel::Label);
    QModelIndex prior = index(episode.row(), EpisodeModel::Priority);
    d->_formMain->itemData()->setData(IFormItemData::ID_EpisodeDateTime, this->data(userDateTime));
    d->_formMain->itemData()->setData(IFormItemData::ID_EpisodeLabel, this->data(label));
    d->_formMain->itemData()->setData(IFormItemData::ID_UserName, this->data(userName));
    d->_formMain->itemData()->setData(IFormItemData::ID_Priority, this->data(prior));
    d->_formMain->itemData()->setModified(false);

    // Populate FormItem data and the patientmodel
    foreach(FormItem *it, items.values()) {
        if (!it) {
            LOG_ERROR("populateFormWithEpisode :: ERROR: no item: " + items.key(it));
            continue;
        }

        QString value = _data.value(it->uuid(), QString::null);
        bool setToModified = false;
        // If the content data is empty, check the equivalents
        if (value.isEmpty()) {
            // Find the first equivalent that lead to a value
            foreach(const QString &eq, it->spec()->equivalentUuid()) {
                // Does the equivalent uuid leads to a value ?
                value = _data.value(eq, QString::null);
                if (!value.isEmpty()) {
                    // Force item update
                    setToModified = true;
                    LOG(QString("Found a field equivalent: %2 -> %1").arg(it->uuid(), eq));
                    break;
                }
            }
        }
        it->itemData()->setStorableData(value);

        if (feedPatientModel && it->patientDataRepresentation() >= 0)
            patient()->setValue(it->patientDataRepresentation(), it->itemData()->data(it->patientDataRepresentation(), IFormItemData::PatientModelRole));

        if (setToModified)
            it->itemData()->setModified(true);
    }

    d->_formMain->formWidget()->setEnabled(true);

    // TODO: if episode is validated ==> read-only
    if (d->isEpisodeValidated(episode)) {
        setReadOnly(true);
    } else {
        setReadOnly(false);
    }

    if (WarnLogChronos)
        Utils::Log::logTimeElapsed(chrono, objectName(), "populateFormWithEpisode");
    return true;
}

/**
 * Populate the Form::IFormItemData of the parent Form::FormMain pointer
 * with the content of the latest valid episode content.
 */
bool EpisodeModel::populateFormWithLatestValidEpisodeContent()
{
    if (rowCount() == 0)
        return true;
    // as the SqlModel is sorted on the userdatetime, we just need to populate with the last index of the model
    QModelIndex index = this->index(rowCount() - 1,0);
    return populateFormWithEpisodeContent(index);
}

/**
 * Save the whole model, set the Form::IFormItemData to non-modified if the
 * submission went right.
 * This does include the root Form::FormMain of this model.
 * \sa isDirty()
 */
bool EpisodeModel::submit()
{
    // No current patient uuid defined?
    if (d->_currentPatientUuid.isEmpty()) {
        LOG_ERROR("No patient uuid. Unable to submit EpisodeModel.");
        return false;
    }

    // Signal all dirty indexes
    foreach(const QModelIndex &index, d->_dirtyIndexes)
        Q_EMIT dataChanged(index, index);
    d->_dirtyIndexes.clear();

    // Submit the internal model (block the reset signal emitted when submitting the model)
    d->_sqlModel->blockSignals(true);
    bool ok = d->_sqlModel->submit();

    // Set all formitemdata to a non-modified state
    if (ok) {
        foreach(FormItem *it, d->_formMain->flattenedFormItemChildren()) {
            if (it->itemData())
                it->itemData()->setModified(false);
        }
        d->_formMain->itemData()->setModified(false);
    }
    d->_sqlModel->blockSignals(false);
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

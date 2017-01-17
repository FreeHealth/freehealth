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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERT_ALERTITEM
#define ALERT_ALERTITEM

#include <alertplugin/alertplugin_exporter.h>
#include <QString>
#include <QVariant>
#include <QDateTime>

QT_BEGIN_NAMESPACE
class QDomElement;
QT_END_NAMESPACE

/**
 * \file alertitem.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 28 May 2012
*/

namespace Alert {

class ALERT_EXPORT AlertTiming
{
public:
    AlertTiming();
    AlertTiming(const QDateTime &start, const QDateTime &expirationDate);
    AlertTiming(const AlertTiming &copy);
    virtual ~AlertTiming() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_modified=true; _id = id;}
    virtual bool isValid() const {return _valid;}
    virtual void setValid(bool state) {_modified=true; _valid=state;}

    virtual void setModified(bool state) {_modified = state;}
    virtual bool isModified() const {return _modified;}

    virtual QDateTime start() const {return _start;}
    virtual QDateTime end() const {return _end;}
    virtual QDateTime expiration() const {return _end;}
    virtual void setStart(const QDateTime &dt) {_modified=true; _start = QDateTime(dt.date(), QTime(dt.time().hour(), dt.time().minute(), dt.time().second()));}
    virtual void setEnd(const QDateTime &dt) {_modified=true; _end = QDateTime(dt.date(), QTime(dt.time().hour(), dt.time().minute(), dt.time().second()));}
    virtual void setExpiration(const QDateTime &dt) {_modified=true; _end = QDateTime(dt.date(), QTime(dt.time().hour(), dt.time().minute(), dt.time().second()));}

    virtual bool isCycling() const {return _isCycle;}
    virtual void setCycling(bool cycle) {_modified=true; _isCycle=cycle;}
    virtual int numberOfCycles() const {return _nCycle;}
    virtual void setNumberOfCycles(int n);
    int currentCycle() const {return _currentCycle;}

    // TODO: obsolete
    virtual QDateTime nextDate() const {return _next;}
    virtual void setNextDate(const QDateTime &dt) {_modified=true; _next=QDateTime(dt.date(), QTime(dt.time().hour(), dt.time().minute(), dt.time().second()));}
    // END

    virtual qlonglong cyclingDelayInMinutes() const {return _delayInMins;}
    virtual void setCyclingDelayInMinutes(const qlonglong delay) {_modified=true; _delayInMins=delay;}

    virtual qlonglong cyclingDelayInHours() const {return qlonglong(_delayInMins/60);}
    virtual void setCyclingDelayInHours(const qlonglong delay) {_modified=true; _delayInMins=delay*60;}

    virtual qlonglong cyclingDelayInDays() const {return qlonglong(_delayInMins/60/24);}
    virtual void setCyclingDelayInDays(const qlonglong delay) {_modified=true; _delayInMins=delay*60*24;}

    virtual qlonglong cyclingDelayInWeeks() const {return qlonglong(_delayInMins/60/24/7);}
    virtual void setCyclingDelayInWeeks(const qlonglong delay) {_modified=true; _delayInMins=delay*60*24*7;}

    virtual qlonglong cyclingDelayInMonth() const {return qlonglong(_delayInMins/60/24/30);}
    virtual void setCyclingDelayInMonth(const qlonglong delay) {_modified=true; _delayInMins=delay*60*24*30;}

    virtual qlonglong cyclingDelayInYears() const {return qlonglong(_delayInMins/60/24/365.25);}
    virtual void setCyclingDelayInYears(const qlonglong delay) {_modified=true; _delayInMins=qlonglong(delay*60*24*365.25);}

    virtual qlonglong cyclingDelayInDecades() const {return qlonglong(_delayInMins/60/24/365.25/10);}
    virtual void setCyclingDelayInDecades(const qlonglong delay) {_modified=true; _delayInMins=qlonglong(delay*60*24*365.25*10);}

    virtual void cyclingDelay(qlonglong *min, qlonglong *hours, qlonglong *days, qlonglong *weeks,
                              qlonglong *months, qlonglong *years, qlonglong *decades) const;

    virtual void cyclingDelayPeriodModulo(int *period, int *mod) const;

    // Values not saved in the database, class is not considered as modified when settings these params
    virtual QDateTime currentCycleStartDate() const {return _cycleStartDate;}
    virtual QDateTime currentCycleExpirationDate() const {return _cycleExpirationDate;}
    // virtual void setCycleStartDate(const QDateTime &dt) {_cycleStartDate=dt;}
    // virtual void setCycleExpirationDate(const QDateTime &dt) {_cycleExpirationDate=dt;}

    virtual QString toXml() const;
    static AlertTiming &fromXml(const QString &xml);
    static AlertTiming &fromDomElement(const QDomElement &element);

    bool operator==(const AlertTiming &other) const;
    bool operator!=(const AlertTiming &other) const {return !operator==(other);}

private:
    void computeCycle();

private:
    int _id, _nCycle, _currentCycle;
    QDateTime _start, _end, _next;
    qlonglong _delayInMins;
    bool _valid, _isCycle;
    bool _modified;
    QDateTime _cycleStartDate, _cycleExpirationDate;
};

class ALERT_EXPORT AlertScript
{
public:
    enum ScriptType {
        CheckValidityOfAlert = 0,
        CyclingStartDate,
        OnAboutToShow,
        DuringAlert,
        OnAboutToValidate,
        OnAboutToOverride,
        OnOverridden,
        OnPatientAboutToChange,
        OnUserAboutToChange,
        OnEpisodeAboutToSave,
        OnEpisodeLoaded,
        OnRemindLater
    };

    AlertScript() : _id(-1), _valid(true), _type(CheckValidityOfAlert), _modified(false) {}
    AlertScript(const QString &uuid, ScriptType type, const QString &script) :
        _id(-1), _valid(true),
        _type(type), _uid(uuid), _script(script), _modified(true) {}
    virtual ~AlertScript() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_modified=true; _id = id;}

    virtual QString uuid() const {return _uid;}
    virtual void setUuid(const QString &uid) {_modified=true; _uid=uid;}

    virtual bool isNull() const {return _script.isEmpty();}

    virtual bool isValid() const {return _valid;}
    virtual void setValid(bool state) {_modified=true; _valid=state;}

    virtual void setModified(bool state) {_modified = state;}
    virtual bool isModified() const {return _modified;}

    virtual ScriptType type() const {return _type;}
    virtual void setType(ScriptType type) {_modified=true; _type=type;}

    virtual QString script() const {return _script;}
    virtual void setScript(const QString &script) {_modified=true; _script=script;}

    static QString typeToString(ScriptType type);
    static QString typeToXml(ScriptType type);
    static ScriptType typeFromXml(const QString &xml);
    virtual QString toXml() const;
    static AlertScript &fromXml(const QString &xml);
    static AlertScript &fromDomElement(const QDomElement &element);

    bool operator<(const AlertScript &script) const;
    bool operator==(const AlertScript &other) const;
    bool operator!=(const AlertScript &other) const {return !operator==(other);}

private:
    int _id;
    bool _valid;
    ScriptType _type;
    QString _uid, _script;
    bool _modified;
};

class ALERT_EXPORT AlertValidation
{
public:
    AlertValidation() : _id(-1), _modified(false), _overridden(false) {}
    AlertValidation(const QDateTime &dateTimeOfValidation, const QString &validatorUid, const QString &validatedUid) :
        _id(-1),
        _modified(true),
        _overridden(false),
        _validator(validatorUid), _validated(validatedUid),
        _date(dateTimeOfValidation)
    {}
    virtual ~AlertValidation() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_modified=true; _id = id;}

    virtual void setModified(bool state) {_modified = state;}
    virtual bool isModified() const {return _modified;}

    virtual QString validatorUid() const {return _validator;}
    virtual void setValidatorUuid(const QString &uid) {_modified=true; _validator=uid;}

    virtual QString userComment() const {return _userComment;}
    virtual void setUserComment(const QString &comment) {_modified=true; _userComment=comment;}

    virtual void setOverriden(bool overridden) {_overridden=overridden;}
    virtual bool isOverriden() const {return _overridden;}
    virtual void setAccepted(bool accepted) {_overridden=!accepted;}
    virtual bool isAccepted() const {return !_overridden;}

    virtual QDateTime dateOfValidation() const {return _date;}
    virtual void setDateOfValidation(const QDateTime &dt) {_modified=true; _date=QDateTime(dt.date(), QTime(dt.time().hour(), dt.time().minute(), dt.time().second()));}

    virtual QString validatedUid() const {return _validated;}
    virtual void setValidatedUuid(const QString &uid) {_validated=uid;}

    virtual QString toXml() const;
    static AlertValidation &fromXml(const QString &xml);
    static AlertValidation &fromDomElement(const QDomElement &element);

    bool operator==(const AlertValidation &other) const;
    bool operator!=(const AlertValidation &other) const {return !operator==(other);}

private:
    int _id;
    bool _modified, _overridden;
    QString _validator, _userComment, _validated;
    QDateTime _date;
};

class ALERT_EXPORT AlertRelation
{
public:
    // THE ORDER OF THIS ENUM MUST NOT CHANGE
    enum RelatedTo {
        RelatedToPatient = 0,
        RelatedToAllPatients,
        RelatedToFamily,
        RelatedToUser,
        RelatedToAllUsers,
        RelatedToUserGroup,
        RelatedToApplication
    };
    AlertRelation() : _id(-1), _modified(false), _related(RelatedToPatient) {}
    AlertRelation(RelatedTo related, const QString &uuid = QString::null) :
        _id(-1), _modified(true),
        _related(related),
        _relatedUid(uuid)
    {}
    virtual ~AlertRelation() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_modified=true; _id = id;}

    virtual void setModified(bool state) {_modified = state;}
    virtual bool isModified() const {return _modified;}

    virtual RelatedTo relatedTo() const {return _related;}
    virtual void setRelatedTo(RelatedTo related) {_modified=true; _related = related;}
    virtual QString relationTypeToString() const;

    virtual QString relatedToUid() const {return _relatedUid;}
    virtual void setRelatedToUid(const QString &uid) {_modified=true; _relatedUid=uid;}

    virtual QString toXml() const;
    static AlertRelation &fromXml(const QString &xml);
    static AlertRelation &fromDomElement(const QDomElement &element);
    static QString relationTypeToXml(AlertRelation::RelatedTo rel);
    static RelatedTo relationTypeFromXml(const QString &xmlValue);

    bool operator==(const AlertRelation &other) const;
    bool operator!=(const AlertRelation &other) const {return !operator==(other);}

private:
    int _id;
    bool _modified;
    RelatedTo _related;
    QString _relatedUid;
};

namespace Internal {
class AlertItemPrivate;
class AlertPlugin;
class AlertBase;
}
class ALERT_EXPORT AlertItem
{
#ifdef WITH_TESTS
    friend class Alert::Internal::AlertPlugin;
#endif
    friend class Alert::Internal::AlertBase;

protected:
    void setDb(int ref, const QVariant &value);
    QVariant db(int ref) const;

public:
    enum ViewType {
        BlockingAlert = 0,
        NonBlockingAlert
    };
    enum ContentType {
        ApplicationNotification = 0,
        PatientCondition,
        UserNotification
    };
    enum Priority {
        High = 0,
        Medium,
        Low
    };

    AlertItem();
    AlertItem(const AlertItem &cp);
    AlertItem &operator=(const AlertItem &cp);

    virtual ~AlertItem();
    virtual bool isValid() const;
    virtual void setValidity(bool isValid);

    virtual QString uuid() const;
    virtual void setUuid(const QString &uid) const;

    virtual QString packUid() const;
    virtual void setPackUid(const QString &uid) const;

    virtual QString cryptedPassword() const;
    virtual void setCryptedPassword(const QString &pass);

    virtual bool isModified() const;
    virtual void setModified(bool modified);

    virtual QString label(const QString &lang = QString::null) const;
    virtual QString toolTip(const QString &lang = QString::null) const;
    virtual QString category(const QString &lang = QString::null) const;
    virtual QString description(const QString &lang = QString::null) const;
    virtual QString comment(const QString &lang = QString::null) const;

    virtual void setLabel(const QString &txt, const QString &lang = QString::null);
    virtual void setToolTip(const QString &txt, const QString &lang = QString::null);
    virtual void setCategory(const QString &txt, const QString &lang = QString::null);
    virtual void setDescription(const QString &txt, const QString &lang = QString::null);
    virtual void setComment(const QString &txt, const QString &lang = QString::null);

    virtual QStringList availableLanguages() const;
    virtual void removeAllLanguages();

    virtual ViewType viewType() const;
    virtual ContentType contentType() const;
    virtual Priority priority() const;
    virtual QString priorityToString() const;
    virtual bool isOverrideRequiresUserComment() const;
    virtual bool mustBeRead() const;
    virtual bool isRemindLaterAllowed() const;
    virtual bool isEditable() const;

    virtual void setViewType(ViewType type);
    virtual void setContentType(ContentType content);
    virtual void setPriority(Priority priority);
    virtual void setOverrideRequiresUserComment(bool required);
    virtual void setMustBeRead(bool mustberead);
    virtual void setRemindLaterAllowed(bool allowed);
    virtual void setEditable(bool editable);

    virtual QDateTime creationDate() const;
    virtual void setCreationDate(const QDateTime &dt);
    virtual QDateTime lastUpdate() const;
    virtual void setLastUpdate(const QDateTime &dt);

    virtual QString themedIcon() const;
    virtual void setThemedIcon(const QString &icon);
    virtual QString styleSheet() const;
    virtual void setStyleSheet(const QString &css);
    virtual QString priorityBackgroundColor() const;
    static QIcon priorityBigIcon(Priority priority);
    virtual QIcon priorityBigIcon() const;
    virtual QString htmlToolTip(bool showCategory = true) const;

    virtual QString extraXml() const;
    virtual void setExtraXml(const QString &xml);

    virtual void clearRelations();
    virtual AlertRelation &relation(int id) const;
    virtual QVector<AlertRelation> &relations() const;
    virtual AlertRelation &relationAt(int id) const;
    virtual void addRelation(const AlertRelation &relation);

    virtual void clearTimings();
    virtual AlertTiming &timing(int id) const;
    virtual QVector<AlertTiming> &timings() const;
    virtual AlertTiming &timingAt(int id) const;
    virtual void addTiming(const AlertTiming &timing);

    virtual void clearScripts();
    virtual AlertScript &script(int id) const;
    virtual AlertScript &scriptType(AlertScript::ScriptType type) const;
    virtual QVector<AlertScript> &scripts() const;
    virtual AlertScript &scriptAt(int id) const;
    virtual void addScript(const AlertScript &script);
    virtual void setScripts(const QVector<AlertScript> &scripts);

    bool setRemindLater();
    bool validateAlertWithCurrentUserAndConfirmationDialog();
    bool validateAlert(const QString &validatorUid, bool override, const QString &overrideComment, const QDateTime &dateOfValidation);
    bool isUserValidated() const;
    virtual void clearValidations();
    virtual AlertValidation &validation(int id) const;
    virtual QVector<AlertValidation> &validations() const;
    virtual AlertValidation &validationAt(int id) const;
    virtual void addValidation(const AlertValidation &val);

    bool operator==(const AlertItem &other) const;
    bool operator!=(const AlertItem &other) const;

    static bool priorityLowerThan(const AlertItem &item1, const AlertItem &item2);
    static bool categoryLowerThan(const AlertItem &item1, const AlertItem &item2);

    QString toXml() const;
    static AlertItem &fromXml(const QString &xml);

private:
    Internal::AlertItemPrivate *d;
};

}  // namespace Alert

ALERT_EXPORT QDebug operator<<(QDebug dbg, const Alert::AlertTiming &c);
ALERT_EXPORT QDebug operator<<(QDebug dbg, const Alert::AlertTiming *c);

ALERT_EXPORT QDebug operator<<(QDebug dbg, const Alert::AlertItem &c);
ALERT_EXPORT QDebug operator<<(QDebug dbg, const Alert::AlertItem *c);

#endif  // ALERT_ALERTITEM


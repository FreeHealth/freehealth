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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERT_ALERTITEM
#define ALERT_ALERTITEM

#include <alertplugin/alertplugin_exporter.h>
#include <QString>
#include <QVariant>
#include <QDateTime>

/**
 * \file alertitem.h
 * \author Eric MAEKER <eric.maeker@gmail.com>, Pierre-Marie Desombre <pm.desombre@gmail.com>
 * \version 0.8.0
 * \date 28 May 2012
*/

namespace Alert {

class ALERT_EXPORT AlertTiming
{
public:
    AlertTiming() : _id(-1), _delay(-1), _ncycle(0), _valid(true), _isCycle(false) {}
    virtual ~AlertTiming() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_id = id;}
    virtual bool isValid() const {return _valid;}
    virtual void setValid(bool state) {_valid=state;}

    virtual QDateTime start() const {return _start;}
    virtual QDateTime end() const {return _end;}
    virtual void setStart(const QDateTime &dt) {_start = dt;}
    virtual void setEnd(const QDateTime &dt) {_end = dt;}

    virtual bool isCycling() const {return _isCycle;}
    virtual void setCycling(bool cycle) {_isCycle=cycle;}
    virtual int numberOfCycles() const {return _ncycle;}
    virtual void setNumberOfCycles(int n) {_ncycle=n; if (n>0)_ncycle=true;}
    virtual QDateTime nextDate() const {return _next;}
    virtual void setNextDate(const QDateTime &dt) {_next = dt;}
    virtual int cyclingDelayInDays() const {return _delay;}
    virtual void setCyclingDelayInDays(const int delay) {_delay=delay;}

private:
    int _id, _delay, _ncycle;
    QDateTime _start, _end, _next;
    bool _valid, _isCycle;
};

class ALERT_EXPORT AlertScript
{
public:
    enum ScriptType {
        BeforeAlert = 0,
        DuringAlert,
        AfterAlert
    };

    AlertScript() : _id(-1), _valid(true) {}
    virtual ~AlertScript() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_id = id;}

    virtual QString uuid() const {return _uid;}
    virtual void setUuid(const QString &uid) {_uid=uid;}

    virtual bool isValid() const {return _valid;}
    virtual void setValid(bool state) {_valid=state;}

    virtual ScriptType type() {return _type;}
    virtual void setType(ScriptType type) {_type=type;}

    virtual QString script() const {return _script;}
    virtual void setScript(const QString &script) {_script=script;}

private:
    int _id;
    bool _valid;
    ScriptType _type;
    QString _uid, _script;
};

class ALERT_EXPORT AlertValidation
{
public:
    AlertValidation() : _id(-1) {}
    virtual ~AlertValidation() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_id = id;}

    virtual QString userUid() const {return _userUid;}
    virtual void setUserUuid(const QString &uid) {_userUid=uid;}
    virtual QString userComment() const {return _userComment;}
    virtual void setUserComment(const QString &comment) {_userComment=comment;}

    virtual QDateTime dateOfValidation() const {return _date;}
    virtual void setDateOfValidation(const QDateTime &dt) {_date=dt;}

private:
    int _id;
    QString _userUid, _userComment;
    QDateTime _date;
};

class ALERT_EXPORT AlertRelation
{
public:
    enum RelatedTo {
        RelatedToPatient = 0,
        RelatedToFamily,
        RelatedToAllPatients,
        RelatedToUser,
        RelatedToUserGroup,
        RelatedToApplication
    };
    AlertRelation() : _id(-1) {}
    virtual ~AlertRelation() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_id = id;}

    virtual RelatedTo relatedTo() const {return _related;}
    virtual void setRelatedTo(RelatedTo related) {_related = related;}

    virtual QString relatedToUid() const {return _relatedUid;}
    virtual void setRelatedToUid(const QString &uid) {_relatedUid=uid;}

private:
    int _id;
    RelatedTo _related;
    QString _relatedUid;
};

namespace Internal {
class AlertItemPrivate;
class AlertBase;
}
class ALERT_EXPORT AlertItem
{
    friend class Alert::Internal::AlertBase;

protected:
    void setDb(int ref, const QVariant &value);
    QVariant db(int ref) const;

public:
    enum ViewType {
        DynamicAlert = 0,
        StaticAlert
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

    virtual ~AlertItem();
    virtual bool isValid() const;
    virtual void setValidity(bool isValid);

    virtual QString uuid() const;
    virtual void setUuid(const QString &uid) const;

    virtual QString cryptedPassword() const;
    virtual void setCryptedPassword(const QString &pass);

    virtual bool isModified() const;
    virtual void setModified(bool modified);

    virtual QString label(const QString &lang = QString::null) const;
    virtual QString category(const QString &lang = QString::null) const;
    virtual QString description(const QString &lang = QString::null) const;
    virtual QString comment(const QString &lang = QString::null) const;

    virtual void setLabel(const QString &txt, const QString &lang = QString::null);
    virtual void setCategory(const QString &txt, const QString &lang = QString::null);
    virtual void setDescription(const QString &txt, const QString &lang = QString::null);
    virtual void setComment(const QString &txt, const QString &lang = QString::null);

    virtual QStringList availableLanguages() const;

    virtual ViewType viewType() const;
    virtual ContentType contentType() const;
    virtual Priority priority() const;
    virtual bool isOverrideRequiresUserComment() const;
    // TODO : virtual xxx condition() const = 0;

    virtual void setViewType(ViewType type);
    virtual void setContentType(ContentType content);
    virtual void setPriority(Priority priority);
    virtual void setOverrideRequiresUserComment(bool required);
    // TODO : virtual void setCondition(...);

    virtual QDateTime creationDate() const;
    virtual void setCreationDate(const QDateTime &dt);
    virtual QDateTime lastUpdate() const;
    virtual void setLastUpdate(const QDateTime &dt);

    virtual QString themedIcon() const;
    virtual void setThemedIcon(const QString &icon);
    virtual QString styleSheet() const;
    virtual void setStyleSheet(const QString &css);

    virtual QString extraXml() const;
    virtual void setExtraXml(const QString &xml);

    // TODO : move this in AlertModel ?
    virtual AlertRelation &relation(int id) const;
    virtual QVector<AlertRelation> &relations() const;
    virtual AlertRelation &relationAt(int id) const;
    virtual void addRelation(const AlertRelation &relation);

    virtual AlertTiming &timing(int id) const;
    virtual QVector<AlertTiming> &timings() const;
    virtual AlertTiming &timingAt(int id) const;
    virtual void addTiming(const AlertTiming &timing);

    virtual AlertScript &script(int id) const;
    virtual QVector<AlertScript> &scripts() const;
    virtual AlertScript &scriptAt(int id) const;
    virtual void addScript(const AlertScript &script);

    virtual AlertValidation &validation(int id) const;
    virtual QVector<AlertValidation> &validations() const;
    virtual AlertValidation &validationAt(int id) const;
    virtual void addValidation(const AlertValidation &val);
    // END

    bool operator==(const AlertItem &other) const;

private:
    Internal::AlertItemPrivate *d;
};

}  // namespace Alert

ALERT_EXPORT QDebug operator<<(QDebug dbg, const Alert::AlertItem &c);
ALERT_EXPORT QDebug operator<<(QDebug dbg, const Alert::AlertItem *c);

#endif  // ALERT_ALERTITEM


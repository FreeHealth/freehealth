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
/**
 * \class Alert::AlertItem
 * Represent an alert.
 * \note Unit-test available (see: Alert:Internal::AlertPlugin)
 */

/**
 * \enum Alert::AlertItem::ViewType
 * Define the type of presentation of the alert.
 */

/**
 * \var Alert::AlertItem::BlockingAlert
 * A blocking alert must be validated by the user. They are presented in a 'blocking' dialog. The user
 * must validate/cancel the dialog to pursue its actions.
 */

/**
 * \var Alert::AlertItem::NonBlockingAlert
 * Non-blocking alerts are alerts that are presented to the user without requiering a validation.
 * For eg, they can be QToolButton, QLabel, QIcon (in views)...
 * \sa Alert::IAlertPlaceHolder
*/

/**
 * \enum Alert::AlertItem::ContentType
 * Descrive the content of an alert.
 */

/**
 * \var Alert::AlertItem::ApplicationNotification
 * Alert is related to the application
 */

/**
 * \var Alert::AlertItem::PatientCondition
 * Alert is related to the patient condition (like allergies, DDI, PIM, diseases, prevention alerts...)
 */

/**
 * \var Alert::AlertItem::UserNotification
 * Alert is related to the user (like 'You have mails', 'My notes'...).
 */

/**
 * \enum Alert::AlertItem::Priority
 * Descrive the priority of an alert.
 * In blocking dialogs, alerts are presented sorted by their priority.
 */

/**
 * \var Alert::AlertItem::High
 * Alert priority is high
 */
/**
 * \var Alert::AlertItem::Medium
 * Alert priority is medium
 */
/**
 * \var Alert::AlertItem::Low
 * Alert priority is low
 */

/**
 * \class Alert::AlertTiming
 * Represent a specific timing for the alert. Alert can be one shot (one timing),
 * multi-defined shots (multi-timings), or cycling alerts. \n
 * When you define a cycling alert, you must in first define all the cycle data before
 * defining the total number of cycle (with setNumberOfCycles()). The current cycle:
 * - number (currentCycle())
 * - start date (currentCycleStartDate())
 * - end date (currentCycleExpirationDate())
 * will be automatically computed.
 * \note: all QTime milliseconds are lost (set to zero) to ensure a total correspondance
 * with database saved timings and XML transformation.
 * \note Unit-test available (see: Alert:Internal::AlertPlugin)
 */

/**
 * \fn void Alert::AlertTiming::setNumberOfCycles(int n)
 * Defines the number of cycling of the alert, n must be > 0 otherwise the alert will not be cycling.
 */

#include "alertitem.h"
#include "alertcore.h"
#include "alertpackdescription.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_colors.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/genericdescription.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_datetime.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/multilingualclasstemplate.h>

#include <QTreeWidgetItem>
#include <QDomDocument>

#include <QDebug>

enum { WarnAlertItemConstructionDestruction = false };

using namespace Alert;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Alert::AlertCore &alertCore() {return Alert::AlertCore::instance();}

namespace {
const char * const XML_ROOT_TAG = "Alert";
const char * const XML_DESCRIPTION_ROOTTAG = "Descr";
const char * const XML_TIMING_ROOTTAG = "Timings";
const char * const XML_RELATED_ROOTTAG = "Rels";
const char * const XML_VALIDATION_ROOTTAG = "Vals";
const char * const XML_SCRIPT_ROOTTAG = "Scripts";
const char * const XML_EXTRAXML_ROOTTAG = "Xtra";

const char * const XML_TIMING_ELEMENTTAG = "Timing";
const char * const XML_RELATED_ELEMENTTAG = "Rel";
const char * const XML_VALIDATION_ELEMENTTAG = "Val";
const char * const XML_SCRIPT_ELEMENTTAG = "Script";

//const char * const XML_DATABASE_VALUES_TAG = "Alert";
}

namespace Alert {
namespace Internal {

class AlertXmlDescription  : public Utils::GenericDescription
{
public:
    enum NonTr {
        CryptedPass = Utils::GenericDescription::NonTranslatableExtraData + 1,
        ViewType,
        ContentType,
        Priority,
        OverrideRequiresUserComment,
        MustBeRead,
        RemindLater,
        Editable,
        StyleSheet,
        PackUid
    };
    enum Tr {
        Comment = Utils::GenericDescription::TranslatableExtraData + 1
    };

    AlertXmlDescription() : Utils::GenericDescription(::XML_DESCRIPTION_ROOTTAG)
    {
        addNonTranslatableExtraData(CryptedPass, "cryptedPassword");
        addNonTranslatableExtraData(ViewType, "viewType");
        addNonTranslatableExtraData(ContentType, "contentType");
        addNonTranslatableExtraData(Priority, "prior");
        addNonTranslatableExtraData(OverrideRequiresUserComment, "overrideComment");
        addNonTranslatableExtraData(MustBeRead, "mustBeRead");
        addNonTranslatableExtraData(RemindLater, "remindLater");
        addNonTranslatableExtraData(Editable, "editable");
        addNonTranslatableExtraData(StyleSheet, "styleSheet");
        addNonTranslatableExtraData(PackUid, "packUid");
        addTranslatableExtraData(Comment, "comment");
    }

    ~AlertXmlDescription() {}
};

class AlertValueBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l) const
    {
        Q_UNUSED(l);
    }

public:
    QString _label, _toolTip, _category, _descr, _comment;
};

class AlertItemPrivate : public Trans::MultiLingualClass<AlertValueBook>
{
public:
    AlertItemPrivate(AlertItem *parent) :
        _id(-1),
        _valid(true), _modified(false),
        _editable(true), _overrideRequiresUserComment(false),
        _mustBeRead(false), _remindAllowed(false),
        _viewType(AlertItem::NonBlockingAlert),
        _contentType(AlertItem::ApplicationNotification),
        _priority(AlertItem::Medium),
        q(parent)
    {}

    ~AlertItemPrivate() {}

    QString viewTypeToXml()
    {
        switch (_viewType) {
        case AlertItem::BlockingAlert: return "blocking";
        case AlertItem::NonBlockingAlert: return "nonblocking";
        }
        return QString::null;
    }

    QString contentTypeToXml()
    {
        switch (_contentType) {
        case AlertItem::ApplicationNotification: return "applicationNotification";
        case AlertItem::PatientCondition: return "patientCondition";
        case AlertItem::UserNotification: return "userNotification";
        }
        return QString::null;
    }

    QString priorityToXml()
    {
        switch (_priority) {
        case AlertItem::High: return "high";
        case AlertItem::Medium: return "medium";
        case AlertItem::Low: return "low";
        }
        return QString::null;
    }

    void viewTypeFromXml(const QString &xml)
    {
        // default is blocking alert
        _viewType = AlertItem::BlockingAlert;
        if (xml.compare("static", Qt::CaseInsensitive)==0) {
            _viewType = AlertItem::NonBlockingAlert;
        }
    }

    void contentTypeFromXml(const QString &xml)
    {
        // default is patientCondition
        _contentType = AlertItem::PatientCondition;
        if (xml.compare("applicationNotification", Qt::CaseInsensitive)==0) {
            _contentType = AlertItem::ApplicationNotification;
        } else if (xml.compare("userNotification", Qt::CaseInsensitive)==0) {
            _contentType = AlertItem::UserNotification;
        }
    }

    void priorityFromXml(const QString &xml)
    {
        // default is low
        _priority = AlertItem::Low;
        if (xml.compare("high", Qt::CaseInsensitive)==0) {
            _priority = AlertItem::High;
        } else if (xml.compare("medium", Qt::CaseInsensitive)==0) {
            _priority = AlertItem::Medium;
        }
    }

    void feedItemWithXmlDescription()
    {
        _id = -1;
        _uid = descr.data(AlertXmlDescription::Uuid).toString();
        _pass = descr.data(AlertXmlDescription::CryptedPass).toString();
        _themedIcon = descr.data(AlertXmlDescription::GeneralIcon).toString();
        _css = descr.data(AlertXmlDescription::StyleSheet).toString();
        _valid = descr.data(AlertXmlDescription::Validity).toBool();
        _overrideRequiresUserComment = descr.data(AlertXmlDescription::OverrideRequiresUserComment).toBool();
        _mustBeRead = descr.data(AlertXmlDescription::MustBeRead).toBool();
        _remindAllowed = descr.data(AlertXmlDescription::RemindLater).toBool();
        _editable = descr.data(AlertXmlDescription::Editable).toBool();
        viewTypeFromXml(descr.data(AlertXmlDescription::ViewType).toString());
        contentTypeFromXml(descr.data(AlertXmlDescription::ContentType).toString());
        priorityFromXml(descr.data(AlertXmlDescription::Priority).toString());
        _creationDate = QDateTime::fromString(descr.data(AlertXmlDescription::CreationDate).toString(), Qt::ISODate);
        _update = QDateTime::fromString(descr.data(AlertXmlDescription::LastModificationDate).toString(), Qt::ISODate);
        _packUid = descr.data(AlertXmlDescription::PackUid).toString();

        foreach(const QString &l, descr.availableLanguages()) {
            q->setLabel(descr.data(Internal::AlertXmlDescription::Label, l).toString(), l);
            q->setToolTip(descr.data(Internal::AlertXmlDescription::ToolTip, l).toString(), l);
            q->setCategory(descr.data(Internal::AlertXmlDescription::Category, l).toString(), l);
            q->setDescription(descr.data(Internal::AlertXmlDescription::ShortDescription, l).toString(), l);
            q->setComment(descr.data(Internal::AlertXmlDescription::Comment, l).toString(), l);
        }
    }

    QString categoryForTreeWiget() const {return QString::null;}

    bool validationsContainsValidatedUuid(const QString &uuid)
    {
        for(int i=0; i< _validations.count(); ++i) {
            const AlertValidation &val = _validations.at(i);
            if (val.validatedUid().compare(uuid, Qt::CaseInsensitive)==0)
                return true;
        }
        return false;
    }

public:
    QString _uid, _packUid, _pass, _themedIcon, _css, _extraXml;
    int _id;
    bool _valid, _modified, _editable, _overrideRequiresUserComment, _mustBeRead, _remindAllowed;
    AlertItem::ViewType _viewType;
    AlertItem::ContentType _contentType;
    AlertItem::Priority _priority;
    QHash<int, QVariant> _db;
    QDateTime _creationDate, _update;
    AlertXmlDescription descr;

    // TODO : move this in an AlertModel
    QVector<AlertRelation> _relations;
    QVector<AlertScript> _scripts;
    QVector<AlertTiming> _timings;
    QVector<AlertValidation> _validations;
    AlertRelation _nullRelation;
    AlertScript _nullScript;
    AlertTiming _nullTiming;
    AlertValidation _nullValidation;
    // END

private:
    AlertItem *q;
};

}
}

AlertItem::AlertItem() :
    d(new Internal::AlertItemPrivate(this))
{
    if (WarnAlertItemConstructionDestruction)
        qWarning() << "AlertItem" << d->_uid << d;
}

// The copy constructor: we have to copy the content of the private part.
// The multilingual class does not store pointers so the copy process is automatic.
AlertItem::AlertItem(const AlertItem &cp) :
    d(new Internal::AlertItemPrivate(*cp.d))
{
    if (WarnAlertItemConstructionDestruction)
        qWarning() << "AlertItem(cp)" << d->_uid << d;
}

// The operator()=: we have to copy the content of the private part.
AlertItem &AlertItem::operator=(const AlertItem &cp)
{
    if (d) {
        // Avoid copying the same
        if (cp.d == d)
           return *this;
        delete d;
    }
    d = new Internal::AlertItemPrivate(*cp.d);
    if (WarnAlertItemConstructionDestruction)
        qWarning() << "AlertItem =()" << d->_uid << d;
    return *this;
}

AlertItem::~AlertItem()
{
    if (WarnAlertItemConstructionDestruction)
        qWarning() << "~AlertItem" << d->_uid << d;
    if (d)
        delete d;
    d = 0;
}

/** Store database identifiers. This part is protected and should only be used by Alert::Internal::AlertBase */
void AlertItem::setDb(int ref, const QVariant &value)
{
    if (!value.toString().isEmpty())
        d->_db.insert(ref, value);
}

/** Returns database identifiers. This part is protected and should only be used by Alert::Internal::AlertBase */
QVariant AlertItem::db(int ref) const
{
    return d->_db.value(ref, QVariant());
}

/** Define the validity of the item */
void AlertItem::setValidity(bool isValid)
{
    d->_valid = isValid;
}

/** Return the validity of the item */
bool AlertItem::isValid() const
{
    return d->_valid;
}

/** Return the modification state of the item */
bool AlertItem::isModified() const
{
    if (d->_modified)
        return true;
    // test all subclasses
    for(int i=0; i < d->_timings.count(); ++i) {
        if (d->_timings.at(i).isModified())
            return true;
    }
    for(int i=0; i < d->_relations.count(); ++i) {
        if (d->_relations.at(i).isModified())
            return true;
    }
    for(int i=0; i < d->_scripts.count(); ++i) {
        if (d->_scripts.at(i).isModified())
            return true;
    }
    for(int i=0; i < d->_validations.count(); ++i) {
        if (d->_relations.at(i).isModified())
            return true;
    }
    return false;
}

/** Define the modification state of the item and all its subitems (timings, relations, validations, scripts). */
void AlertItem::setModified(bool modified)
{
    d->_modified = modified;
    for(int i=0; i < d->_timings.count(); ++i) {
        d->_timings[i].setModified(modified);
    }
    for(int i=0; i < d->_relations.count(); ++i) {
        d->_relations[i].setModified(modified);
    }
    for(int i=0; i < d->_scripts.count(); ++i) {
        d->_scripts[i].setModified(modified);
    }
    for(int i=0; i < d->_validations.count(); ++i) {
        d->_validations[i].setModified(modified);
    }
}

/** Return the uuid of the item */
QString AlertItem::uuid() const
{
    return d->_uid;
}

/** Define the uuid of the item. By default, a uuid is defined in the constructor. */
void AlertItem::setUuid(const QString &uid) const
{
    d->_uid = uid;
}

/** Return the pacl uid of the item */
QString AlertItem::packUid() const
{
    return d->_packUid;
}

/** Define the pack uid of the item. */
void AlertItem::setPackUid(const QString &uid) const
{
    d->_packUid = uid;
}

/** Return the crypted password of the item. */
QString AlertItem::cryptedPassword() const
{
    return d->_pass;
}

/** Define the crypted password of the item. */
void AlertItem::setCryptedPassword(const QString &pass)
{
    d->_pass = pass;
}

/** Return the translated label of the item. */
QString AlertItem::label(const QString &lang) const
{
    Internal::AlertValueBook *v = d->getLanguage(lang);
    if (!v) {
        v = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!v) {
            v = d->getLanguage("en");
            if (!v)
                return QString::null;
        }
    }
    return v->_label;
}

/** Return the translated tooltip of the item. */
QString AlertItem::toolTip(const QString &lang) const
{
    Internal::AlertValueBook *v = d->getLanguage(lang);
    if (!v) {
        v = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!v) {
            v = d->getLanguage("en");
            if (!v)
                return QString::null;
        }
    }
    return v->_toolTip;
}

/** Return the translated category label of the item. */
QString AlertItem::category(const QString &lang) const
{
    Internal::AlertValueBook *v = d->getLanguage(lang);
    if (!v) {
        v = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!v) {
            v = d->getLanguage("en");
            if (!v)
                return QString::null;
        }
    }
    return v->_category;
}

/** Return the translated description of the item. */
QString AlertItem::description(const QString &lang) const
{
    Internal::AlertValueBook *v = d->getLanguage(lang);
    if (!v) {
        v = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!v) {
            v = d->getLanguage("en");
            if (!v)
                return QString::null;
        }
    }
    return v->_descr;
}

/** Return the translated comment of the item. */
QString AlertItem::comment(const QString &lang) const
{
    Internal::AlertValueBook *v = d->getLanguage(lang);
     if (!v) {
         v = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
         if (!v) {
             v = d->getLanguage("en");
             if (!v)
                 return QString::null;
         }
     }
     return v->_comment;
}

void AlertItem::setLabel(const QString &txt, const QString &lang)
{
    Internal::AlertValueBook *v = 0;
    QString l = lang;
    if (l.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    if (d->hasLanguage(l))
        v = d->getLanguage(l);
    else
        v = d->createLanguage(l);
    v->_label = txt;
}

void AlertItem::setToolTip(const QString &txt, const QString &lang)
{
    Internal::AlertValueBook *v = 0;
    QString l = lang;
    if (l.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    if (d->hasLanguage(l))
        v = d->getLanguage(l);
    else
        v = d->createLanguage(l);
    v->_toolTip= txt;
}

void AlertItem::setCategory(const QString &txt, const QString &lang)
{
    Internal::AlertValueBook *v = 0;
    QString l = lang;
    if (l.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    if (d->hasLanguage(l))
        v = d->getLanguage(l);
    else
        v = d->createLanguage(l);
    v->_category = txt;
}

void AlertItem::setDescription(const QString &txt, const QString &lang)
{
    Internal::AlertValueBook *v = 0;
    QString l = lang;
    if (l.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    if (d->hasLanguage(l))
        v = d->getLanguage(l);
    else
        v = d->createLanguage(l);
    v->_descr = txt;
}

void AlertItem::setComment(const QString &txt, const QString &lang)
{
    Internal::AlertValueBook *v = 0;
    QString l = lang;
    if (l.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    if (d->hasLanguage(l))
        v = d->getLanguage(l);
    else
        v = d->createLanguage(l);
    v->_comment = txt;
}

/** Return all the languages of the item. */
QStringList AlertItem::availableLanguages() const
{
    return d->languages();
}

/** Clear and remove all translatable values (label, description, category...) */
void AlertItem::removeAllLanguages()
{
    d->clear();
}

AlertItem::ViewType AlertItem::viewType() const
{
    return d->_viewType;
}

AlertItem::ContentType AlertItem::contentType() const
{
    return d->_contentType;
}

AlertItem::Priority AlertItem::priority() const
{
    return d->_priority;
}

QString AlertItem::priorityToString() const
{
    switch (d->_priority) {
    case High: return tkTr(Trans::Constants::HIGH);
    case Medium: return tkTr(Trans::Constants::MEDIUM);
    case Low: return tkTr(Trans::Constants::LOW);
    }
    return QString::null;
}

bool AlertItem::isOverrideRequiresUserComment() const
{
    return d->_overrideRequiresUserComment;
}

/** When alert is included in a blocking alert dialog with other alerts, setting the mustBeRead state ensure that user read the alert. */
bool AlertItem::mustBeRead() const
{
    return d->_mustBeRead;
}

/** Return true is the alert can be reminded later */
bool AlertItem::isRemindLaterAllowed() const
{
    return d->_remindAllowed;
}

bool AlertItem::isEditable() const
{
    return d->_editable;
}

void AlertItem::setViewType(AlertItem::ViewType type)
{
    d->_viewType = type;
}

void AlertItem::setContentType(AlertItem::ContentType content)
{
    d->_contentType = content;
}

void AlertItem::setPriority(AlertItem::Priority priority)
{
    d->_priority = priority;
}

void AlertItem::setOverrideRequiresUserComment(bool required)
{
    d->_overrideRequiresUserComment = required;
}

/** When alert is included in a blocking alert dialog with other alerts, setting the mustBeRead state ensure that user read the alert. */
void AlertItem::setMustBeRead(bool mustberead)
{
    d->_mustBeRead = mustberead;
}

void AlertItem::setRemindLaterAllowed(bool allowed)
{
    d->_remindAllowed = allowed;
}

void AlertItem::setEditable(bool editable)
{
    d->_editable = editable;
}

QDateTime AlertItem::creationDate() const
{
    return d->_creationDate;
}

void AlertItem::setCreationDate(const QDateTime &dt)
{
    d->_creationDate = QDateTime(dt.date(), QTime(dt.time().hour(), dt.time().minute(), dt.time().second()));
}

QDateTime AlertItem::lastUpdate() const
{
    return d->_update;
}

void AlertItem::setLastUpdate(const QDateTime &dt)
{
    d->_update = QDateTime(dt.date(), QTime(dt.time().hour(), dt.time().minute(), dt.time().second()));
}

QString AlertItem::themedIcon() const
{
    return d->_themedIcon;
}

void AlertItem::setThemedIcon(const QString &icon)
{
    d->_themedIcon = icon;
}

QString AlertItem::styleSheet() const
{
    return d->_css;
}

void AlertItem::setStyleSheet(const QString &css)
{
    d->_css = css;
}

/** Returns the background color related to the Alert::AlertItem::priority(). */
QString AlertItem::priorityBackgroundColor() const
{
    QString background;
    switch (d->_priority) {
    case AlertItem::Low: background = Core::Constants::COLOR_BACKGROUND_ALERT_LOW; break;
    case AlertItem::Medium: background = Core::Constants::COLOR_BACKGROUND_ALERT_MEDIUM; break;
    case AlertItem::High: background = Core::Constants::COLOR_BACKGROUND_ALERT_HIGH; break;
    }
    return background;
}

/** Return the icon corresponding to the \e priority. */
QIcon AlertItem::priorityBigIcon(Priority priority)
{
    QString icon;
    switch (priority) {
    case AlertItem::High: icon = Core::Constants::ICONCRITICAL; break;
    case AlertItem::Medium: icon = Core::Constants::ICONWARNING; break;
    case AlertItem::Low: icon = Core::Constants::ICONINFORMATION; break;
    }
    return theme()->icon(icon, Core::ITheme::BigIcon).pixmap(64,64);
}

/** Return the priority's icon corresponding of the alert. */
QIcon AlertItem::priorityBigIcon() const
{
    return priorityBigIcon(d->_priority);
}

static const char *CSS = "text-indent:5px;padding:5px;font-weight:bold;font-size:large;background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))";

QString AlertItem::htmlToolTip(bool showCategory) const
{
    QString toolTip;
    // category, label, priority
    QString header;
    if (showCategory)
        header = QString("<table border=0 margin=0 width=100%>"
                         "<tr>"
                         "<td valign=middle width=70% style=\"%1\">%2</td>"
                         "<td valign=middle align=center style=\"font-size:large;font-weight:bold;background-color:%4;text-transform:uppercase\">%5</td>"
                         "</tr>"
                         "<tr>"
                         "<td colspan=2 style=\"font-weight:bold;color:#101010;padding-left:10px\">%3</td>"
                         "</tr>"
                         "</table>")
                .arg(CSS)
                .arg(category())
                .arg(label())
                .arg(priorityBackgroundColor())
                .arg(priorityToString())
                ;
    else
        header = QString("<table border=0 margin=0 width=100%>"
                         "<tr>"
                         "<td valign=middle width=70% style=\"font-weight:bold\">%1</td>"
                         "<td valign=middle align=center style=\"font-size:large;font-weight:bold;background-color:%2;text-transform:uppercase\">%3</td>"
                         "</tr>"
                         "</table>")
                .arg(label())
                .arg(priorityBackgroundColor())
                .arg(priorityToString())
                ;

    QString descr;
    if (!description().isEmpty()) {
        descr += QString("<span style=\"color:black\">%1</span>"
                         "<hr align=center width=50% color=lightgray size=1>").arg(description());
    }

    QStringList related;
    for(int i = 0; i < relations().count(); ++i) {
        const AlertRelation &rel = relationAt(i);
        related += QString("%1").arg(rel.relationTypeToString());
    }

    QString content;
    if (!related.isEmpty())
        content += QString("<span style=\"font-size:small;color:#303030\">%1</span><br />").arg(related.join("<br />"));

    QStringList tim;
    for(int i =0; i < timings().count(); ++i) {
        AlertTiming &timing = timingAt(i);
        if (timing.isCycling()) {
            // TODO: create a AlertTiming::cycleDelayToString() and use it here
            tim << QString(QApplication::translate("Alert::AlertItem", "Started on: %1<br />Cycling every: %2<br />Expires on: %3"))
                       .arg(timing.currentCycleStartDate().toString(QLocale().dateFormat()))
                       .arg(timing.cyclingDelayInDays())
                       .arg(timing.currentCycleExpirationDate().toString(QLocale().dateFormat()));
        } else {
            tim << QString(QApplication::translate("Alert::AlertItem", "Started on: %1<br />Expires on: %2"))
                       .arg(timing.start().toString(QLocale().dateFormat()))
                       .arg(timing.expiration().toString(QLocale().dateFormat()));
        }
    }
    if (!tim.isEmpty())
        content += QString("<span style=\"font-size:small;color:#303030\">%1</span>").arg(tim.join("<br />"));

    // Alert pack description
    if (!packUid().isEmpty()) {
        AlertPackDescription pack = alertCore().getAlertPackDescription(packUid());
        content += QString("<span style=\"font-size:small;color:#303030\">%1</span>")
                .arg(tkTr(Trans::Constants::FROM) + " " + pack.label());
    }
    toolTip = QString("%1"
                      "<table border=0 cellpadding=0 cellspacing=0 width=100%>"
                      "<tr><td style=\"padding-left:10px;\">%2</td></tr>"
                      "<tr><td align=center>%3</td></tr>"
                      "</table>")
            .arg(header)
            .arg(descr)
            .arg(content)
            ;
    return toolTip;
}

QString AlertItem::extraXml() const
{
    return d->_extraXml;
}

void AlertItem::setExtraXml(const QString &xml)
{
    d->_extraXml = xml;
}

// TODO : void setCondition(...);

void AlertItem::clearRelations()
{
    d->_modified = true;
    d->_relations.clear();
}

AlertRelation &AlertItem::relation(int id) const
{
    for(int i=0; i<d->_relations.count();++i) {
        if (d->_relations.at(i).id()==id)
            return d->_relations[i];
    }
    return d->_nullRelation;
}

QVector<AlertRelation> &AlertItem::relations() const
{
    return d->_relations;
}

AlertRelation &AlertItem::relationAt(int id) const
{
    if (IN_RANGE_STRICT_MAX(id, 0, d->_relations.count()))
        return d->_relations[id];
    return d->_nullRelation;
}

void AlertItem::addRelation(const AlertRelation &relation)
{
    d->_modified = true;
    d->_relations << relation;
}

void AlertItem::clearTimings()
{
    d->_modified = true;
    d->_timings.clear();
}

AlertTiming &AlertItem::timing(int id) const
{
    for(int i=0; i<d->_timings.count();++i) {
        if (d->_timings.at(i).id()==id)
            return d->_timings[i];
    }
    return d->_nullTiming;
}

QVector<AlertTiming> &AlertItem::timings() const
{
    return d->_timings;
}

AlertTiming &AlertItem::timingAt(int id) const
{
    if (IN_RANGE_STRICT_MAX(id, 0, d->_timings.count()))
        return d->_timings[id];
    return d->_nullTiming;
}

void AlertItem::addTiming(const AlertTiming &timing)
{
    d->_modified = true;
    d->_timings.append(timing);
}

void AlertItem::clearScripts()
{
    d->_modified = true;
    d->_scripts.clear();
}

AlertScript &AlertItem::script(int id) const
{
    for(int i=0; i<d->_scripts.count();++i) {
        if (d->_scripts.at(i).id()==id)
            return d->_scripts[i];
    }
    return d->_nullScript;
}

AlertScript &AlertItem::scriptType(AlertScript::ScriptType type) const
{
    for(int i=0; i < d->_scripts.count(); ++i) {
        AlertScript &script = d->_scripts[i];
        if (script.type()==type)
            return script;
    }
    return d->_nullScript;
}

QVector<AlertScript> &AlertItem::scripts() const
{
    return d->_scripts;
}

AlertScript &AlertItem::scriptAt(int id) const
{
    if (IN_RANGE_STRICT_MAX(id, 0, d->_scripts.count()))
        return d->_scripts[id];
    return d->_nullScript;
}

void AlertItem::addScript(const AlertScript &script)
{
    d->_modified = true;
    d->_scripts << script;
}

void AlertItem::setScripts(const QVector<AlertScript> &scripts)
{
    d->_modified = true;
    d->_scripts.clear();
    d->_scripts = scripts;
}

/**
 * Removes the alert from the core.\n
 * Modification are not saved in the database.
 * \sa  Alert::AlertCore::removeAlert()
*/
bool AlertItem::setRemindLater()
{
    // move start date +1 day
//    QDate today = QDate::currentDate();
//    for(int i=0; i < d->_timings.count(); ++i) {
//        AlertTiming &timing = d->_timings[i];
//        if (timing.start().date() <= today) {
//            QDateTime from = timing.start();
//            QDateTime to = QDateTime(today.addDays(1), QTime(0,0,0));
//            timing.setStart(to);
//            timing.setExpiration(timing.expiration().addDays(from.daysTo(to)));
//        }
//    }
    // inform the core
    alertCore().removeAlert(*this);
    return true;
}

/**
  Validate an Alert::AlertItem with the current user. Return true if the alert was validated.\n
  The new state of the alert is not automatically saved into database, but
  the core is informed of this modification.
  \sa Alert::AlertCore::updateAlert()
*/
bool AlertItem::validateAlertWithCurrentUserAndConfirmationDialog()
{
    bool yes = Utils::yesNoMessageBox(
                QApplication::translate("Alert::AlertItem", "Alert validation."),
                QApplication::translate("Alert::AlertItem",
                                        "You are about to validate this alert:<br />"
                                        "<b>%1</b><br /><br />"
                                        "Do you really want to validate this alert ?")
                .arg(label()), "",
                QApplication::translate("Alert::AlertItem", "Alert validation."));
    if (yes) {
        QString validator;
        user() ? validator = user()->uuid() : validator = "UnknownUser";
        return validateAlert(validator, false, "", QDateTime::currentDateTime());
    }
    return false;
}

/**
  Validate an Alert::AlertItem with the \e validatorUid, define override state with \e override,
  define the \e overrideComment, at the \e dateOfValidation.
  Return true if the alert was validated.\n
  The new state of the alert is not automatically saved into database, but
  the core is informed of this modification. \sa Alert::AlertCore::updateAlert()
*/
bool AlertItem::validateAlert(const QString &validatorUid, bool override, const QString &overrideComment, const QDateTime &dateOfValidation)
{
    // Create the validation
    AlertValidation val;
    val.setDateOfValidation(QDateTime::currentDateTime());
    val.setValidatorUuid(validatorUid);
    val.setAccepted(!override);
    val.setOverriden(override);
    val.setUserComment(overrideComment);
    val.setDateOfValidation(dateOfValidation);

    // Get validated
    if (d->_relations.count()  > 0) {
        const AlertRelation &rel = d->_relations.at(0);
        switch (rel.relatedTo())
        {
        case AlertRelation::RelatedToPatient:
        case AlertRelation::RelatedToAllPatients:
        {
            if (patient())
                val.setValidatedUuid(patient()->uuid());
            else if (!Utils::isReleaseCompilation())
                val.setValidatedUuid("patient1");
            break;
        }
        case AlertRelation::RelatedToFamily: // TODO: manage family
            break;
        case AlertRelation::RelatedToUser:
        case AlertRelation::RelatedToAllUsers:
        {
            if (user())
                val.setValidatedUuid(user()->uuid());
            else if (!Utils::isReleaseCompilation())
                val.setValidatedUuid("user1");
            break;
        }
        case AlertRelation::RelatedToUserGroup: // TODO: manage user groups
            break;
        case AlertRelation::RelatedToApplication:
        {
            val.setValidatedUuid(qApp->applicationName().toLower());
            break;
        }
        }
    }
    addValidation(val);
    // inform the core
    alertCore().updateAlert(*this);
    return true;
}

/** Return true if the alert was validated by any user. */
bool AlertItem::isUserValidated() const
{
    if (d->_validations.count()==0)
        return false;

    if (d->_relations.count() > 0) {
        const AlertRelation &rel = d->_relations.at(0);
        switch (rel.relatedTo())
        {
        case AlertRelation::RelatedToPatient:
        case AlertRelation::RelatedToAllPatients:
        {
            if (patient())
                return d->validationsContainsValidatedUuid(patient()->uuid());
            else if (!Utils::isReleaseCompilation())
                return d->validationsContainsValidatedUuid("patient1");
            break;
        }
        case AlertRelation::RelatedToFamily: // TODO: manage family
            break;
        case AlertRelation::RelatedToUser:
        case AlertRelation::RelatedToAllUsers:
        {
            if (user())
                return d->validationsContainsValidatedUuid(user()->uuid());
            else if (!Utils::isReleaseCompilation())
                return d->validationsContainsValidatedUuid("user1");
            break;
        }
        case AlertRelation::RelatedToUserGroup: // TODO: manage user groups
            break;
        case AlertRelation::RelatedToApplication:
        {
            return d->validationsContainsValidatedUuid(qApp->applicationName().toLower());
        }
        }
    }
    LOG_ERROR_FOR("AlertItem", "No relation to link validation");
    return false;
}

/** Remove all recorded validations. */
void AlertItem::clearValidations()
{
    // TODO: check: clearValidations -> look at db (are all validations are removed ?)
    d->_modified = true;
    d->_validations.clear();
}

/** Return the validation according to its identifiant \sa AlertValidation::id(). */
AlertValidation &AlertItem::validation(int id) const
{
    for(int i=0; i<d->_validations.count();++i) {
        if (d->_validations.at(i).id()==id)
            return d->_validations[i];
    }
    return d->_nullValidation;
}

/** Return all the recorded validations. */
QVector<AlertValidation> &AlertItem::validations() const
{
    return d->_validations;
}

/** Return all the recorded validation at index \e id. */
AlertValidation &AlertItem::validationAt(int id) const
{
    if (IN_RANGE_STRICT_MAX(id, 0, d->_validations.count()))
        return d->_validations[id];
    return d->_nullValidation;
}

/** Add a validation to the alert. */
void AlertItem::addValidation(const AlertValidation &val)
{
    d->_modified = true;
    d->_validations << val;
}

/**
 * Check equality between two Alert::AlertItem. If item contains
 * Alert::AlertTiming, Alert::AlertRelation, Alert::AlertScript and/or
 * Alert::AlertValidation, their equality are also checked (including
 * their id()).
 */
bool AlertItem::operator==(const AlertItem &other) const
{
    // first test
    if (other.uuid() != uuid())
        return false;

    // second test
    if (other.cryptedPassword() != cryptedPassword() ||
            other.creationDate() != creationDate() ||
            other.lastUpdate() != lastUpdate() ||
            other.availableLanguages() != availableLanguages() ||
            other.viewType() != viewType() ||
            other.contentType() != contentType() ||
            other.priority() != priority() ||
            other.themedIcon() != themedIcon() ||
            other.styleSheet() != styleSheet() ||
            other.extraXml() != extraXml()
            )
        return false;

    // third test (same number of sub-classes)
    if (other.relations().count() != relations().count() ||
            other.scripts().count() != scripts().count() ||
            other.validations().count() != validations().count() ||
            other.timings().count() != timings().count()
            )
        return false;

    // test all timings
    for(int i = 0; i < d->_timings.count(); ++i) {
        const AlertTiming &first = d->_timings.at(i);
        bool ok = false;
        for(int j = 0; j < other.d->_timings.count(); ++j) {
            const AlertTiming &second = other.d->_timings.at(j);
            if (first == second) {
                ok = true;
                break;
            }
        }
        if (!ok)
            return false;
    }

    // test all relations
    for(int i = 0; i < d->_relations.count(); ++i) {
        const AlertRelation &first = d->_relations.at(i);
        bool ok = false;
        for(int j = 0; j < other.d->_relations.count(); ++j) {
            const AlertRelation &second = other.d->_relations.at(j);
            if (first == second) {
                ok = true;
                break;
            }
        }
        if (!ok)
            return false;
    }

    // test all validations
    for(int i = 0; i < d->_validations.count(); ++i) {
        const AlertValidation &first = d->_validations.at(i);
        bool ok = false;
        for(int j = 0; j < other.d->_validations.count(); ++j) {
            const AlertValidation &second = other.d->_validations.at(j);
            if (first == second) {
                ok = true;
                break;
            }
        }
        if (!ok)
            return false;
    }

    // test all scripts
    for(int i = 0; i < d->_scripts.count(); ++i) {
        const AlertScript &first = d->_scripts.at(i);
        bool ok = false;
        for(int j = 0; j < other.d->_scripts.count(); ++j) {
            const AlertScript &second = other.d->_scripts.at(j);
            if (first == second) {
                ok = true;
                break;
            }
        }
        if (!ok)
            return false;
    }
    return true;
}

bool AlertItem::operator!=(const AlertItem &other) const
{
    return !(operator==(other));
}

/** sort helpers */
bool AlertItem::priorityLowerThan(const AlertItem &item1, const AlertItem &item2)
{
    return item1.priority() < item2.priority();
}

/** sort helpers */
bool AlertItem::categoryLowerThan(const AlertItem &item1, const AlertItem &item2)
{
    return item1.category() < item2.category();
}

/** Transform to XML */
QString AlertItem::toXml() const
{
    // Feed description
    d->descr.setData(Internal::AlertXmlDescription::Uuid, d->_uid);
//    d->descr.setData(Internal::AlertXmlDescription::Version, );
//    d->descr.setData(Internal::AlertXmlDescription::Author, );
//    d->descr.setData(Internal::AlertXmlDescription::Country, );
//    d->descr.setData(Internal::AlertXmlDescription::URL, );
//    d->descr.setData(Internal::AlertXmlDescription::AbsFileName, );
//    d->descr.setData(Internal::AlertXmlDescription::Vendor, );
    d->descr.setData(Internal::AlertXmlDescription::Validity, d->_valid);
//    d->descr.setData(Internal::AlertXmlDescription::FreeMedFormsCompatVersion, );
//    d->descr.setData(Internal::AlertXmlDescription::FreeDiamsCompatVersion, );
    d->descr.setData(Internal::AlertXmlDescription::CreationDate, d->_creationDate);
    d->descr.setData(Internal::AlertXmlDescription::LastModificationDate, d->_update);
    d->descr.setData(Internal::AlertXmlDescription::CryptedPass, d->_pass);
    d->descr.setData(Internal::AlertXmlDescription::ViewType, d->viewTypeToXml());
    d->descr.setData(Internal::AlertXmlDescription::ContentType, d->contentTypeToXml());
    d->descr.setData(Internal::AlertXmlDescription::Priority, d->priorityToXml());
    d->descr.setData(Internal::AlertXmlDescription::OverrideRequiresUserComment, d->_overrideRequiresUserComment);
    d->descr.setData(Internal::AlertXmlDescription::MustBeRead, d->_mustBeRead);
    d->descr.setData(Internal::AlertXmlDescription::RemindLater, d->_remindAllowed);
    d->descr.setData(Internal::AlertXmlDescription::Editable, d->_editable);
    d->descr.setData(Internal::AlertXmlDescription::StyleSheet, d->_css);
    d->descr.setData(Internal::AlertXmlDescription::GeneralIcon, d->_themedIcon);
    d->descr.setData(Internal::AlertXmlDescription::PackUid, d->_packUid);

    foreach(const QString &l, availableLanguages()) {
        d->descr.setData(Internal::AlertXmlDescription::Label, label(l) , l);
        d->descr.setData(Internal::AlertXmlDescription::Category, category(l), l);
        d->descr.setData(Internal::AlertXmlDescription::ToolTip, toolTip(l), l);
//        d->descr.setData(Internal::AlertXmlDescription::Specialties, , l);
//        d->descr.setData(Internal::AlertXmlDescription::TypeName, , l);
        d->descr.setData(Internal::AlertXmlDescription::ShortDescription, description(l), l);
//        d->descr.setData(Internal::AlertXmlDescription::HtmlDescription, , l);
//        d->descr.setData(Internal::AlertXmlDescription::LicenseName, , l);
//        d->descr.setData(Internal::AlertXmlDescription::LicenseTerms, , l);
//        d->descr.setData(Internal::AlertXmlDescription::WebLink, , l);
        d->descr.setData(Internal::AlertXmlDescription::Comment, comment(l), l);
    }

    // Prepare xml code
    QString xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml += "<!DOCTYPE FreeMedForms>\n";
    xml += QString("<%1>\n").arg(::XML_ROOT_TAG);
    xml += d->descr.toXml();
    // Include timings
    int n = d->_timings.count();
    if (n) {
        xml += QString("<%1>\n").arg(::XML_TIMING_ROOTTAG);
        for(int i=0; i < n; ++i) {
            xml += d->_timings.at(i).toXml();
        }
        xml += QString("</%1>\n").arg(::XML_TIMING_ROOTTAG);
    }
    // Include related
    n = d->_relations.count();
    if (n) {
        xml += QString("<%1>\n").arg(::XML_RELATED_ROOTTAG);
        for(int i=0; i < n; ++i) {
            xml += d->_relations.at(i).toXml();
        }
        xml += QString("</%1>\n").arg(::XML_RELATED_ROOTTAG);
    }
    // Include scripts
    n = d->_scripts.count();
    if (n) {
        xml += QString("<%1>\n").arg(::XML_SCRIPT_ROOTTAG);
        for(int i=0; i < n; ++i) {
            xml += d->_scripts.at(i).toXml();
        }
        xml += QString("</%1>\n").arg(::XML_SCRIPT_ROOTTAG);
    }
    // Include validations
    n = d->_validations.count();
    if (n) {
        xml += QString("<%1>\n").arg(::XML_VALIDATION_ROOTTAG);
        for(int i=0; i < n; ++i) {
            xml += d->_validations.at(i).toXml();
        }
        xml += QString("</%1>\n").arg(::XML_VALIDATION_ROOTTAG);
    }
    // Include extra-xml
    if (!d->_extraXml.isEmpty())
        xml += QString("<%1>%2</%1>").arg(::XML_EXTRAXML_ROOTTAG).arg(d->_extraXml);
    // TODO: Include database values ?
    xml += QString("</%1>\n").arg(::XML_ROOT_TAG);
    QDomDocument doc;
    QString err;
    int l, c;
    if (!doc.setContent(xml, &err, &l, &c))
        LOG_ERROR_FOR("AlertItem", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(l).arg(c).arg(err));
    return doc.toString(2);
}

/** Create from XML */
AlertItem &AlertItem::fromXml(const QString &xml)
{
    AlertItem *item = new AlertItem();
    QDomDocument doc;
    QString err;
    int l, c;
    if (!doc.setContent(xml, &err, &l, &c)) {
        LOG_ERROR_FOR("AlertItem", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(l).arg(c).arg(err));
        return *item;
    }
    QDomElement root = doc.firstChildElement(::XML_ROOT_TAG);

    // Description
    item->d->descr.fromDomElement(root.firstChildElement(::XML_DESCRIPTION_ROOTTAG));
    item->d->feedItemWithXmlDescription();

    // Timings
    QDomElement main = root.firstChildElement(::XML_TIMING_ROOTTAG);
    if (!main.isNull()) {
        QDomElement element = main.firstChildElement(::XML_TIMING_ELEMENTTAG);
        while (!element.isNull()) {
            AlertTiming timing = AlertTiming::fromDomElement(element);
            item->addTiming(timing);
            element = element.nextSiblingElement(::XML_TIMING_ELEMENTTAG);
        }
    }

    // Relations
    main = root.firstChildElement(::XML_RELATED_ROOTTAG);
    if (!main.isNull()) {
        QDomElement element = main.firstChildElement(::XML_RELATED_ELEMENTTAG);
        while (!element.isNull()) {
            AlertRelation rel = AlertRelation::fromDomElement(element);
            item->addRelation(rel);
            element = element.nextSiblingElement(::XML_RELATED_ELEMENTTAG);
        }
    }

    // Validations
    main = root.firstChildElement(::XML_VALIDATION_ROOTTAG);
    if (!main.isNull()) {
        QDomElement element = main.firstChildElement(::XML_VALIDATION_ELEMENTTAG);
        while (!element.isNull()) {
            AlertValidation val = AlertValidation::fromDomElement(element);
            item->addValidation(val);
            element = element.nextSiblingElement(::XML_VALIDATION_ELEMENTTAG);
        }
    }

    // Scripts
    main = root.firstChildElement(::XML_SCRIPT_ROOTTAG);
    if (!main.isNull()) {
        QDomElement element = main.firstChildElement(::XML_SCRIPT_ELEMENTTAG);
        while (!element.isNull()) {
            AlertScript scr = AlertScript::fromDomElement(element);
            item->addScript(scr);
            element = element.nextSiblingElement(::XML_SCRIPT_ELEMENTTAG);
        }
    }

    // Extra-xml
    int begin = xml.indexOf(QString("<%1>").arg(::XML_EXTRAXML_ROOTTAG), Qt::CaseInsensitive);
    if (begin > 0) {
        begin = xml.indexOf("<", begin+2);
        int end = xml.indexOf(::XML_EXTRAXML_ROOTTAG, begin);
        end = xml.lastIndexOf(">", end) + 1;
        if (end > begin) {
            item->d->_extraXml = xml.mid(begin, end-begin);
        }
    }

    item->setModified(false);
    return *item;
}

AlertTiming::AlertTiming() :
    _id(-1), _nCycle(0), _currentCycle(-1),
    _delayInMins(0), _valid(true), _isCycle(false), _modified(false)
{}

AlertTiming::AlertTiming(const QDateTime &start, const QDateTime &expirationDate) :
    _id(-1), _nCycle(0), _currentCycle(-1),
    _start(QDateTime(start.date(), QTime(start.time().hour(), start.time().minute(), start.time().second()))),
    _end(QDateTime(expirationDate.date(), QTime(expirationDate.time().hour(), expirationDate.time().minute(), expirationDate.time().second()))),
    _delayInMins(0), _valid(true), _isCycle(false), _modified(true)
{}

AlertTiming::AlertTiming(const AlertTiming &copy) :
    _id(copy._id), _nCycle(copy._nCycle), _currentCycle(copy._currentCycle),
    _start(copy._start), _end(copy._end), _next(copy._next),
    _delayInMins(copy._delayInMins),
    _valid(copy._valid), _isCycle(copy._isCycle),
    _modified(copy._modified),
    _cycleStartDate(copy._cycleStartDate), _cycleExpirationDate(copy._cycleExpirationDate)
{}

void AlertTiming::cyclingDelay(qlonglong *mins, qlonglong *hours, qlonglong *days, qlonglong *weeks, qlonglong *months, qlonglong *years, qlonglong *decades) const
{
    qlonglong tmp = _delayInMins;
    *decades = cyclingDelayInDecades();
    tmp -= (*decades)*60*24*365.25*10;

    *years = tmp/60/24/365.25;
    tmp -= (*years)*60*24*365.25;

    *months = tmp/60/24/30;
    tmp -= (*months)*60*24*30;

    *weeks = tmp/60/24/7;
    tmp -= (*weeks)*60*24*7;

    *days = tmp/60/24;
    tmp -= (*days)*60/24;

    *hours = tmp/60;
    tmp -= (*hours)*60;

    *mins = tmp;
}

/**
  Computes the cycling delay in on unique period modulo. \n
  \e period correspond to the Trans::Constants::Time enumerator \n
  \e mod the number of times the period is repeated \n
  Example:
  \code
    AlertTiming time;
    time.setCycling(true);
    time.setCyclingDelayInWeeks(11);
    int period, mod;

    time.cyclingDelayPeriodModulo(&period, &mod);
    // period = Trans::Constants::Time::Weeks
    // mod = 11

    time.setCyclingDelayInMinutes(110880); // == 11 weeks
    time.cyclingDelayPeriodModulo(&period, &mod);
    // period = Trans::Constants::Time::Weeks
    // mod = 11

    time.setCyclingDelayInMinutes(110881); // == 11 weeks + 1Minutes
    time.cyclingDelayPeriodModulo(&period, &mod);
    // period = Trans::Constants::Time::Minutes
    // mod = 0
  \endcode
*/
void AlertTiming::cyclingDelayPeriodModulo(int *period, int *mod) const
{
    Q_ASSERT(period);
    Q_ASSERT(mod);
    if (!period || !mod)
        return;
    QList<int> ops;
    //     hours days     weeks      months      years                decade
    ops << 60 << 60*24 << 60*24*7 << 60*24*30 << int(60*24*365.25) << int(60*24*365.25*10);

    // find the natural corresponding multiple of a period
    *period = -1;
    *mod = 0;
    for(int i = 0; i < ops.count(); ++i) {
        if ((_delayInMins % ops.at(i)) == 0) {
            *period = i;
        }
    }
    if (*period != -1)
        *mod = _delayInMins / ops.at(*period);
    switch (*period) {
    case -1:
        *period = Trans::Constants::Time::Minutes;
        break;
    case 0:
        *period = Trans::Constants::Time::Hours;
        break;
    case 1:
        *period = Trans::Constants::Time::Days;
        break;
    case 2:
        *period = Trans::Constants::Time::Weeks;
        break;
    case 3:
        *period = Trans::Constants::Time::Months;
        break;
    case 4:
        *period = Trans::Constants::Time::Year;
        break;
    case 5:
        *period = Trans::Constants::Time::Decade;
        break;
    }
}

/**
 * Define the number of cycle of this cycling timing. When defining this,
 * current cycle is computed. Before, you must defined
 * - the start and expiration dates of the timing ;
 * - the delay in cycles.
 * \sa setStartDate(), setCyclingDelayInMinutes(), setCyclingDelayInHours()
 * \sa setCyclingDelayInDays(), setCyclingDelayInWeeks(), setCyclingDelayInMonth()
 * \sa setCyclingDelayInYears()
 */
void AlertTiming::setNumberOfCycles(int n)
{
    _modified=true;
    _nCycle=n;
    _isCycle=(n>0);
    computeCycle();
}

/** Compute cycle data range and check internal data */
void AlertTiming::computeCycle()
{
    // Is really cycling?
    if (!_isCycle || _nCycle <= 0 || _delayInMins <= 0) {
        _nCycle = 0;
        _isCycle = false;
        return;
    }

    // Already computed?
    if (_currentCycle > 0 &&
            _cycleStartDate.isValid() && !_cycleStartDate.isNull() &&
            _cycleExpirationDate.isValid() && !_cycleExpirationDate.isNull()) {
        return;
    }

    _cycleStartDate = QDateTime();
    _cycleExpirationDate = QDateTime();
    _currentCycle = 0;

    // Is computation possible?
    if (!_start.isValid() || _start.isNull()) {
        return;
    }

    // Compute current cycle
    _currentCycle = int((_start.secsTo(QDateTime::currentDateTime())/60) / _delayInMins);
    _cycleStartDate = _start.addSecs(_delayInMins*60*_currentCycle);
    _cycleExpirationDate = _start.addSecs(_delayInMins*60*(_currentCycle+1));
}

QString AlertTiming::toXml() const
{
    QDomDocument doc;
    QDomElement el = doc.createElement(::XML_TIMING_ELEMENTTAG);
    el.setAttribute("id", _id);
    el.setAttribute("valid", _valid ? "true" : "false");
    el.setAttribute("start", _start.toString(Qt::ISODate));
    el.setAttribute("end", _end.toString(Qt::ISODate));
    el.setAttribute("isCycle", _isCycle ? "true" : "false");
    if (_isCycle) {
        el.setAttribute("ncycle", _nCycle);
        el.setAttribute("delayInMin", _delayInMins);
        el.setAttribute("next", _next.toString(Qt::ISODate));
    }
    doc.appendChild(el);
    return doc.toString();
}

AlertTiming &AlertTiming::fromXml(const QString &xml)
{
    // Read the XML content
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(xml, &error, &line, &col)) {
        LOG_ERROR_FOR("AlertTiming", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(line).arg(col).arg(error));
        AlertTiming *t = new AlertTiming;
        return *t;
    }
    // Find the XML element corresponding to the script
    QDomElement el = doc.documentElement();
    if (el.tagName().compare(::XML_TIMING_ELEMENTTAG, Qt::CaseInsensitive) != 0)
        el = el.firstChildElement(::XML_TIMING_ELEMENTTAG);
    if (el.isNull()) {
        LOG_ERROR_FOR("AlertTiming", tkTr(Trans::Constants::XML_WRONG_NUMBER_OF_TAG_1).arg(::XML_TIMING_ELEMENTTAG));
        AlertTiming *t = new AlertTiming;
        return *t;
    }
    return fromDomElement(el);
}

AlertTiming &AlertTiming::fromDomElement(const QDomElement &element)
{
    AlertTiming *timing = new AlertTiming;
    if (element.tagName().compare(::XML_TIMING_ELEMENTTAG, Qt::CaseInsensitive)!=0)
        return *timing;
    if (!element.attribute("id").isEmpty())
        timing->setId(element.attribute("id").toInt());
    timing->setValid(element.attribute("valid").compare("true",Qt::CaseInsensitive)==0);
    if (!element.attribute("start").isEmpty())
        timing->setStart(QDateTime::fromString(element.attribute("start"), Qt::ISODate));
    if (!element.attribute("end").isEmpty())
        timing->setEnd(QDateTime::fromString(element.attribute("end"), Qt::ISODate));
    if (!element.attribute("next").isEmpty())
        timing->setNextDate(QDateTime::fromString(element.attribute("next"), Qt::ISODate));
    timing->setCycling(element.attribute("isCycle").compare("true",Qt::CaseInsensitive)==0);
    timing->setCyclingDelayInMinutes(element.attribute("delayInMin").toLongLong());
    timing->setNumberOfCycles(element.attribute("ncycle").toInt());
    timing->setModified(false);
    return *timing;
}

/** Compares two Alert::AlertTiming (including their id()) */
bool AlertTiming::operator==(const AlertTiming &other) const
{
//    if (_id != other._id) qWarning() << "!=_id";
//    if (_nCycle != other._nCycle) qWarning() << "!=_nCycle";
//    if (_start != other._start) qWarning() << "!=_start";
//    if (_end != other._end) qWarning() << "!=_end";
//    if (_next != other._next) qWarning() << "!=_next";
//    if (_delayInMins != other._delayInMins) qWarning() << "!=_delayInMins";
//    if (_valid != other._valid) qWarning() << "!=_valid";
//    if (_isCycle != other._isCycle) qWarning() << "!=_isCycle";
//    if (_modified != other._modified) qWarning() << "!=_modified";
    return _id == other._id &&
            _nCycle == other._nCycle &&
            _valid == other._valid &&
            _isCycle == other._isCycle &&
            _modified == other._modified &&
            _delayInMins == other._delayInMins &&
            _start == other._start &&
            _end == other._end &&
            _next == other._next;
    // currentCycle && cycleStartDate && cycleExpirationDate shouldn't be tested as they are computed values
}

/** Return a human readable \e type property */
QString AlertScript::typeToString(ScriptType type)
{
    switch (type) {
    case CheckValidityOfAlert: return QApplication::translate("Alert::AlertScript", "Check alert validity");
    case CyclingStartDate: return QApplication::translate("Alert::AlertScript", "Compute cycling starting date");
    case OnAboutToShow: return QApplication::translate("Alert::AlertScript", "About to show alert");
    case DuringAlert: return QApplication::translate("Alert::AlertScript", "During the alert presentation");
    case OnAboutToValidate: return QApplication::translate("Alert::AlertScript", "About to validate the alert");
    case OnAboutToOverride: return QApplication::translate("Alert::AlertScript", "On alert about to be overridden");
    case OnOverridden: return QApplication::translate("Alert::AlertScript", "On alert overridden");
    case OnPatientAboutToChange: return QApplication::translate("Alert::AlertScript", "On patient about to change");
    case OnUserAboutToChange: return QApplication::translate("Alert::AlertScript", "On user about to change");
    case OnEpisodeAboutToSave: return QApplication::translate("Alert::AlertScript", "On episode about to save");
    case OnEpisodeLoaded: return QApplication::translate("Alert::AlertScript", "On episode loaded");
    case OnRemindLater: return QApplication::translate("Alert::AlertScript", "On remind later requested");
    }

    return QString::null;
}

/** Return the XML content corresponding to the AlertScript::ScriptType \e type */
QString AlertScript::typeToXml(ScriptType type)
{
    switch (type) {
    case CheckValidityOfAlert: return "check";
    case CyclingStartDate: return "cyclingStartDate";
    case OnAboutToShow: return "onabouttoshow";
    case DuringAlert: return "during";
    case OnAboutToValidate: return "onabouttovalidate";
    case OnAboutToOverride: return "onabouttooverride";
    case OnOverridden: return "onoverride";
    case OnPatientAboutToChange: return "onpatientabouttochange";
    case OnUserAboutToChange: return "onuserabouttochange";
    case OnEpisodeAboutToSave: return "onepisodeabouttosave";
    case OnEpisodeLoaded: return "onepisodeloaded";
    case OnRemindLater: return "onremindlater";
    }
    return QString::null;
}

/** Return the AlertScript::ScriptType from XML content */
AlertScript::ScriptType AlertScript::typeFromXml(const QString &xml)
{
    if (xml.compare("check", Qt::CaseInsensitive)==0)
        return CheckValidityOfAlert;
    else if (xml.compare("cyclingStartDate", Qt::CaseInsensitive)==0)
        return CyclingStartDate;
    else if (xml.compare("onabouttoshow", Qt::CaseInsensitive)==0)
        return OnAboutToShow;
    else if (xml.compare("onabouttovalidate", Qt::CaseInsensitive)==0)
        return OnAboutToValidate;
    else if (xml.compare("during", Qt::CaseInsensitive)==0)
        return DuringAlert;
    else if (xml.compare("onabouttooverride", Qt::CaseInsensitive)==0)
        return OnAboutToOverride;
    else if (xml.compare("onoverride", Qt::CaseInsensitive)==0)
        return OnOverridden;
    else if (xml.compare("onpatientabouttochange", Qt::CaseInsensitive)==0)
        return OnPatientAboutToChange;
    else if (xml.compare("onuserabouttochange", Qt::CaseInsensitive)==0)
        return OnUserAboutToChange;
    else if (xml.compare("onepisodeabouttosave", Qt::CaseInsensitive)==0)
        return OnEpisodeAboutToSave;
    else if (xml.compare("onepisodeloaded", Qt::CaseInsensitive)==0)
        return OnEpisodeLoaded;
    else if (xml.compare("onremindlater", Qt::CaseInsensitive)==0)
        return OnRemindLater;
    return CheckValidityOfAlert;
}

/** Transform the script to XML */
QString AlertScript::toXml() const
{
    QDomDocument doc;
    QDomElement el = doc.createElement(::XML_SCRIPT_ELEMENTTAG);
    el.setAttribute("id", _id);
    el.setAttribute("valid", _valid ? "true" : "false");
    el.setAttribute("type", typeToXml(_type));
    el.setAttribute("uid", _uid);
    QDomText text = doc.createTextNode(_script);
    el.appendChild(text);
    doc.appendChild(el);
    return doc.toString();
}

/**
 * Create a script from XML
 * \sa fromDomElement(), toXml()
 */
AlertScript &AlertScript::fromXml(const QString &xml)
{
    // Read the XML content
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(xml, &error, &line, &col)) {
        LOG_ERROR_FOR("AlertScript", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(line).arg(col).arg(error));
        AlertScript *script = new AlertScript;
        return *script;
    }
    // Find the XML element corresponding to the script
    QDomElement el = doc.documentElement();
    if (el.tagName().compare(::XML_SCRIPT_ELEMENTTAG, Qt::CaseInsensitive) != 0)
        el = el.firstChildElement(::XML_SCRIPT_ELEMENTTAG);
    if (el.isNull()) {
        LOG_ERROR_FOR("AlertScript", tkTr(Trans::Constants::XML_WRONG_NUMBER_OF_TAG_1).arg(::XML_SCRIPT_ELEMENTTAG));
        AlertScript *script = new AlertScript;
        return *script;
    }
    return fromDomElement(el);
}

/**
 * Create a script from XML
 * \sa fromXml(), toXml()
 */
AlertScript &AlertScript::fromDomElement(const QDomElement &element)
{
    AlertScript *script = new AlertScript;
    if (element.tagName().compare(::XML_SCRIPT_ELEMENTTAG, Qt::CaseInsensitive)!=0)
        return *script;
    if (!element.attribute("id").isEmpty())
        script->setId(element.attribute("id").toInt());
    script->setUuid(element.attribute("uid"));
    script->setValid(element.attribute("valid").compare("true", Qt::CaseInsensitive) == 0);
    script->setType(typeFromXml(element.attribute("type")));
    script->setScript(element.text());
    script->setModified(false);
    return *script;
}

/** Sort scripts by type */
bool AlertScript::operator<(const AlertScript &script) const
{
    return this->type() < script.type();
}

/** Compares two Alert::AlertScript (including their id()) */
bool AlertScript::operator==(const AlertScript &other) const
{
//    if (_id != other._id) qWarning() << "!=_id";
//    if (_modified != other._modified) qWarning() << "!=_modified";
//    if (_valid != other._valid) qWarning() << "!=_valid";
//    if (_type != other._type) qWarning() << "!=_type";
//    if (_uid != other._uid) qWarning() << "!=_uid";
//    if (_script != other._script) qWarning() << "!=_script";
    return _id == other._id &&
            _modified == other._modified &&
            _valid == other._valid &&
            _type == other._type &&
            _uid == other._uid &&
            _script == other._script;
}

/** Transform the validation to XML */
QString AlertValidation::toXml() const
{
    QDomDocument doc;
    QDomElement el = doc.createElement(::XML_VALIDATION_ELEMENTTAG);
    el.setAttribute("id", _id);
    el.setAttribute("overridden", _overridden ? "true" : "false");
    el.setAttribute("validator", _validator);
    el.setAttribute("comment", _userComment);
    el.setAttribute("dt", _date.toString(Qt::ISODate));
    el.setAttribute("validated", _validated);
    doc.appendChild(el);
    return doc.toString();
}

/** Create a validation from XML */
AlertValidation &AlertValidation::fromXml(const QString &xml)
{
    // Read the XML content
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(xml, &error, &line, &col)) {
        LOG_ERROR_FOR("AlertValidation", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(line).arg(col).arg(error));
        AlertValidation *val = new AlertValidation();
        return *val;
    }
    // Find the XML element corresponding to the script
    QDomElement el = doc.documentElement();
    if (el.tagName().compare(::XML_VALIDATION_ELEMENTTAG, Qt::CaseInsensitive) != 0)
        el = el.firstChildElement(::XML_VALIDATION_ELEMENTTAG);
    if (el.isNull()) {
        LOG_ERROR_FOR("AlertValidation", tkTr(Trans::Constants::XML_WRONG_NUMBER_OF_TAG_1).arg(::XML_VALIDATION_ELEMENTTAG));
        AlertValidation *val = new AlertValidation();
        return *val;
    }
    return fromDomElement(el);
}

/** Create a validation from XML */
AlertValidation &AlertValidation::fromDomElement(const QDomElement &element)
{
    AlertValidation *val = new AlertValidation();
    if (element.tagName().compare(::XML_VALIDATION_ELEMENTTAG, Qt::CaseInsensitive)!=0)
        return *val;
    if (!element.attribute("id").isEmpty())
        val->setId(element.attribute("id").toInt());
    val->setOverriden(element.attribute("overridden").compare("true", Qt::CaseInsensitive)==0);
    val->setValidatorUuid(element.attribute("validator"));
    val->setUserComment(element.attribute("comment"));
    val->setValidatedUuid(element.attribute("validated"));
    val->setDateOfValidation(QDateTime::fromString(element.attribute("dt"), Qt::ISODate));
    val->setModified(false);
    return *val;
}

/** Compares two Alert::AlertValidation (including their id()) */
bool AlertValidation::operator==(const AlertValidation &other) const
{
//    if (_id != other._id) qWarning() << "!=_id";
//    if (_modified != other._modified) qWarning() << "!=_modified";
//    if (_overridden != other._overridden) qWarning() << "!=_overridden";
//    if (_validator != other._validator) qWarning() << "!=_validator";
//    if (_userComment != other._userComment) qWarning() << "!=_userComment";
//    if (_validated != other._validated) qWarning() << "!=_validated";
//    if (_date != other._date) qWarning() << "!=_date";
    return _id == other._id &&
            _modified == other._modified &&
            _overridden == other._overridden &&
            _validator == other._validator &&
            _userComment == other._userComment &&
            _validated == other._validated &&
            _date == other._date;
}

/** Return a human readable string of the current relation. */
QString AlertRelation::relationTypeToString() const
{
    switch (_related) {
    case RelatedToPatient:
    {
        if (patient()) {
            if (patient()->uuid().compare(_relatedUid, Qt::CaseInsensitive)==0) {
                return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_CURRENT_PATIENT));
            } else {
                return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_PATIENT_1)
                                                   .arg(patient()->fullPatientName(_relatedUid).value(_relatedUid)));
            }
        }
        return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_CURRENT_PATIENT));
    }
    case RelatedToFamily: return tkTr(Trans::Constants::RELATED_TO_PATIENT_FAMILY_1).arg("");
    case RelatedToAllPatients: return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_ALL_PATIENTS));
    case RelatedToUser:
    {
        if (user()) {
            if (user()->uuid().compare(_relatedUid, Qt::CaseInsensitive)==0) {
                return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_CURRENT_USER));
            } else {
                return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_USER_1)
                                                   .arg(user()->fullNameOfUser(_relatedUid)));
            }
        }
        return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_CURRENT_USER));
    }
    case RelatedToAllUsers: return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_ALL_USERS));
    case RelatedToUserGroup: return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_USER_GROUP_1).arg(""));
    case RelatedToApplication: return Utils::firstLetterUpperCase(tkTr(Trans::Constants::RELATED_TO_APPLICATION));
    }
    return QString::null;
}

/** Return a human readable string of the current relation. */
QString AlertRelation::relationTypeToXml(AlertRelation::RelatedTo rel) // static
{
    switch (rel) {
    case RelatedToPatient: return "patient";
    case RelatedToAllPatients: return "allPatients";
    case RelatedToFamily: return "family";
    case RelatedToUser: return "user";
    case RelatedToAllUsers: return "allUsers";
    case RelatedToUserGroup: return "userGroup";
    case RelatedToApplication: return "application";
    }
    return QString::null;
}

AlertRelation::RelatedTo AlertRelation::relationTypeFromXml(const QString &xmlValue) // static
{
    if (xmlValue.compare("patient", Qt::CaseInsensitive) == 0)
        return RelatedToPatient;
    else if (xmlValue.compare("allPatients", Qt::CaseInsensitive) == 0)
        return RelatedToAllPatients;
    else if (xmlValue.compare("family", Qt::CaseInsensitive) == 0)
        return RelatedToFamily;
    else if (xmlValue.compare("user", Qt::CaseInsensitive) == 0)
        return RelatedToUser;
    else if (xmlValue.compare("allUsers", Qt::CaseInsensitive) == 0)
        return RelatedToAllUsers;
    else if (xmlValue.compare("userGroup", Qt::CaseInsensitive) == 0)
        return RelatedToUserGroup;
    else if (xmlValue.compare("application", Qt::CaseInsensitive) == 0)
        return RelatedToApplication;
    return RelatedToApplication;
}

/** Transform the relation to XML */
QString AlertRelation::toXml() const
{
    QDomDocument doc;
    QDomElement el = doc.createElement(::XML_RELATED_ELEMENTTAG);
    el.setAttribute("id", _id);
    el.setAttribute("to", relationTypeToXml(_related));
    el.setAttribute("uid", _relatedUid);
    doc.appendChild(el);
    return doc.toString();
}

AlertRelation &AlertRelation::fromXml(const QString &xml)
{
    // Read the XML content
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(xml, &error, &line, &col)) {
        LOG_ERROR_FOR("AlertRelation", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
                      .arg(error).arg(line).arg(col));
        qWarning() << xml;
        AlertRelation *rel = new AlertRelation;
        return *rel;
    }
    // Find the XML element corresponding to the script
    QDomElement el = doc.documentElement();
    if (el.tagName().compare(::XML_RELATED_ELEMENTTAG, Qt::CaseInsensitive) != 0)
        el = el.firstChildElement(::XML_RELATED_ELEMENTTAG);
    if (el.isNull()) {
        LOG_ERROR_FOR("AlertRelation", tkTr(Trans::Constants::XML_WRONG_NUMBER_OF_TAG_1).arg(::XML_RELATED_ELEMENTTAG));
        AlertRelation *rel = new AlertRelation;
        return *rel;
    }
    return fromDomElement(el);
}

/** Create a relation from XML */
AlertRelation &AlertRelation::fromDomElement(const QDomElement &element)
{
    AlertRelation *rel = new AlertRelation;
    if (element.tagName().compare(::XML_RELATED_ELEMENTTAG, Qt::CaseInsensitive)!=0)
        return *rel;
    if (!element.attribute("id").isEmpty())
        rel->setId(element.attribute("id").toInt());
    rel->setRelatedTo(AlertRelation::relationTypeFromXml(element.attribute("to")));
    rel->setRelatedToUid(element.attribute("uid"));
    rel->setModified(false);
    return *rel;
}

/** Compares two Alert::AlertRelation (including their id()) */
bool AlertRelation::operator==(const AlertRelation &other) const
{
//    if (_id != other._id) qWarning() << "!=_id";
//    if (_modified != other._modified) qWarning() << "!=_modified";
//    if (_related != other._related) qWarning() << "!=_related";
//    if (_relatedUid != other._relatedUid) qWarning() << "!=_relatedUid";
    return _id == other._id &&
            _related == other._related &&
            _modified == other._modified &&
            _relatedUid == other._relatedUid;
}

QDebug operator<<(QDebug dbg, const Alert::AlertTiming *c)
{
    if (!c) {
        dbg.nospace() << "AlertTiming(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}

QDebug operator<<(QDebug dbg, const Alert::AlertTiming &a)
{
    QStringList s;
    s << QString("AlertTiming(%1; %2").arg(a.id()).arg(a.isValid()?"valid":"invalid");
    if (a.isModified())
        s << "modified";
    else
        s << "non-modified";
    s << QString("Start: %1").arg(a.start().toString(Qt::ISODate));
    s << QString("End: %1").arg(a.end().toString(Qt::ISODate));
    s << QString("Expiration: %1").arg(a.expiration().toString(Qt::ISODate));
    if (a.isCycling()) {
        s << "\n             cycling";
        s << QString("Delay(mins): %1").arg(a.cyclingDelayInMinutes());
        s << QString("NCycle: %1").arg(a.numberOfCycles());
        s << QString("currentCycleStart: %1").arg(a.currentCycleStartDate().toString(Qt::ISODate));
        s << QString("currentCycleExpiration: %1").arg(a.currentCycleExpirationDate().toString(Qt::ISODate));
        s << QString("currentCycle: %1").arg(a.currentCycle());
    }
    dbg.nospace() << s.join("; ")
                  << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Alert::AlertItem *c)
{
    if (!c) {
        dbg.nospace() << "AlertItem(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}

QDebug operator<<(QDebug dbg, const Alert::AlertItem &a)
{
    QStringList s;
    s << "AlertItem(" + a.uuid();
    if (a.isValid()) {
        if (a.isModified())
            s << "valid*";
        else
            s << "valid";
    } else {
        if (a.isModified())
            s << "notValid*";
        else
            s << "notValid";
    }
    switch (a.priority()) {
    case AlertItem::High:
        s << "high";
        break;
    case AlertItem::Medium:
        s << "medium";
        break;
    case AlertItem::Low:
        s << "low";
        break;
    }
    if (!a.cryptedPassword().isEmpty())
        s << "pass:" + a.cryptedPassword();
    s << "lbl:" + a.label();
    s << "cat:" + a.category();
    s << "availableLang:" + a.availableLanguages().join(";");
    switch (a.viewType()) {
    case AlertItem::BlockingAlert:
        s << "view:blocking";
        break;
    case AlertItem::NonBlockingAlert:
        s << "view:nonblocking";
        break;
    default:
        s << "view:" + QString::number(a.viewType());
        break;
    }
    switch (a.contentType()) {
    case AlertItem::ApplicationNotification:
        s << "content:appNotification";
        break;
    case AlertItem::PatientCondition:
        s << "content:patientCondition";
        break;
    case AlertItem::UserNotification:
        s << "content:userNotification";
        break;
    default:
        s << "content:" + QString::number(a.contentType());
        break;
    }
    s << "create:" + a.creationDate().toString(Qt::ISODate);
    s << QString::number(a.timingAt(0).cyclingDelayInMinutes());
    dbg.nospace() << s.join(",\n           ")
                  << ")";
    return dbg.space();
}

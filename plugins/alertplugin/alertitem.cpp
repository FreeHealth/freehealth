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
#include "alertitem.h"
#include "alertcore.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/genericdescription.h>
#include <translationutils/multilingualclasstemplate.h>
#include <translationutils/trans_datetime.h>

#include <QTreeWidgetItem>
#include <QDomDocument>

#include <QDebug>

enum { WarnAlertItemConstructionDestruction = false };

using namespace Alert;

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

const char * const XML_DATABASE_VALUES_TAG = "Alert";
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
        StyleSheet
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
        addNonTranslatableExtraData(StyleSheet, "styleSheet");
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
        _valid(true), _modified(false), _overrideRequiresUserComment(false),
        _viewType(AlertItem::StaticAlert),
        _contentType(AlertItem::ApplicationNotification),
        _priority(AlertItem::Medium),
        q(parent)
    {}

    ~AlertItemPrivate() {}

    QString viewTypeToXml()
    {
        switch (_viewType) {
        case AlertItem::DynamicAlert: return "dynamic";
        case AlertItem::StaticAlert: return "static";
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
        // default is dynamic alert
        _viewType = AlertItem::DynamicAlert;
        if (xml.compare("static", Qt::CaseInsensitive)==0) {
            _viewType = AlertItem::StaticAlert;
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
        _valid = descr.data(AlertXmlDescription::Validity).toInt();
        _overrideRequiresUserComment = descr.data(AlertXmlDescription::OverrideRequiresUserComment).toInt();
        viewTypeFromXml(descr.data(AlertXmlDescription::ViewType).toString());
        contentTypeFromXml(descr.data(AlertXmlDescription::ContentType).toString());
        priorityFromXml(descr.data(AlertXmlDescription::Priority).toString());
        _creationDate = QDateTime::fromString(descr.data(AlertXmlDescription::CreationDate).toString(), Qt::ISODate);
        _update = QDateTime::fromString(descr.data(AlertXmlDescription::LastModificationDate).toString(), Qt::ISODate);

        foreach(const QString &l, descr.availableLanguages()) {
            q->setLabel(descr.data(Internal::AlertXmlDescription::Label, l).toString(), l);
            q->setToolTip(descr.data(Internal::AlertXmlDescription::ToolTip, l).toString(), l);
            q->setCategory(descr.data(Internal::AlertXmlDescription::Category, l).toString(), l);
            q->setDescription(descr.data(Internal::AlertXmlDescription::ShortDescription, l).toString(), l);
            q->setComment(descr.data(Internal::AlertXmlDescription::Comment, l).toString(), l);
        }
    }

    QString categoryForTreeWiget() const {return QString::null;}

public:
    QString _uid, _pass, _themedIcon, _css, _extraXml;
    int _id;
    bool _valid, _modified, _overrideRequiresUserComment;
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
void AlertItem::operator=(const AlertItem &cp)
{
    if (d) {
        // Avoid copying the same
        if (cp.d == d)
           return;
        delete d;
    }
    d = new Internal::AlertItemPrivate(*cp.d);
    if (WarnAlertItemConstructionDestruction)
        qWarning() << "AlertItem =()" << d->_uid << d;
}

AlertItem::~AlertItem()
{
    if (WarnAlertItemConstructionDestruction)
        qWarning() << "~AlertItem" << d->_uid << d;
    if (d)
        delete d;
    d = 0;
}

/** Store database identifiants. This part is protected and should only be used by Alert::Internal::AlertBase */
void AlertItem::setDb(int ref, const QVariant &value)
{
    d->_db.insert(ref, value);
}

/** Returns database identifiants. This part is protected and should only be used by Alert::Internal::AlertBase */
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

QString AlertItem::cryptedPassword() const
{
    return d->_pass;
}

void AlertItem::setCryptedPassword(const QString &pass)
{
    d->_pass = pass;
}

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

bool AlertItem::isOverrideRequiresUserComment() const
{
    return d->_overrideRequiresUserComment;
}

// TODO : xxx condition() const = 0;

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

QDateTime AlertItem::creationDate() const
{
    return d->_creationDate;
}

void AlertItem::setCreationDate(const QDateTime &dt)
{
    d->_creationDate = dt;
}

QDateTime AlertItem::lastUpdate() const
{
    return d->_update;
}

void AlertItem::setLastUpdate(const QDateTime &dt)
{
    d->_update = dt;
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
    if (IN_RANGE(id, 0, d->_relations.count()))
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
    if (IN_RANGE(id, 0, d->_timings.count()))
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

QVector<AlertScript> &AlertItem::scripts() const
{
    return d->_scripts;
}

AlertScript &AlertItem::scriptAt(int id) const
{
    if (IN_RANGE(id, 0, d->_scripts.count()))
        return d->_scripts[id];
    return d->_nullScript;
}

void AlertItem::addScript(const AlertScript &script)
{
    d->_modified = true;
    d->_scripts << script;
}

void AlertItem::clearValidations()
{
    d->_modified = true;
    d->_validations.clear();
}

AlertValidation &AlertItem::validation(int id) const
{
    for(int i=0; i<d->_validations.count();++i) {
        if (d->_validations.at(i).id()==id)
            return d->_validations[i];
    }
    return d->_nullValidation;
}

QVector<AlertValidation> &AlertItem::validations() const
{
    return d->_validations;
}

AlertValidation &AlertItem::validationAt(int id) const
{
    if (IN_RANGE(id, 0, d->_validations.count()))
        return d->_validations[id];
    return d->_nullValidation;
}

void AlertItem::addValidation(const AlertValidation &val)
{
    d->_modified = true;
    d->_validations << val;
}

bool AlertItem::operator==(const AlertItem &other) const
{
    // first test
    if (other.uuid() != uuid())
        return false;
//    if (other.d->_db != d->_db)
//        return false;
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
    // fourth test: test each relations, validations, scripts and timings equality
    // TODO: test each relations, validations, scripts and timings equality
    return true;
}

bool AlertItem::operator!=(const AlertItem &other) const
{
    return !(operator==(other));
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
    case AlertItem::DynamicAlert:
        s << "view:dynamic";
        break;
    case AlertItem::StaticAlert:
        s << "view:staticAlert";
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
    d->descr.setData(Internal::AlertXmlDescription::Validity, int(d->_valid));
//    d->descr.setData(Internal::AlertXmlDescription::FreeMedFormsCompatVersion, );
//    d->descr.setData(Internal::AlertXmlDescription::FreeDiamsCompatVersion, );
//    d->descr.setData(Internal::AlertXmlDescription::FreeAccountCompatVersion, );
    d->descr.setData(Internal::AlertXmlDescription::CreationDate, d->_creationDate);
    d->descr.setData(Internal::AlertXmlDescription::LastModificationDate, d->_update);
    d->descr.setData(Internal::AlertXmlDescription::CryptedPass, d->_pass);
    d->descr.setData(Internal::AlertXmlDescription::ViewType, d->viewTypeToXml());
    d->descr.setData(Internal::AlertXmlDescription::ContentType, d->contentTypeToXml());
    d->descr.setData(Internal::AlertXmlDescription::Priority, d->priorityToXml());
    d->descr.setData(Internal::AlertXmlDescription::OverrideRequiresUserComment, d->_overrideRequiresUserComment);
    d->descr.setData(Internal::AlertXmlDescription::StyleSheet, d->_css);
    d->descr.setData(Internal::AlertXmlDescription::GeneralIcon, d->_themedIcon);

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
        LOG_ERROR_FOR("AlertItem", QString("XML Error (%1,%2): %3\n%4").arg(l).arg(c).arg(err).arg(xml));
    return doc.toString(2);
}

AlertItem AlertItem::fromXml(const QString &xml)
{
//    ::XML_DESCRIPTION_ROOTTAG;
    QDomDocument doc;
    QString err;
    int l, c;
    if (!doc.setContent(xml, &err, &l, &c)) {
        LOG_ERROR_FOR("AlertItem", QString("XML Error (%1,%2): %3\n%4").arg(l).arg(c).arg(err).arg(xml));
        return AlertItem();
    }
    AlertItem item;
    QDomElement root = doc.firstChildElement(::XML_ROOT_TAG);

    // Description
    item.d->descr.fromDomElement(root.firstChildElement(::XML_DESCRIPTION_ROOTTAG));
    item.d->feedItemWithXmlDescription();

    // Timings
    QDomElement main = root.firstChildElement(::XML_TIMING_ROOTTAG);
    if (!main.isNull()) {
        QDomElement element = main.firstChildElement(::XML_TIMING_ELEMENTTAG);
        while (!element.isNull()) {
            AlertTiming timing = AlertTiming::fromDomElement(element);
            item.addTiming(timing);
            element = element.nextSiblingElement(::XML_TIMING_ELEMENTTAG);
        }
    }

    // Relations
    main = root.firstChildElement(::XML_RELATED_ROOTTAG);
    if (!main.isNull()) {
        QDomElement element = main.firstChildElement(::XML_RELATED_ELEMENTTAG);
        while (!element.isNull()) {
            AlertRelation rel = AlertRelation::fromDomElement(element);
            item.addRelation(rel);
            element = element.nextSiblingElement(::XML_RELATED_ELEMENTTAG);
        }
    }

    // Validations
    main = root.firstChildElement(::XML_VALIDATION_ROOTTAG);
    if (!main.isNull()) {
        QDomElement element = main.firstChildElement(::XML_VALIDATION_ELEMENTTAG);
        while (!element.isNull()) {
            AlertValidation val = AlertValidation::fromDomElement(element);
            item.addValidation(val);
            element = element.nextSiblingElement(::XML_VALIDATION_ELEMENTTAG);
        }
    }

    // Scripts
    main = root.firstChildElement(::XML_SCRIPT_ROOTTAG);
    if (!main.isNull()) {
        QDomElement element = main.firstChildElement(::XML_SCRIPT_ELEMENTTAG);
        while (!element.isNull()) {
            AlertScript scr = AlertScript::fromDomElement(element);
            item.addScript(scr);
            element = element.nextSiblingElement(::XML_SCRIPT_ELEMENTTAG);
        }
    }

    // Extra-xml
    int begin = xml.indexOf(QString("<%1>").arg(::XML_EXTRAXML_ROOTTAG), Qt::CaseInsensitive);
    if (begin > 0) {
        begin += QString("<%1>").arg(::XML_EXTRAXML_ROOTTAG).length();
        int end = xml.indexOf(::XML_EXTRAXML_ROOTTAG, begin);
        if (end > begin) {
            --end;
            --end;
            item.d->_extraXml = xml.mid(begin, end-begin);
        }
    }
    return item;
}

void AlertTiming::cyclingDelay(qlonglong *mins, qlonglong *hours, qlonglong *days, qlonglong *weeks, qlonglong *months, qlonglong *years, qlonglong *decades) const
{
    qlonglong tmp = _delay;
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
        if ((_delay % ops.at(i)) == 0) {
            *period = i;
        }
    }
    if (*period != -1)
        *mod = _delay / ops.at(*period);
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

QString AlertTiming::toXml() const
{
    if (_isCycle)
        return QString("<%1 id='%2' valid='%3' start='%4' end='%5' "
                       "isCycle='%6' ncycle='%7' delayInMin='%8' next='%9'/>\n")
                .arg(::XML_TIMING_ELEMENTTAG)
                .arg(_id)
                .arg(_valid)
                .arg(_start.toString(Qt::ISODate))
                .arg(_end.toString(Qt::ISODate))
                .arg(_isCycle)
                .arg(_ncycle)
                .arg(QString::number(_delay))
                .arg(_next.toString(Qt::ISODate))
                ;
    return QString("<%1 id='%2' valid='%3' start='%4' end='%5'/>\n")
            .arg(::XML_TIMING_ELEMENTTAG)
            .arg(_id)
            .arg(_valid)
            .arg(_start.toString(Qt::ISODate))
            .arg(_end.toString(Qt::ISODate))
            ;
}

AlertTiming AlertTiming::fromDomElement(const QDomElement &element)
{
    if (element.tagName().compare(::XML_TIMING_ELEMENTTAG, Qt::CaseInsensitive)!=0)
        return AlertTiming();
    AlertTiming timing;
    timing.setId(element.attribute("id").toInt());
    timing.setValid(element.attribute("valid").toInt());
    timing.setStart(QDateTime::fromString(element.attribute("start"), Qt::ISODate));
    timing.setEnd(QDateTime::fromString(element.attribute("end"), Qt::ISODate));
    timing.setCycling(element.attribute("isCycle").toInt());
    timing.setNumberOfCycles(element.attribute("ncycle").toInt());
    timing.setCyclingDelayInMinutes(element.attribute("delayInMin").toLongLong());
    timing.setNextDate(QDateTime::fromString(element.attribute("next"), Qt::ISODate));
    return timing;
}

QString AlertScript::toXml() const
{
    // TODO: manage "<" in script
    return QString("<%1 id='%2' valid='%3' type='%4' uid='%5'>\n%6\n</%1>\n")
            .arg(::XML_SCRIPT_ELEMENTTAG)
            .arg(_id)
            .arg(_valid)
            .arg(_type)
            .arg(_uid)
            .arg(_script)
            ;
}

AlertScript AlertScript::fromDomElement(const QDomElement &element)
{
    if (element.tagName().compare(::XML_SCRIPT_ELEMENTTAG, Qt::CaseInsensitive)!=0)
        return AlertScript();
    AlertScript script;
    script.setId(element.attribute("id").toInt());
    script.setUuid(element.attribute("uid"));
    script.setValid(element.attribute("valid").toInt());
    script.setType(AlertScript::ScriptType(element.attribute("type").toInt()));
    script.setScript(element.text());
    return script;
}

QString AlertValidation::toXml() const
{
    QString comment = _userComment;
    comment = comment.replace("<", "&lt;");
    return QString("<%1 id='%2' user='%3' comment='%4' dt='%5'/>\n")
            .arg(::XML_VALIDATION_ELEMENTTAG)
            .arg(_id)
            .arg(_userUid)
            .arg(comment)
            .arg(_date.toString(Qt::ISODate))
            ;
}

AlertValidation AlertValidation::fromDomElement(const QDomElement &element)
{
    if (element.tagName().compare(::XML_VALIDATION_ELEMENTTAG, Qt::CaseInsensitive)!=0)
        return AlertValidation();
    AlertValidation val;
    val.setId(element.attribute("id").toInt());
    val.setUserUuid(element.attribute("user"));
    val.setUserComment(element.attribute("comment"));
    val.setDateOfValidation(QDateTime::fromString(element.attribute("dt"), Qt::ISODate));
    return val;
}

QString AlertRelation::toXml() const
{
    if (_relatedUid.isEmpty())
        return QString("<%1 id='%2' to='%3'/>\n")
                .arg(::XML_RELATED_ELEMENTTAG)
                .arg(_id)
                .arg(_related)
                ;
    return QString("<%1 id='%2' to='%3' uid='%4'/>\n")
            .arg(::XML_RELATED_ELEMENTTAG)
            .arg(_id)
            .arg(_related)
            .arg(_relatedUid)
            ;
}

AlertRelation AlertRelation::fromDomElement(const QDomElement &element)
{
    if (element.tagName().compare(::XML_RELATED_ELEMENTTAG, Qt::CaseInsensitive)!=0)
        return AlertRelation();
    AlertRelation rel;
    rel.setId(element.attribute("id").toInt());
    rel.setRelatedTo(AlertRelation::RelatedTo(element.attribute("to").toInt()));
    rel.setRelatedToUid(element.attribute("uid"));
    return rel;
}

QDebug operator<<(QDebug dbg, const Alert::AlertItem *c)
{
    return operator<<(dbg, *c);
}

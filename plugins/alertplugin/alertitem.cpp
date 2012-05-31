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

#include <translationutils/multilingualclasstemplate.h>

#include <QTreeWidgetItem>

#include <QDebug>

using namespace Alert;

namespace Alert {
namespace Internal {

class AlertValueBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l) const
    {
        Q_UNUSED(l);
    }

public:
    QString _label, _category, _descr, _comment;
};

class AlertItemPrivate : public Trans::MultiLingualClass<AlertValueBook>
{
public:
    AlertItemPrivate() :
        _id(-1),
        _valid(true), _modified(false), _overrideRequiresUserComment(false),
        _viewType(AlertItem::StaticPatientBar),
        _contentType(AlertItem::ApplicationNotification),
        _priority(AlertItem::Medium)
    {}

    ~AlertItemPrivate() {}

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
};

}
}

AlertItem::AlertItem() :
    d(new Internal::AlertItemPrivate)
{}

// The copy constructor is required for the QVector/QList and others.
// We have to copy the content of the private part.
// The multilingual class does not store pointers so the copy process is automatic.
AlertItem::AlertItem(const AlertItem &cp) :
  d(new Internal::AlertItemPrivate(*cp.d))
{
}

AlertItem::~AlertItem()
{
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
    return d->_modified;
}

/** Define the modification state of the item */
void AlertItem::setModified(bool modified)
{
    d->_modified = modified;
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
    if (d->hasLanguage(lang))
        v = d->getLanguage(lang);
    else
        v = d->createLanguage(lang);
    v->_label = txt;
}

void AlertItem::setCategory(const QString &txt, const QString &lang)
{
    Internal::AlertValueBook *v = 0;
    if (d->hasLanguage(lang))
        v = d->getLanguage(lang);
    else
        v = d->createLanguage(lang);
    v->_category = txt;
}

void AlertItem::setDescription(const QString &txt, const QString &lang)
{
    Internal::AlertValueBook *v = 0;
    if (d->hasLanguage(lang))
        v = d->getLanguage(lang);
    else
        v = d->createLanguage(lang);
    v->_descr = txt;
}

void AlertItem::setComment(const QString &txt, const QString &lang)
{
    Internal::AlertValueBook *v = 0;
    if (d->hasLanguage(lang))
        v = d->getLanguage(lang);
    else
        v = d->createLanguage(lang);
    v->_comment = txt;
}

QStringList AlertItem::availableLanguages() const
{
    return d->languages();
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
    if (id>0 && id<d->_relations.count())
        return d->_relations[id];
    return d->_nullRelation;
}

void AlertItem::addRelation(const AlertRelation &relation)
{
    d->_relations << relation;
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
    if (id>0 && id<d->_timings.count())
        return d->_timings[id];
    return d->_nullTiming;
}

void AlertItem::addTiming(const AlertTiming &timing)
{
    d->_timings << timing;
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
    if (id>0 && id<d->_scripts.count())
        return d->_scripts[id];
    return d->_nullScript;
}

void AlertItem::addScript(const AlertScript &script)
{
    d->_scripts << script;
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
    if (id>0 && id<d->_validations.count())
        return d->_validations[id];
    return d->_nullValidation;
}

void AlertItem::addValidation(const AlertValidation &val)
{
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
    dbg.nospace() << s.join(",\n           ")
                  << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Alert::AlertItem *c)
{
    return operator<<(dbg, *c);
}

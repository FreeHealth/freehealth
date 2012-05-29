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

using namespace Alert;

namespace Alert {
namespace Internal {

class AlertValueBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l)
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
        _valid(true),
        _viewType(AlertItem::StaticPatientBar),
        _contentType(AlertItem::ApplicationNotification),
        _priority(AlertItem::Medium)
    {}

    ~AlertItemPrivate() {}

    QString categoryForTreeWiget() const {return QString::null;}

public:
    QString _uid;
    int _id;
    bool _valid;
    AlertItem::ViewType _viewType;
    AlertItem::ContentType _contentType;
    AlertItem::Priority _priority;

    // TODO : move this in an AlertModel
    QVector<AlertScript> _scripts;
    QVector<AlertTiming> _timings;
    QVector<AlertValidation> _validations;
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

AlertItem::~AlertItem()
{
    if (d)
        delete d;
    d = 0;
}

bool AlertItem::setValidity(bool isValid)
{
    d->_valid = isValid;
}

bool AlertItem::isValid() const
{
    return d->_valid;
}

QString AlertItem::uuid() const
{
    return d->_uid;
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

// TODO : void setCondition(...);

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

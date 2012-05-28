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
#include "ialert.h"
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
    QVector<AlertScript> _scripts;
    QVector<AlertTiming> _timings;
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

bool AlertItem::isValid() const
{}


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
{}

QVector<AlertTiming> &AlertItem::timings() const
{}

AlertTiming &AlertItem::timingAt(int id) const
{}

void AlertItem::addTiming(const AlertTiming &timing)
{}

AlertScript &AlertItem::script(int id) const
{}

QVector<AlertScript> &AlertItem::scripts() const
{}

AlertScript &AlertItem::scriptAt(int id) const
{}

void AlertItem::addScript(const AlertScript &script)
{}


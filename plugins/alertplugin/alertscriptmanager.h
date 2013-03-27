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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERT_INTERNAL_ALERTSCRIPTMANAGER_H
#define ALERT_INTERNAL_ALERTSCRIPTMANAGER_H

#include <alertplugin/alertitem.h>

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QStringList>
QT_BEGIN_NAMESPACE
class QScriptEngine;
QT_END_NAMESPACE

/**
 * \file alertscriptmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 08 July 2012
*/

namespace Alert {
class AlertCore;
namespace Internal {
class AlertItemScriptWrapper;

class AlertScriptManager : public QObject
{
    friend class Alert::AlertCore;

protected:
    AlertScriptManager(QObject *parent = 0);

public:
    // type is a AlertScript::ScriptType
    QVariant execute(AlertItem &item, const int scriptType);

private:
    AlertItemScriptWrapper *_wrapper;
    QScriptEngine *_test;
};


class AlertItemScriptWrapper : public QObject
{
    Q_OBJECT
public:
    AlertItemScriptWrapper(AlertItem &item, QObject *parent = 0) :
        QObject(parent), _item(item)
    {}
    ~AlertItemScriptWrapper() {}

public Q_SLOTS:
    QString label(const QString &lang = QString::null) const {return _item.label(lang);}
    QString toolTip(const QString &lang = QString::null) const {return _item.toolTip(lang);}
    QString category(const QString &lang = QString::null) const {return _item.category(lang);}
    QString description(const QString &lang = QString::null) const {return _item.description(lang);}
    QString comment(const QString &lang = QString::null) const {return _item.comment(lang);}

    void setLabel(const QString &txt, const QString &lang = QString::null) {_item.setLabel(txt, lang);}
    void setToolTip(const QString &txt, const QString &lang = QString::null) {_item.setToolTip(txt, lang);}
    void setCategory(const QString &txt, const QString &lang = QString::null) {_item.setCategory(txt, lang);}
    void setDescription(const QString &txt, const QString &lang = QString::null) {_item.setDescription(txt, lang);}
    void setComment(const QString &txt, const QString &lang = QString::null) {_item.setComment(txt, lang);}

    QStringList availableLanguages() const {return _item.availableLanguages();}

    int viewType() const {return _item.viewType();}
    int contentType() const {return _item.contentType();}
    int priority() const {return _item.priority();}
    QString priorityToString() const {return _item.priorityToString();}
    bool isOverrideRequiresUserComment() const {return _item.isOverrideRequiresUserComment();}
    bool mustBeRead() const {return _item.mustBeRead();}
    bool isRemindLaterAllowed() const {return _item.isRemindLaterAllowed();}
    bool isEditable() const {return _item.isEditable();}

    void setViewType(int type) {_item.setViewType(AlertItem::ViewType(type));}
    void setContentType(int content) {_item.setContentType(AlertItem::ContentType(content));}
    void setPriority(int priority) {_item.setPriority(AlertItem::Priority(priority));}
    void setOverrideRequiresUserComment(bool required) {_item.setOverrideRequiresUserComment(required);}
    void setMustBeRead(bool mustberead) {_item.setMustBeRead(mustberead);}
    void setRemindLaterAllowed(bool allowed) {_item.setRemindLaterAllowed(allowed);}
    void setEditable(bool editable) {_item.setEditable(editable);}

    QDateTime creationDate() const {return _item.creationDate();}
    QString themedIcon() const {return _item.themedIcon();}
    void setThemedIcon(const QString &icon) {_item.setThemedIcon(icon);}

    bool remindLater() {return _item.remindLater();}
    bool validateAlertWithCurrentUserAndConfirmationDialog() {return _item.validateAlertWithCurrentUserAndConfirmationDialog();}
    bool validateAlert(const QString &validatorUid, bool override, const QString &overrideComment, const QDateTime &dateOfValidation) {return _item.validateAlert(validatorUid, override, overrideComment, dateOfValidation);}

    bool isUserValidated() const {return _item.isUserValidated();}
    void clearValidations() {_item.clearValidations();}

private:
    AlertItem &_item;
};

} // namespace Internal
} // namespace Alert

#endif // ALERT_INTERNAL_ALERTSCRIPTMANAGER_H

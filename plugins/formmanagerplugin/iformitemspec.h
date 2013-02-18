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
#ifndef IFORMITEMSPEC_H
#define IFORMITEMSPEC_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <coreplugin/idebugpage.h>

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QWidget>
#include <QVariant>
#include <QPointer>
#include <QHash>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

/**
 * \file iformitem.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 11 Sept 2012
*/

namespace Form {
namespace Internal{
class FormItemSpecPrivate;
}

class FORM_EXPORT FormItemSpec
{
public:
    enum SpecData {
        Spec_Uuid = 0,
        Spec_Author,
        Spec_License,
        Spec_Category,
        Spec_CreationDate,
        Spec_LastModified,
        Spec_Version,
        Spec_Description,
        Spec_Bibliography,
        Spec_Label,
        Spec_ExtraLabel,
        Spec_Priority,
        Spec_Plugin,
        Spec_UiFileContent, // only used for FormMain (non empty roots)
        Spec_UiLabel,
        Spec_UiWidget,
        Spec_UiInsertIntoLayout,
        Spec_HtmlPrintMask,
        Spec_PlainTextPrintMask,
        Spec_IconFileName,
        Spec_Tooltip,
        Spec_PlaceHolder,
        Spec_IsIdentityForm
    };
    FormItemSpec();
    ~FormItemSpec();

    QString uuid() const;
    void setUuid(const QString &uuid);

    void setValue(int type, const QVariant &val, const QString &lang = QString::null);
    QVariant value(const int type, const QString &lang = QString::null) const;

    QString author(const QString &lang = QString::null) const { return value(Spec_Author,lang).toString(); }
    QString license(const QString &lang = QString::null) const {return value(Spec_License,lang).toString();}
    QString category(const QString &lang = QString::null) const { return value(Spec_Category,lang).toString();}
    QDateTime creationDate(const QString &lang = QString::null) const {return value(Spec_CreationDate,lang).toDateTime();}
    QDateTime modificationDate(const QString &lang = QString::null) const {return value(Spec_LastModified,lang).toDateTime();}
    QString version(const QString &lang = QString::null) const {return value(Spec_Version,lang).toString();}
    QString description(const QString &lang = QString::null) const {return value(Spec_Description,lang).toString();}
    QString bibliography(const QString &lang = QString::null) const {return value(Spec_Bibliography,lang).toString();}
    QString label(const QString &lang = QString::null) const {return value(Spec_Label,lang).toString();}
    QString extraLabel(const QString &lang = QString::null) const {return value(Spec_ExtraLabel,lang).toString();}
    QString pluginName(const QString &lang = QString::null) const {return value(Spec_Plugin,lang).toString();}
    QString iconFileName(const QString &lang = QString::null) const {return value(Spec_IconFileName,lang).toString();}
    QString tooltip(const QString &lang = QString::null) const {return value(Spec_Tooltip,lang).toString();}
    QString placeHolder(const QString &lang = QString::null) const {return value(Spec_PlaceHolder,lang).toString();}

    void setEquivalentUuid(const QStringList &list);
    QStringList equivalentUuid() const;

    void toTreeWidget(QTreeWidgetItem *tree) const;

private:
    Form::Internal::FormItemSpecPrivate *d;
};


} // end Form

#endif // IFORMITEMSPEC_H

/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IFORMITEMSCRIPTS_H
#define IFORMITEMSCRIPTS_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <translationutils/constants.h>

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
 * \version 0.0.8
 * \date 27 Aug 2009
*/

namespace Form {

namespace Internal{
class FormItemScriptsPrivate;
}

class FORM_EXPORT FormItemScripts
{
public:
    enum TypeOfScript {
        Script_OnLoad = 0,
        Script_PostLoad,
        Script_OnDemand,
        Script_OnValueChanged,
        Script_OnValueRequiered,
        Script_OnDependentValueChanged,
        Script_OnClicked

    };


    FormItemScripts(const QString &lang = Trans::Constants::ALL_LANGUAGE,
                    const QString &onLoad = QString::null,
                    const QString &postLoad = QString::null,
                    const QString &onDemand = QString::null,
                    const QString &onValChanged = QString::null,
                    const QString &onValRequiered = QString::null,
                    const QString &onDependentValuesChanged = QString::null
                    );
    ~FormItemScripts();

    void setScript(const int type, const QString &script, const QString &lang = Trans::Constants::ALL_LANGUAGE);
    QString script(const int type, const QString &lang = Trans::Constants::ALL_LANGUAGE) const;

    void warn() const;

    QString onLoadScript() const {return script(Script_OnLoad);}
    QString postLoadScript() const {return script(Script_PostLoad);}
    QString onDemandScript() const {return script(Script_OnDemand);}
    QString onValueChangedScript() const {return script(Script_OnValueChanged);}
    QString onValueRequieredScript() const {return script(Script_OnValueRequiered);}
    QString onDependentValueChangedScript() const {return script(Script_OnDependentValueChanged);}
    QString onClicked() const {return script(Script_OnClicked);}

    void toTreeWidget(QTreeWidgetItem *tree);
private:
    Internal::FormItemScriptsPrivate *d;
};


} // end Form

#endif // IFORMITEMSCRIPTS_H

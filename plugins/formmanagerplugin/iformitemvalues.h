/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef IFORMITEMVALUES_H
#define IFORMITEMVALUES_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <coreplugin/idebugpage.h>
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
 * \file iformitemvalues.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 04 Apr 2011
*/

namespace Form {

namespace Internal {
class FormItemValuesPrivate;
}

class FORM_EXPORT FormItemValues //: public QObject
{
//    Q_OBJECT
public:
    enum {
        Value_Uuid = 0,
        Value_Possible,
        Value_Script,
        Value_Numerical,
        Value_Default,
        Value_Printing,
        Value_Dependency
    };
//    FormItemValues(QObject *parent);
    FormItemValues();
    ~FormItemValues();

    void setValue(int type, const int id, const QVariant &val, const QString &language = QString::null);
    void setDefaultValue(const QVariant &val, const QString &lang = QString::null);
    QVariant defaultValue(const QString &lang = QString::null) const;

    bool isOptional() const;
    void setOptional(bool state);

    QStringList values(const int typeOfValues) const;

    // filename
    void setFileName(const QString &fileName) {m_FileName=fileName;}
    QString fileName() const {return m_FileName;}

    void toTreeWidget(QTreeWidgetItem *tree) const;

private:
    Internal::FormItemValuesPrivate *d;
    QString m_FileName;
};

} // end Form

#endif // IFORMITEMVALUES_H

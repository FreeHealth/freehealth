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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERTPACKDESCRIPTION_H
#define ALERTPACKDESCRIPTION_H

#include <alertplugin/alertplugin_exporter.h>
#include <utils/genericdescription.h>

#include <QHash>
#include <QVariant>

namespace Alert {
namespace Internal {
class AlertBase;
}
class ALERT_EXPORT AlertPackDescription : public Utils::GenericDescription
{
    friend class Alert::Internal::AlertBase;

public:
    enum nonTrData {
        InUse = Utils::GenericDescription::NonTranslatableExtraData + 1
    };

    AlertPackDescription();

    QString uid() const {return data(Utils::GenericDescription::Uuid).toString();}

    void setInUse(bool inUse) {setData(InUse, inUse);}
    bool inUse() const {return data(InUse).toBool();}

    QString label(const QString &lang = QString::null) const {return data(Utils::GenericDescription::Label, lang).toString();}
    QString category(const QString &lang = QString::null) const {return data(Utils::GenericDescription::Category, lang).toString();}
    QString description(const QString &lang = QString::null) const {return data(Utils::GenericDescription::HtmlDescription, lang).toString();}

    void setLabel(const QString &txt, const QString &lang = QString::null) {setData(Utils::GenericDescription::Label, txt, lang);}
    void setCategory(const QString &txt, const QString &lang = QString::null) {setData(Utils::GenericDescription::Category, txt, lang);}
    void setDescription(const QString &txt, const QString &lang = QString::null) {setData(Utils::GenericDescription::HtmlDescription, txt, lang);}

protected:
    void setDbData(const int ref, const QVariant &data) {_dbData.insert(ref, data);}
    QVariant dbData(const int ref) {return _dbData.value(ref, QVariant());}

private:
    QHash<int, QVariant> _dbData;
};

} // namespace Alert

#endif // ALERTPACKDESCRIPTION_H

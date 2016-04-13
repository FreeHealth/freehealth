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
 *   Main developers : Eric Maeker                                         *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_INTERNAL_FORMITEMTOKEN_H
#define FORM_INTERNAL_FORMITEMTOKEN_H

#include <coreplugin/ipadtools.h>
#include <formmanagerplugin/iformitem.h>

/**
 * \file formitemtoken.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 30 Apr 2013
*/

namespace Form {
namespace Internal {
class FormItemTokenPrivate;

class FormItemToken : public Core::IToken
{
public:
    enum ValueType {
        FormItemLabel = 0,
        FormItemTooltip,
        FormItemPatientModelValue,
        FormItemPrintValue,
        FormItemDataValue
    };
    static bool canManageValueType(FormItem *item, ValueType type);

    explicit FormItemToken(Form::FormItem *item, ValueType type);
    ~FormItemToken();
    bool initialize();
    
    QString uid() const;
    QString humanReadableName() const;
    QString tooltip() const;
    QString helpText() const;
    QString shortHtmlDescription() const;

    virtual QVariant testValue() const;
    virtual QVariant value() const;

private:
    FormItemTokenPrivate *d;
};

} // namespace Internal
} // namespace Form

#endif // FORM_INTERNAL_FORMITEMTOKEN_H


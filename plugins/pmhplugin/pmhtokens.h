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
 *  Authors:                                                               *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *                                                                         *
 ***************************************************************************/
#pragma once

#include <coreplugin/ipadtools.h>
#include <QObject>

/**
 * \file pmhtokens.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 07 May 2013
*/

namespace PMH {
class PmhCategoryModel;
namespace Internal {
class PmhTokensPrivate;

class PmhTokens : public Core::IToken
{
public:
    enum OutputType {
        HtmlOutput = 0,
        PlainTextOutput
    };

    PmhTokens();
    ~PmhTokens();
    bool initialize(PmhCategoryModel *model);
    
    void setOutputType(OutputType type);
    QString tooltip() const;

    QVariant testValue() const;
    QVariant value() const;

private:
    PmhTokensPrivate *d;
};

} // namespace Internal
} // namespace PMH


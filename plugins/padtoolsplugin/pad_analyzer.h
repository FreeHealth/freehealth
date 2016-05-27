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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_PAD_ANALYZER_H
#define PADTOOLS_PAD_ANALYZER_H

#include <QString>
#include <QStack>

#include <coreplugin/ipadtools.h>

#include "pad_fragment.h"
#include "pad_string.h"
#include "pad_document.h"
#include "pad_item.h"

/**
 * \file pad_analyzer.h
 * \author Eric Maeker, Guillaume Denry
 * \version 0.8.0
 * \date 25 Apr 2012
*/

namespace PadTools {
namespace Internal {
class PadDocument;
class PadAnalyzerPrivate;

class PadAnalyzer : public QObject
{
    Q_OBJECT
public:
    PadAnalyzer(QObject *parent = 0);
    ~PadAnalyzer();

    PadDocument *analyze(const QString &source);
    PadDocument *analyze(QTextDocument *source, PadDocument *padDocument);

    const QList<Core::PadAnalyzerError> lastErrors() const;

private:
    Internal::PadAnalyzerPrivate *d;
};

} // namespace Internal
} // namespace PadTools

#endif  // PADTOOLS_PAD_ANALYZER

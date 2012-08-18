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
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 ***************************************************************************/
#include "ipadtools.h"

/**
  \class Core::PadAnalyzerError
  Store all data relative to an analyzer error.
  Can be useful to reconstruct localized error string because all tokens are transmitted.
*/

/**
  \class Core::IPadTools
  Use this class to avoid any plugin dependencies (other than Core),
  when needing to access the \e current \e pad data.
*/

/**
  \fn virtual QString Core::IPadTools::parse(const QString &templ, QMap<QString,QVariant> &tokens, QList<PadAnalyzerError> &errors) = 0;
  Returns a parsing result of a template against some tokens
*/

/**
  \fn virtual QSyntaxHighlighter *Core::IPadTools::createSyntaxHighlighter(QTextEdit *textEdit, QMap<QString,QVariant> &tokens)
  Returns a syntax highlighter which can be used in text editors
  \todo +++OBSOLETE+++
*/

#include <translationutils/constants.h>

#include <QApplication>

using namespace Core;

TokenDescription::TokenDescription(const QString &uid) :
    _uid(uid),
    _trContext(Trans::Constants::CONSTANTS_TR_CONTEXT)
{
}

QString TokenDescription::humanReadableName() const
{
    if (!_trContext.isEmpty())
        return QApplication::translate(_trContext.toUtf8(), _human.toUtf8());
    return QApplication::translate(Trans::Constants::CONSTANTS_TR_CONTEXT, _human.toUtf8());;
}

QString TokenDescription::tooltip() const
{
    if (!_trContext.isEmpty())
        return QApplication::translate(_trContext.toUtf8(), _tooltip.toUtf8());
    return QApplication::translate(Trans::Constants::CONSTANTS_TR_CONTEXT, _tooltip.toUtf8());;
}

QString TokenDescription::helpText() const
{
    if (!_trContext.isEmpty())
        return QApplication::translate(_trContext.toUtf8(), _help.toUtf8());
    return QApplication::translate(Trans::Constants::CONSTANTS_TR_CONTEXT, _help.toUtf8());;
}

QString TokenDescription::shortHtmlDescription() const
{
    if (!_trContext.isEmpty())
        return QApplication::translate(_trContext.toUtf8(), _descr.toUtf8());
    return QApplication::translate(Trans::Constants::CONSTANTS_TR_CONTEXT, _descr.toUtf8());;
}

/** Sort description using the uids of items */
bool TokenDescription::operator<(const TokenDescription &descr) const
{
    return _uid < descr._uid;
}

void TokenNamespace::addChild(const Core::TokenNamespace &child)
{
    _children.append(child);
}

QList<Core::TokenNamespace> TokenNamespace::children() const
{
    return _children;
}

void TokenNamespace::clearChildren()
{
    _children.clear();
}

/**
 * \class Core::ITokenPool
 * Manages the Core::IToken objects. You can register your Core::IToken pointers
 * to make them available for the whole application.
 * The pointer pool is deleted by this manager, so you must not delete your registered pointers.
 */


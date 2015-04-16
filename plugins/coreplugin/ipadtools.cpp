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
 * \class Core::TokenDescription
 * Encapsulate token description. By default the translation context is
 * the one from the translationutils lib.
 * \sa Trans::Constants::CONSTANTS_TR_CONTEXT
 */

#include <coreplugin/icore.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QApplication>

using namespace Core;
using namespace Trans::ConstantTranslations;

static inline Core::ITokenPool *tokenPool() {return Core::ICore::instance()->padTools()->tokenPool();}

TokenDescription::TokenDescription(const QString &uid) :
    _uid(uid),
    _trContext(Trans::Constants::CONSTANTS_TR_CONTEXT)
{
}

/** Returns the human readable name of the token translated to the current application language */
QString TokenDescription::humanReadableName() const
{
    if (!_trContext.isEmpty())
        return QApplication::translate(_trContext.toUtf8(), _human.toUtf8());
    return QApplication::translate(Trans::Constants::CONSTANTS_TR_CONTEXT, _human.toUtf8());;
}

static QString fullNamespace(Core::TokenNamespace &ns)
{
    QStringList r;
    Core::TokenNamespace *current = const_cast<Core::TokenNamespace *>(&ns);
    while (current->children().count() > 0) {
        r << current->humanReadableName();
        current = const_cast<Core::TokenNamespace *>(&current->children().at(0));
    }
    if (current)
        r << current->humanReadableName();
    return r.join(".");
}

/**
 * Return the translated tooltip of the token, remembering to the user:
 * - its current namespace (human readable)
 * - its name (human readable)
 * - its current value.
 */
QString IToken::tooltip() const
{
    // Construct name
    Core::TokenNamespace ns = tokenPool()->getTokenNamespace(uid());
    QString name;
    humanReadableName().isEmpty() ? name = uid() : name = humanReadableName();
    QString tooltip;
    tooltip = QString("<b>%1<br /><b>%2<br /><b>%3")
            .arg(tkTr(Trans::Constants::TOKEN_NAMESPACE_1).arg(fullNamespace(ns).replace(" ", "&nbsp;")))
            .arg(tkTr(Trans::Constants::TOKEN_1).arg(name))
            .arg(tkTr(Trans::Constants::VALUE_1).arg(value().toString()))
            .replace(":", ":</b>");
    if (!TokenDescription::tooltip().isEmpty())
        tooltip += "<br />" + TokenDescription::tooltip().replace("\n", "<br />");
    return tooltip;
}

/** Returns the human readable tooltip of the token translated to the current application language */
QString TokenDescription::tooltip() const
{
    if (!_trContext.isEmpty())
        return QApplication::translate(_trContext.toUtf8(), _tooltip.toUtf8());
    return QApplication::translate(Trans::Constants::CONSTANTS_TR_CONTEXT, _tooltip.toUtf8());;
}

/** Returns the human readable help text of the token translated to the current application language */
QString TokenDescription::helpText() const
{
    if (!_trContext.isEmpty())
        return QApplication::translate(_trContext.toUtf8(), _help.toUtf8());
    return QApplication::translate(Trans::Constants::CONSTANTS_TR_CONTEXT, _help.toUtf8());;
}

/** Returns the human readable short description Html text of the token translated to the current application language */
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

/**
 * \fn virtual void Core::ITokenPool::registerNamespace(const Core::TokenNamespace &ns) = 0;
 * Register a token namespace. You must register namespace separately to tokens.
 */

/**
 * \fn virtual int Core::ITokenPool::rootNamespaceCount() const = 0;
 * Returns the root namespace count.
 */

/**
 * \fn virtual const Core::TokenNamespace &Core::ITokenPool::rootNamespaceAt(int index) const = 0;
 * Returns the root namespace at \e index.
 * \sa Core::ITokenPool::rootNamespaceCount()
 */

/**
 * \fn virtual Core::TokenNamespace Core::ITokenPool::getTokenNamespace(const QString &name) const = 0;
 */

/**
 * \fn virtual void Core::ITokenPool::addToken(Core::IToken *token) = 0;
 * Register a \e token. Emits the Core::ITokenPool::tokenAdded() signal.
 */

/**
 * \fn virtual void Core::ITokenPool::addTokens(QVector<Core::IToken *> &tokens) = 0;
 * Register a list of \e tokens. Emits the Core::ITokenPool::tokenAdded() signal.
 */

/**
 * \fn virtual Core::IToken *Core::ITokenPool::token(const QString &name) = 0;
 * Return the token with the uuid \e name.
 */

/**
 * \fn virtual void Core::ITokenPool::removeToken(Core::IToken *token) = 0;
 * Removes a registered token. Emit the Core::ITokenPool::tokenRemoved() signal.
 */

/**
 * \fn virtual QList<Core::IToken *> Core::ITokenPool::tokens() const = 0;
 * Returns all registered tokens (unsorted).
 * \sa Core::IToken::sortIndex(), Core::IToken::setSortIndex(), Core::IToken::sortIndexLessThan()
 */

/**
 * \fn virtual QVariant Core::ITokenPool::tokenTestingValue(const QString &name) = 0;
 */

/**
 * \fn virtual QVariant Core::ITokenPool::tokenCurrentValue(const QString &name) = 0;
 */

/**
 * \fn void Core::ITokenPool::tokenAdded(Core::IToken *token);
 * Signal emitted when a token is registered.
 */

/**
 * \fn void Core::ITokenPool::tokenRemoved(Core::IToken *token);
 * Signal emitted when a token is removed.
 */

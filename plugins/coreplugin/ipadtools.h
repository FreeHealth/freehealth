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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 ***************************************************************************/

#ifndef IPADTOOLS_H
#define IPADTOOLS_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QSyntaxHighlighter>

#include <coreplugin/core_exporter.h>

class QTextEdit;

/**
 * \file ipadtools.h
 * \author Guillaume DENRY <guillaume.denry@gmail.com>
 * \version 0.5.0
 * \date 13 Jan 2011
 */

namespace Core {
	/**
	   \brief Store all date relative to an analyzer error. Can be useful to reconstruct localized error string because all tokens are transmitted
	 */
	class PadAnalyzerError {
	public:
		enum Type {
			Error_NoError,
			Error_UnexpectedChar,
			Error_CoreDelimiterExpected
		};

		PadAnalyzerError(Type errorType, int line, int pos, const QMap<QString,QVariant> &errorTokens) :
			_errorType(errorType),
			_line(line),
			_pos(pos),
			_errorTokens(errorTokens) {}

		int errorType() const { return _errorType; }
		int line() const { return _line; }
		int pos() const { return _pos; }
		const QMap<QString,QVariant> &errorTokens() const { return _errorTokens; }

	private:
		Type _errorType;
		int _line, _pos;
		QMap<QString,QVariant> _errorTokens;
	};

	/**
	   \brief Use this class to avoid any plugin dependencies (other than Core), when needing to access to the \e current \e pad datas.
	*/
	class CORE_EXPORT IPadTools : public QObject
	{
        Q_OBJECT

	public:
		IPadTools(QObject * parent = 0) : QObject(parent) {}
		virtual ~IPadTools() {}

		// TODO manage errors

		/**
		 * \brief returns a parsing result of a template against some tokens
		 */
		virtual QString parse(const QString &templ, QMap<QString,QVariant> &tokens, QList<PadAnalyzerError> &errors) = 0;

		/**
		 * \brief returns a syntax highlighter which can be used in text editors
		 */
		virtual QSyntaxHighlighter *createSyntaxHighlighter(QTextEdit *textEdit, QMap<QString,QVariant> &tokens) { Q_UNUSED(textEdit); Q_UNUSED(tokens); return NULL ; }
	};
}


#endif

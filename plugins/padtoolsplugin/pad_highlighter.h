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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_PAD_HIGHLIGHTER_H
#define PADTOOLS_PAD_HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include "pad_analyzer.h"

/**
 * \file pad_highlighter.h
 * \author Eric Maeker, Guillaume Denry
 * \version 0.8.0
 * \date 25 Apr 2012
*/

namespace PadTools {

struct BlockData : public QTextBlockUserData
{
	enum TokenType {
		Token_OpenPad,
		Token_CoreDelimiter,
		Token_Core
	};
	QVector<TokenType> tokens;

	void eatClosePad();
	void eatCoreDelimiter();
};

class PadHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	PadHighlighter(QObject *parent) : QSyntaxHighlighter(parent) { init(); }
	PadHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) { init(); }
	PadHighlighter(QTextEdit *parent) : QSyntaxHighlighter(parent) { init(); }

protected:
    void highlightBlock(const QString &text);

private:
/*	enum BlockState {
		State_Normal = 0,
		State_Prefix,
		State_Postfix,
		State_Core
		};*/

	PadAnalyzer _padAnalyzer;
	QTextCharFormat _padDelimiterFormat;
	QTextCharFormat _coreDelimiterFormat;
	QTextCharFormat _prefixFormat;
	QTextCharFormat _postfixFormat;
	QTextCharFormat _coreTextFormat;

	void init();
};

}  // PadTools


#endif  // PADTOOLS_PAD_HIGHLIGHTER

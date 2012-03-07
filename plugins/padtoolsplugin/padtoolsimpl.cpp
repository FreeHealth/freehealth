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
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "padtoolsimpl.h"

#include "pad_analyzer.h"
#include "pad_highlighter.h"

using namespace PadTools;

QString PadToolsImpl::parse(const QString &templ, QMap<QString,QVariant> &tokens, QList<Core::PadAnalyzerError> &errors)
{
	Q_UNUSED(tokens); //TMP

	PadAnalyzer analyzer;
	Pad *pad = analyzer.analyze(templ);
	errors = analyzer.lastErrors();

	return pad->run(tokens);
}

QSyntaxHighlighter *PadToolsImpl::createSyntaxHighlighter(QTextEdit *textEdit, QMap<QString,QVariant> &tokens)
{
	Q_UNUSED(tokens); //TMP

	return new PadHighlighter(textEdit);
}

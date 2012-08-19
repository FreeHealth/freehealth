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
#ifndef PADTOOLS_PADWRITER_H
#define PADTOOLS_PADWRITER_H

#include <coreplugin/ipadtools.h>

QT_BEGIN_NAMESPACE
class QTextCursor;
QT_END_NAMESPACE

/**
 * \file padwriter.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace PadTools {
class PadFragment;

namespace Internal {
class PadWriterPrivate;
namespace Ui {
class PadWriter;
}
}

class PadWriter : public Core::IPadWriter
{
    Q_OBJECT
    
public:
    explicit PadWriter(QWidget *parent = 0);
    ~PadWriter();

public Q_SLOTS:
    // Core::IPadWriter interface
    void setPlainTextSource(const QString &plainText);
    void setHtmlSource(const QString &html);
    void filterTokenPool(const QString &tokenNamespace);
    void filterTokenPool(const QStringList &tokenNamespaces);

public:
    // Core::IPadWriter interface
    QString outputToPlainText() const;
    QString outputToHtml() const;
    QString rawSourceToPlainText() const;
    QString rawSourceToHtml() const;

public Q_SLOTS:
    void highlightCursor();

private:
    void findCursorPositionInOutput();

private Q_SLOTS:
    void wysiwygCursorChanged();
    void rawSourceCursorChanged();
    void changeRawSourceScenario(QAction*);

    void expandTokenTreeView();
    void analyseRawSource();
    void outputToRaw();

    void viewErrors();
    void setAutoUpdateOfResult(bool state);
    void setTestValues(bool state);
    void onPadFragmentChanged(PadFragment *fragment);

//private:
//    bool event(QEvent *event);

//private:
//    bool eventFilter(QObject *obj, QEvent *event);
//    void mouseMoveEvent(QMouseEvent *e);

private:
    Internal::PadWriterPrivate *d;
};

} // namespace PadTools

#endif // PADTOOLS_PADWRITER_H

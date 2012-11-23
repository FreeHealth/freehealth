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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "tableeditor.h"
#include "tabledialog.h"
#include "tablepropertieswidget.h"

#include <QTextEdit>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QMouseEvent>

#include <QDebug>

using namespace Editor;
using namespace Editor::Internal;

TableEditor::TableEditor(QWidget *parent) :
        QWidget(parent)
{}

TableEditor::~TableEditor()
{}

void TableEditor::addTable()
{
    TableDialog dialog(this);
    if ( dialog.exec() == QDialog::Rejected ) return;

    QTextCursor cursor(textEdit()->textCursor());

    QTextTable *table = cursor.insertTable(dialog.rows(), dialog.cols(), dialog.format());

    // Set Font Header to bold
    if ( dialog.format().headerRowCount() ) {
        int i = 0;
        for ( i = 0; i < dialog.cols() ; ++i ) {
            QTextCharFormat txtformat = table->cellAt( 0, i ).format();
            txtformat.setFontWeight( QFont::Bold );
            txtformat.setFontItalic( true );
            table->cellAt( 0, i ).setFormat( txtformat );
            textEdit()->setTextCursor( table->cellAt( 0, i ).firstCursorPosition() ) ;
            textEdit()->setAlignment( Qt::AlignCenter );
        }
    }
    textEdit()->setTextCursor( cursor );
}

void TableEditor::tableProperties()
{
    QTextTable *table = textEdit()->textCursor().currentTable();
    if (!table)
        return;
    TablePropertiesDialog dlg(this);
    dlg.setFormat(table->format());
    if (dlg.exec()==QDialog::Accepted) {
        table->setFormat(dlg.format());
    }
}

void TableEditor::tableAddRow()
{
    QTextTable *table = textEdit()->textCursor().currentTable();
    if (!table)
        return;
    const QTextTableCell & cell = table->cellAt( textEdit()->textCursor() );
    table->insertRows( cell.row() + 1, 1 );
}

void TableEditor::tableAddCol()
{
    QTextTable * table = textEdit()->textCursor().currentTable();
    if ( !table ) return;
    const QTextTableCell & cell = table->cellAt( textEdit()->textCursor() );
    table->insertColumns( cell.column() + 1, 1 );
    // Resize columns
    QTextTableFormat format = table->format();
    int i = 0;
    QVector<QTextLength> lengths;
    for ( i = 0; i < table->columns(); i++ ) {
        lengths << QTextLength( QTextLength::PercentageLength, 100 / table->columns() );
    }
    format.setColumnWidthConstraints( lengths );
    table->setFormat( format );
}

void TableEditor::tableRemoveRow()
{
    QTextCursor cursor = textEdit()->textCursor();
    QTextTable * table = cursor.currentTable();
    if ( !table ) return;
    int col = 0;
    int row = 0;
    int nrows, ncols;
    if ( cursor.hasSelection() ) {
        cursor.selectedTableCells( &row, &nrows, &col, &ncols );
        if ( nrows == 0 ) nrows++;
    } else {
        QTextTableCell cell = table->cellAt( cursor );
        col = cell.column();
        row = cell.row();
        nrows = 1;
    }
    table->removeRows( row , nrows );
}

void TableEditor::tableRemoveCol()
{
    QTextCursor cursor = textEdit()->textCursor();
    QTextTable * table = cursor.currentTable();
    int numberOfCols = table->columns();
    if ( !table ) return;
    int col = 0;
    int row = 0;
    int nrows, ncols;
    if ( cursor.hasSelection() ) {
        cursor.selectedTableCells( &row, &nrows, &col, &ncols );
        if ( nrows == 0 ) nrows++;
    } else {
        QTextTableCell cell = table->cellAt( cursor );
        col = cell.column();
        row = cell.row();
        ncols = 1;
    }
    table->removeColumns( col , ncols );

    if ( (numberOfCols - ncols ) <= 0 )
        return;

    // Resize columns
    QTextTableFormat format = table->format();
    int i = 0;
    QVector<QTextLength> lengths;
    for ( i = 0; i < table->columns(); i++ ) {
        lengths << QTextLength( QTextLength::PercentageLength, 100 / table->columns() );
    }
    format.setColumnWidthConstraints( lengths );
    table->setFormat( format );
}

void TableEditor::tableMergeCells()
{
    if ( !textEdit()->textCursor().hasSelection() ) return;
    QTextTable * table = textEdit()->textCursor().currentTable();
    if ( !table ) return;
    int col = 0;
    int row = 0;
    int nrows, ncols;
    textEdit()->textCursor().selectedTableCells( &row, &nrows, &col, &ncols );
    if ( ( nrows == 0 ) && ( ncols == 0 ) ) return;
    table->mergeCells( textEdit()->textCursor() );

    QTextTableCell cell = table->cellAt( row, col );
    textEdit()->setTextCursor( cell.firstCursorPosition() );
}

void TableEditor::tableSplitCells()
{
    // TODO : tableSplitCells
    QTextTable * table = textEdit()->textCursor().currentTable();
    if ( !table ) return;
    QTextTableCell cell = table->cellAt( textEdit()->textCursor() );

    //      table->splitCell( cell.row(), cell.column(), 2, 2 );
}


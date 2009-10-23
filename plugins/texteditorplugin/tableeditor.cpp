/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "tableeditor.h"
#include "tabledialog.h"
#include "tablepropertieswidget.h"

#include <QTextEdit>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>


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

    QTextCursor cursor( textEdit()->textCursor() );

    QTextTable *table = cursor.insertTable( dialog.rows(), dialog.cols(), dialog.format() );

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
    /** \todo v0.2.0 */
//    QTextTable *table = textEdit()->textCursor().currentTable();
//    if (!table)
//        return;
//    TablePropertiesDialog dlg(this);
//    dlg.setFormat(table->format());
//    if (dlg.exec()==QDialog::Accepted) {
//        table->setFormat(dlg.format());
//    }
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


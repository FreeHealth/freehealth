/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TEXTEDITORTABLEDIALOG_H
#define TEXTEDITORTABLEDIALOG_H

#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_icons.h>

#include <QDialog>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>

// include UI headers
#include "ui_tabledialog.h"

namespace Editor {
namespace Internal {

class TableDialog : public QDialog, private Ui::TableDialog
{
    Q_OBJECT
public:
    TableDialog( QWidget *parent = 0 ) :
            QDialog( parent )
    {
        setupUi( this );
        setWindowTitle(qApp->applicationName() + " - Rich Text Widget");
        setWindowIcon(Core::ICore::instance()->theme()->icon(Core::Constants::ICONTABLE));
    }

    ~TableDialog() {}

    int rows() { return rowSpin->value() ; }
    int cols() { return colSpin->value() ; }
    int cellSpacing() { return cellSpacingSpin->value(); }
    int cellPadding() { return cellPaddingSpin->value(); }
    int border() { return borderSpin->value(); }

    QTextTableFormat format()
    {
        QTextTableFormat format;
        format.setCellPadding( cellPadding() );
        format.setCellSpacing( cellSpacing() );
        format.setBorder( border() );
        format.setWidth( QTextLength( QTextLength::PercentageLength, 100 ) );
        int i = 0;
        QVector<QTextLength> lengths;
        for ( i = 0; i < cols(); i++ ) {
            lengths << QTextLength( QTextLength::PercentageLength, 100 / cols() );
        }
        format.setColumnWidthConstraints( lengths );
        if ( header->isChecked() ) format.setHeaderRowCount( 1 );
        else format.setHeaderRowCount( 0 );
        return format;
    }
};

}  // End Internal
}  // End Editor

#endif // TEXTEDITORTABLEDIALOG_H

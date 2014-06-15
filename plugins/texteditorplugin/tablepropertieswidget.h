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
#ifndef TABLEPROPERTIESWIDGET_H
#define TABLEPROPERTIESWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QTextTableFormat>

/**
 * \file tablepropertieswidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.2.1
 * \date 25 Oct 2009
*/

namespace Editor {
namespace Internal {
namespace Ui {
class TablePropertiesWidget;
class TablePropertiesDialog;
}

class TablePropertiesWidget : public QWidget
{
    Q_OBJECT
public:
    TablePropertiesWidget(QWidget *parent = 0);
    ~TablePropertiesWidget();

    void setFormat(const QTextTableFormat &format);
    QTextTableFormat format() const;

    int cellLeftMargin() const;
    int cellRightMargin() const;
    int cellTopMargin() const;
    int cellBottomMargin() const;
    int cellSpacing() const;
    int cellPadding() const;
    int borderWidth() const;


private:
    Ui::TablePropertiesWidget *m_ui;
    QTextTableFormat m_InitialFormat;
};


class TablePropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    TablePropertiesDialog(QWidget *parent = 0);
    ~TablePropertiesDialog();

    void setFormat(const QTextTableFormat &format);
    QTextTableFormat format() const;

    bool applyToSelectedCells() const;
    bool applyToWholeTable() const;

    int cellLeftMargin() const;
    int cellRightMargin() const;
    int cellTopMargin() const;
    int cellBottomMargin() const;
    int cellPadding() const;
    int cellSpacing() const;
    int borderWidth() const;

private:
    bool applyToCell, applyToTable;
    TablePropertiesWidget *m_Widget;
    Ui::TablePropertiesDialog *m_ui;
};

}  // End Internal
}  // End Editor


#endif // TABLEPROPERTIESWIDGET_H

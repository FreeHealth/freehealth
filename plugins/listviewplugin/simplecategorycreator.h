/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SIMPLECATEGORYCREATOR_H
#define SIMPLECATEGORYCREATOR_H

#include <QDialog>
#include <QModelIndex>
class QAbstractItemModel;

namespace Views {

namespace Ui {
    class SimpleCategoryCreator;
}

class SimpleCategoryCreator : public QDialog
{
    Q_OBJECT

public:
    explicit SimpleCategoryCreator(QWidget *parent = 0);
    ~SimpleCategoryCreator();

    void setModel(QAbstractItemModel *model, const QModelIndex &parent, const int columnToShow = 0);

    void setLabelColumn(int col) {m_LabelCol=col;}
    void setIconColumn(int col) {m_IconCol=col;}

public Q_SLOTS:
    void done(int r);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SimpleCategoryCreator *ui;
    QAbstractItemModel *m_Model;
    QModelIndex m_Parent;
    int m_LabelCol, m_IconCol;
    QString m_ThemedIconFileName;
};

}  // End namespace Views


#endif // SIMPLECATEGORYCREATOR_H

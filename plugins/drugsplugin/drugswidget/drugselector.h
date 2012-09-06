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
#ifndef DRUGSELECTOR_H
#define DRUGSELECTOR_H

// include form headers
#include "ui_drugselector.h"

QT_BEGIN_NAMESPACE
class QSqlTableModel;
class QToolButton;
QT_END_NAMESPACE

/**
 * \file drugselector.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 23 Sept 2010
 */

namespace DrugsDB {
class AtcTreeModel;
class GlobalDrugsModel;
}

namespace DrugsWidget {
namespace Internal {
class TreeProxyModel;

/**
 \brief Creates a QWidget that contains a search line, and views to drugs and inn. Emit signal drugSelected( int ) when a drug is selected. int is the CIS of the selected drug.
 \ingroup freediams drugswidget
*/
class DrugSelector : public QWidget, private Ui::DrugSelector
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugSelector);

public:
    explicit DrugSelector(QWidget *parent = 0);
    ~DrugSelector();

    void initialize();
    void updateDrugsViewColumns();

    void setFont(const QFont &font);
    void setFocus(Qt::FocusReason r = Qt::TabFocusReason) { searchLine->setFocus(r); }
    void onSearchShortCutTriggered();

    void setSearchMethod(int method);

    void connectFilter();
    void disconnectFilter();

Q_SIGNALS:
    void drugSelected(const QVariant &drugUid);
    void drugSelected(const QModelIndex &index);

private:
    void createDrugsHistoryActions();
    void createToolButtons();
    void refreshSearchToolButton();
    void refreshAvailableDrugsDatabaseButtons();
    void createDrugModelView();
    void createINNModelView();

private Q_SLOTS:
    // models slots
    void updateModelFilter();
    void on_InnView_clicked( const QModelIndex & index );
    void on_drugsView_doubleClicked( const QModelIndex & );
    void on_textButton_clicked();

    // history slots
    void historyAct_triggered( QAction * action );

    void onDrugsBaseChanged();
    void changeDrugBaseUid(QAction *a);

private:
    void changeEvent(QEvent *e);

private:
    // models
    DrugsDB::GlobalDrugsModel *m_GlobalDrugsModel;
    DrugsDB::AtcTreeModel *m_InnModel;
    TreeProxyModel *m_AtcProxyModel;

    // widgets and actions
    QToolButton  *m_SearchToolButton;      // toolbutton that goes to the searchline (left)
    QToolButton  *m_DrugsHistoryButton;   // toolbutton that goes to the searchline (right)
    QActionGroup *m_HistoryAct;

    // filter for drugs model
    QString m_filterModel;           // '__replaceit__' must be replaced by search text.
    QString m_WinTitle;

    // search method and history
    int    m_SearchMethod;              /*!< \sa mfDrugsConstants::SearchMethod */
};

}  // End Internal
}  // End DrugsWidget


#endif // DRUGSELECTOR_H

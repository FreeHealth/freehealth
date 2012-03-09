/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#ifndef RECEIPTVIEWER_H
#define RECEIPTVIEWER_H

#include "controlreceiptsIO.h"
#include <accountplugin/accountmode.h>
#include <accountplugin/account_exporter.h>

#include <QWidget>
#include <QStandardItemModel>
#include <QHash>
#include <QSqlError>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QStringListModel>
#include <QListView>
#include <QTreeView>
#include <QFile>
#include <QVBoxLayout>


namespace Ui {
    class ReceiptViewer;
}

/** \todo this is not correct. Two choices: create its own files or hide it in the cpp in an anonymous namespace. */
namespace InternalAmount {
class AmountModel;
}  // End namespace Internal

enum Rows {
        PREFERENTIAL_VALUE = 0,
        THESAURUS,
        VALUES,
        ROUND_TRIP,
        rows_MaxParam
        };

class treeViewsActions: public QTreeView
{
    Q_OBJECT

public:
    
    treeViewsActions(QWidget *parent);
    ~treeViewsActions();

    void reset() {QTreeView::reset();}
    bool fillActionTreeView();
    QStandardItemModel *treeModel() const {return m_actionsTreeModel;}

private:
    //void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool deleteItemFromThesaurus(QModelIndex &index);
    bool addPreferedItem(QModelIndex &index);
    bool isChildOfThesaurus();
    
/*Q_SIGNALS :
    void pressed(const QModelIndex&) { emit QAbstractItemView::pressed(const QModelIndex&);}*/

private Q_SLOTS:
    void choosePreferedValue(bool b);
    void deleteBox(bool b);
    void userIsChanged();

private:
    void changeEvent(QEvent *e);

private:
    /** \create a pimpl */
    QAction *m_choosePreferedValue;
    QAction *m_deleteThesaurusValue;
    QMenu *m_menuRightClic;
    QStandardItemModel *m_actionsTreeModel;
    QString m_userUuid;
    QMap<QString,QString> m_mapSubItems;
    QMap<int,QStandardItem*> m_mapOfMainItems;
};

class ChoosenListView: public QListView{
    Q_OBJECT
    public:
        ChoosenListView(QObject *parent ,InternalAmount::AmountModel *amountModel);
        ~ChoosenListView();
        InternalAmount::AmountModel * m_amountModel;
    private Q_SLOTS:
        void deleteItem(bool b);
    private:
        void mousePressEvent(QMouseEvent *event);
        QAction *m_clear;
        QAction * m_deleteInReturnedList;
        QMenu *m_menu;
        
};


class ACCOUNT_EXPORT ReceiptViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptViewer(QWidget *parent = 0);
    ~ReceiptViewer();
    void setPosition(QWidget *parent);
    
    QString m_userUuid;
    treeViewsActions *m_actionTreeView;
    ChoosenListView *m_returnedListView;

protected:
    void changeEvent(QEvent *e);
    void fillModel(QHash<QString,QString> &hashOfValues, 
                   int typeOfPayment, 
                   double percentage, 
                   const QVariant & debtor, 
                   const QVariant & site, 
                   const QVariant & distrules,
                   const int row);

    /** \todo remove this using the Core::ActionManager. */
    QVBoxLayout *m_vbox;
    QVBoxLayout *m_vboxForList;

private:
    
    QVariant firstItemChoosenAsPreferential(QString &item);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    //void deleteLine();
    void save();
    void saveAndQuit();
    void actionsOfTreeView(const QModelIndex& );
    void saveInThesaurus();
    void showControlReceipts(bool b);
    void clearAll(bool b);
    void controlReceiptsDestroyed();
    void userUid();
    
    //void createFirstTimeTxt();

private:
    /** \todo create a pimpl */
    Ui::ReceiptViewer *ui;
    QStringListModel *m_modelReturnedList;
    InternalAmount::AmountModel *m_model;
    ControlReceipts * m_control;
    QVariant m_siteUid;
    QVariant m_insuranceUid;
    double m_kilometers;
    double m_distanceRuleValue;
    QString m_distanceRuleType;
    QStringList m_listOfValues;
    //QMenu * m_returnedListMenu;
    
    //QFile m_fileFirstTime;
};

#endif // RECEIPTVIEWER_H

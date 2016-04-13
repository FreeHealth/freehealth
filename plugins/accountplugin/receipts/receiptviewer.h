/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef RECEIPTVIEWER_H
#define RECEIPTVIEWER_H

#include "controlreceiptsio.h"
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

namespace InternalAmount {

class DisplayModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    enum ColumnRepresentation {
        Col_Cash = 0,
        Col_Cheque,  // Devise monétaire
        Col_Visa,
        Col_Banking,
        Col_Other,
        Col_Due,
        Col_Debtor,
        Col_Site,
        Col_DistRule,
        Col_Act,
        Col_Count
    };
    DisplayModel(QObject *parent = 0) ;
    ~DisplayModel();

    int rowCount(const QModelIndex &parent ) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    bool insertRows( int position, int count, const QModelIndex & parent = QModelIndex() );
    bool removeRows(int position, int count, const QModelIndex & parent = QModelIndex());
    bool submit();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setHeaderData( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );
    QSqlError lastError();
    Qt::ItemFlags flags(const QModelIndex &index) const;

  private:
    QVector<QList<QVariant> > *m_listsOfValuesbyRows;
    QStringList m_headersRows;
    QStringList m_headersColumns;
    int m_rows ;
};
}  // End namespace Internal


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
    bool addpreferredItem(QModelIndex &index);
    bool isChildOfThesaurus();

/*Q_SIGNALS :
    void pressed(const QModelIndex&) { emit QAbstractItemView::pressed(const QModelIndex&);}*/

private Q_SLOTS:
    void choosepreferredValue(bool b);
    void deleteBox(bool b);
    void userIsChanged();

private:
    void changeEvent(QEvent *e);

private:
    /** \create a pimpl */
    QAction *m_choosepreferredValue;
    QAction *m_deleteThesaurusValue;
    QMenu *m_menuRightClic;
    QStandardItemModel *m_actionsTreeModel;
    QString m_userUuid;
    QMap<QString,QString> m_mapSubItems;
    QMap<int,QStandardItem*> m_mapOfMainItems;
};

class ChosenListView: public QListView{
    Q_OBJECT
    public:
        ChosenListView(QObject *parent ,InternalAmount::DisplayModel *amountModel);
        ~ChosenListView();
        InternalAmount::DisplayModel * m_amountModel;
    protected:
    void mouseReleaseEvent(QMouseEvent *event);
    private Q_SLOTS:
        void deleteItem(bool b);
    private:
        void changeEvent(QEvent *e);

        QAction *m_clear;
        QAction * m_deleteInReturnedList;
        QMenu *m_menu;
        QObject *m_parent;

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
    ChosenListView *m_returnedListView;

protected:
    void changeEvent(QEvent *e);
    void fillModel(QHash<QString,QString> &hashOfValues,
                   int typeOfPayment,
                   double percentage,
                   const QVariant & debtor,
                   const QVariant & site,
                   const QVariant & distrules,
                   const int row);

    // TODO: remove this using the Core::ActionManager.
    QVBoxLayout *m_vbox;
    QVBoxLayout *m_vboxForList;

private:

    QVariant firstItemchosenAsPreferential(QString &item);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    //void deleteLine();
    void save();
    void saveAndQuit();
    void quitFreeAccount();
    void actionsOfTreeView(const QModelIndex& );
    void saveInThesaurus();
    void showControlReceipts();
    void clearAll(bool b);
    void controlReceiptsDestroyed();
    void userUid();

    //void createFirstTimeTxt();

private:
    // TODO: create a pimpl
    Ui::ReceiptViewer *ui;
    QStringListModel *m_modelReturnedList;
    InternalAmount::DisplayModel *m_model;
    ControlReceipts * m_control;
    QVariant m_siteUid;
    QVariant m_insuranceUid;
    double m_kilometers;
    double m_distanceRuleValue;
    QString m_distanceRuleType;
    QStringList m_listOfValues;
    QString m_freePayer;
    //QMenu * m_returnedListMenu;

    //QFile m_fileFirstTime;
};

#endif // RECEIPTVIEWER_H

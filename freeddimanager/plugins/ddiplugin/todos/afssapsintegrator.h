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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AFSSAPSINTEGRATOR_H
#define AFSSAPSINTEGRATOR_H

#include <coreplugin/itoolpage.h>

#include <QObject>
#include <QAbstractTableModel>
class QSortFilterProxyModel;

namespace DrugsDB {

class AfssapsIntegratorPage : public Core::IToolPage
{
    Q_OBJECT
public:
    AfssapsIntegratorPage(QObject *parent = 0) : IToolPage(parent) { setObjectName("AfssapsIntegratorPage"); }

    virtual QString id() const {return "AfssapsIntegratorPage";}
    virtual QString name() const {return "Interactions Categories Creator";}
    virtual QString category() const {return tr("Interaction database");}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};

class AfssapsClassTreePage : public Core::IToolPage
{
    Q_OBJECT
public:
    AfssapsClassTreePage(QObject *parent = 0) : Core::IToolPage(parent) { setObjectName("AfssapsClassTreePage"); }

    virtual QString id() const {return "AfssapsClassTreePage";}
    virtual QString name() const {return "AfssapsClassTreePage";}
    virtual QString category() const {return tr("Interaction database");}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


class AfssapsLinkerModelPrivate;

class AfssapsLinkerModel : public QAbstractTableModel
{
    Q_OBJECT
    explicit AfssapsLinkerModel(QObject *parent = 0);

public:
    enum DataRepresentation {
        FancyButton = 0,
        AfssapsName,
        AffapsCategory,
        AtcCodes,
        En_Label,
        De_Label,
        Es_Label,
        Review,
        Reviewer,
        References,
        Comments,
        Date,
        AutoFoundAtcs,
        WarnDuplicates,
        ColumnCount
    };

    enum ModelType {
        Model_Labels = 0,
        Model_Tree
    };

    static AfssapsLinkerModel *instance(QObject *parent = 0);
    ~AfssapsLinkerModel();

    void selectModel(const int type);

    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    void fetchMore(const QModelIndex &parent = QModelIndex());

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool addUnreviewedMolecules(const QString &dbUid, const QStringList &molecules);
    bool addAutoFoundMolecules(const QMultiHash<QString, QString> &mol_atc, bool removeFromModel = false);

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;

public Q_SLOTS:
    bool saveModel();
    void setActualReviewer(const QString &name);

private:
    static AfssapsLinkerModel *m_Instance;
    AfssapsLinkerModelPrivate *d;

};

class AfssapsClassTreeModelPrivate;

class AfssapsClassTreeModel : public QAbstractTableModel
{
    Q_OBJECT
    explicit AfssapsClassTreeModel(QObject *parent = 0);

public:
    enum DataRepresentation {
        Name=0,
        Review,
        Reviewer,
        Date,
        ColumnCount
    };

    static AfssapsClassTreeModel *instance(QObject *parent = 0);
    ~AfssapsClassTreeModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void addBuggyInclusions(const QMultiHash<QString, QString> &buggyIncludes);

public Q_SLOTS:
    bool saveModel();
    void setActualReviewer(const QString &name);

private:
    static AfssapsClassTreeModel *m_Instance;
    AfssapsClassTreeModelPrivate *d;

};

namespace Ui {
class AfssapsIntegratorWidget;
}

class AfssapsIntegratorWidget : public QWidget
{
    Q_OBJECT
public:
    AfssapsIntegratorWidget(QWidget *parent = 0);

private:
    Ui::AfssapsIntegratorWidget *ui;
};


namespace Ui {
class AfssapsLinkerWidget;
}

class AfssapsLinkerWidget : public QWidget
{
    Q_OBJECT
public:
    AfssapsLinkerWidget(QWidget *parent = 0);

protected Q_SLOTS:
    void pressed(const QModelIndex &index);
    void on_saveFile_clicked();
    void on_findAtc_clicked();

private:
    Ui::AfssapsLinkerWidget *ui;
    QSortFilterProxyModel *proxyModel;
};


namespace Ui {
class AfssapsTreeWidget;
}

class AfssapsTreeWidget : public QWidget
{
    Q_OBJECT
public:
    AfssapsTreeWidget(QWidget *parent = 0);

protected Q_SLOTS:
//    void pressed(const QModelIndex &index);
//    void on_saveFile_clicked();
//    void on_findAtc_clicked();

private:
    Ui::AfssapsTreeWidget *ui;
    QSortFilterProxyModel *proxyModel;
};


} // namespace DrugsDB


#endif // AFSSAPSINTEGRATOR_H

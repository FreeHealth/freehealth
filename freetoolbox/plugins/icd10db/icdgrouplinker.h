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
#ifndef ICDGROUPLINKER_H
#define ICDGROUPLINKER_H

#include <icd10db/icd10_exporter.h>
#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

namespace Icd10 {

class ICD10DB_EXPORT IcdGroupLinkerPage : public Core::IToolPage
{
    Q_OBJECT
public:
    IcdGroupLinkerPage(QObject *parent = 0) : IToolPage(parent) { setObjectName("IcdGroupLinkerPage"); }

    virtual QString id() const {return "IcdGroupLinkerPage";}
    virtual QString name() const {return "ICD Group Linker";}
    virtual QString category() const {return Core::Constants::CATEGORY_ICD10DATABASE;}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};

namespace Ui {
    class IcdGroupLinkerWidget;
}

class IcdGroupLinkerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IcdGroupLinkerWidget(QWidget *parent = 0);
    ~IcdGroupLinkerWidget();

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void pressed(const QModelIndex &index);
    void on_saveFile_clicked();


private:
    Ui::IcdGroupLinkerWidget *ui;
    QSortFilterProxyModel *proxyModel;
};

class IcdGroupLinkerModelPrivate;

class ICD10DB_EXPORT IcdGroupLinkerModel : public QAbstractTableModel
{
    Q_OBJECT
    explicit IcdGroupLinkerModel(QObject *parent = 0);

public:
    enum DataRepresentation {
        FancyButton = 0,
        GroupId,
        GroupNameFr,
        GroupNameEn,
        GroupNameDe,
        GroupNameEs,
        IcdCodes,
        Review,
        Reviewer,
        References,
        Comments,
        Date,
        ColumnCount
    };

    static IcdGroupLinkerModel *instance(QObject *parent = 0);
    ~IcdGroupLinkerModel();

    bool databaseAvailable();

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

    QList<QVariant> getSidsForRow(int row);
    QList<QVariant> getSidsForGroup(const QString &icdGroup);

//    QStringList mimeTypes() const;
//    QMimeData *mimeData(const QModelIndexList &indexes) const;

public Q_SLOTS:
    bool saveModel();
//    void setActualReviewer(const QString &name);

private:
    static IcdGroupLinkerModel *m_Instance;
    IcdGroupLinkerModelPrivate *d;
};

} // namespace Icd10

#endif // ICDGROUPLINKER_H

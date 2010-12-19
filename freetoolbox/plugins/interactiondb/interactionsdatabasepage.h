/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef INTERACTIONSDATABASEPAGE_H
#define INTERACTIONSDATABASEPAGE_H

#include <coreplugin/itoolpage.h>
#include <QAbstractItemModel>

namespace IAMDb {

class InteractionsDatabasePage : public Core::IToolPage
{
    Q_OBJECT

public:
    InteractionsDatabasePage(QObject *parent = 0) : IToolPage(parent) {}
    ~InteractionsDatabasePage() {}

    virtual QString id() const {return "InteractionsDatabasePage";}
    virtual QString name() const {return "Interactions Database Builder";}
    virtual QString category() const {return tr("Interaction database");}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


class InteractionsDatabaseCreatorPage : public Core::IToolPage
{
    Q_OBJECT

public:
    InteractionsDatabaseCreatorPage(QObject *parent = 0) : Core::IToolPage(parent) {}
    ~InteractionsDatabaseCreatorPage() {}

    virtual QString id() const {return "InteractionsDatabaseCreatorPage";}
    virtual QString name() const {return "Create the interactions database";}
    virtual QString category() const {return tr("Interaction database");}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


class InteractionModelPrivate;

class InteractionModel : public QAbstractTableModel
{
    Q_OBJECT
    explicit InteractionModel(QObject *parent = 0);

public:
    enum DataRepresentation {
        Name = 0,
        Level,
        Review,
        Reviewer,
        References,
        Comments,
        Date,
        ColumnCount
    };

    static InteractionModel *instance(QObject *parent = 0);
    ~InteractionModel();

//    bool canFetchMore(const QModelIndex &parent) const;
//    void fetchMore(const QModelIndex &parent);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QString tagName(const QModelIndex &index) const;

    QString getRisk(const QModelIndex &index, const QString &lang);
    QString getManagement(const QModelIndex &index, const QString &lang);
    QString getLevel(const QModelIndex &index, const QString &lang);
    QString getReviewer(const QModelIndex &index);
    bool getReviewState(const QModelIndex &index);

    bool setRisk(const QModelIndex &item, const QString &lang, const QString &value);
    bool setManagement(const QModelIndex &item, const QString &lang, const QString &value);
    bool setLevel(const QModelIndex &item, const QString &lang, const QString &value);
    bool setReviewer(const QModelIndex &index, const QString &reviewer);
    bool setReviewState(const QModelIndex &index, bool state);

public Q_SLOTS:
    bool saveModel();
    void setActualReviewer(const QString &name);
    void startTranslations();
    void correctTranslations();

private Q_SLOTS:
    void translateNextData();
    void translationDone(const QString &trans);
    void saveTranslations();

private:
    static InteractionModel *m_Instance;
    InteractionModelPrivate *d;
};



namespace Ui {
class InteractionDatabaseCreator;
}

class InteractionDatabaseCreatorPrivate;

class InteractionDatabaseCreator : public QWidget
{
    Q_OBJECT
public:
    InteractionDatabaseCreator(QWidget *parent = 0);
    ~InteractionDatabaseCreator();

protected Q_SLOTS:
    void on_checkDatas_clicked();
    void on_createAndSave_clicked();

private:
    Ui::InteractionDatabaseCreator *ui;
    InteractionDatabaseCreatorPrivate *d;
};




namespace Ui {
class InteractionDatabaseBuilder;
}

class InteractionsDatabaseBuilderPrivate;

class InteractionsDatabaseBuilder : public QWidget
{
    Q_OBJECT
public:
    InteractionsDatabaseBuilder(QWidget *parent = 0);
    ~InteractionsDatabaseBuilder();

protected Q_SLOTS:
    void on_modify_clicked();
    void interactionActivated(const QModelIndex &index);
    void on_save_clicked();
    void on_translate_clicked();
    void on_makeCorrections_clicked();

private:
    Ui::InteractionDatabaseBuilder *ui;
    InteractionsDatabaseBuilderPrivate *d;
};




namespace Ui {
class InteractionDatabaseChecker;
}

class InteractionDatabaseCheckerPrivate;

class InteractionDatabaseChecker : public QWidget
{
    Q_OBJECT
public:
    InteractionDatabaseChecker(QWidget *parent = 0);
    ~InteractionDatabaseChecker();

protected Q_SLOTS:
    void on_check_clicked();

private:
    Ui::InteractionDatabaseChecker *ui;
    InteractionDatabaseCheckerPrivate *d;
};


} // namespace IAMDb

#endif // INTERACTIONSDATABASEPAGE_H

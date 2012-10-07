#ifndef ICDGROUPLINKER_H
#define ICDGROUPLINKER_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>


class IcdGroupLinkerPage : public Core::IToolPage
{
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

class IcdGroupLinkerModel : public QAbstractTableModel
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
//    bool addUnreviewedMolecules(const QString &dbUid, const QStringList &molecules);
//    bool addAutoFoundMolecules(const QMultiHash<QString, QString> &mol_atc, bool removeFromModel = false);

//    QStringList mimeTypes() const;
//    QMimeData *mimeData(const QModelIndexList &indexes) const;

public Q_SLOTS:
    bool saveModel();
//    void setActualReviewer(const QString &name);

private:
    static IcdGroupLinkerModel *m_Instance;
    IcdGroupLinkerModelPrivate *d;

};


#endif // ICDGROUPLINKER_H

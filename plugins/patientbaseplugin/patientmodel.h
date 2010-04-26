#ifndef PATIENTMODEL_H
#define PATIENTMODEL_H

#include <QAbstractTableModel>

namespace Patients {
namespace Internal {
class PatientModelPrivate;
}

class PatientModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class Internal::PatientModelPrivate;

public:
    enum DataRepresentation {
        Id = 0,
        Uid,
        UsersUidList,
        GroupsUidList,
        FamilyUid,
        IconizedGender,
        Title,
        FullName,
        BirthName,
        SecondName,
        Surname,
        Gender,
        DateOfBirth,
        MaritalStatus,
        DateOfDeath,
        Street,
        ZipCode,
        City,
        Country,
        AddressNote,
        FullAddress,
        Mails,
        Tels,
        Faxes,
        Photo,
        NumberOfColumns
    };

    enum TitleInterpret {
        NoTitle = 0,
        Mister = 1,
        Miss = 2,
        Madam = 3,
        Doctor = 4,
        Professor = 5,
        Captain = 6
    };

    enum FilterOn {
        FilterOnName,
        FilterOnFullName,
        FilterOnCity
    };

    PatientModel(QObject *parent);
    ~PatientModel();

    static PatientModel *activeModel() {return m_ActiveModel;}
    static void setActiveModel(PatientModel *model) {m_ActiveModel = model;}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void setFilter(const QString &name, const QString &surname, const FilterOn on = FilterOnFullName);
    QString filter() const;
    int numberOfFilteredPatients() const;

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
//    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    bool insertRow(int arow, const QModelIndex &aparent = QModelIndex())        { return insertRows(arow, 1, aparent); }
    bool insertColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return insertColumns(acolumn, 1, aparent); }
    bool removeRow(int arow, const QModelIndex &aparent = QModelIndex())        { return removeRows(arow, 1, aparent); }
    bool removeColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return removeColumns(acolumn, 1, aparent); }

    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;
//    Qt::ItemFlags flags(const QModelIndex &index) const;
//    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
//    QModelIndex buddy(const QModelIndex &index) const;
//    QModelIndexList match(const QModelIndex &start, int role,
//                                  const QVariant &value, int hits = 1,
//                                  Qt::MatchFlags flags =
//                                  Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const;
//    QSize span(const QModelIndex &index) const;



Q_SIGNALS:
    void patientAboutToChange(const QString &newUuid);
    void patientAboutToBeDeleted(const QString &uuid);
    void patientAboutToBeCreated(const QString &uuid);

    void patientChanged(const QString &uuid);
    void patientDeleted(const QString &uuid);
    void patientCreated(const QString &uuid);

protected Q_SLOTS:
    void changeUserUuid(const QString &uuid);

private:
    Internal::PatientModelPrivate *d;
    static PatientModel *m_ActiveModel;
};

}  // End namespace Patients


#endif // PATIENTMODEL_H

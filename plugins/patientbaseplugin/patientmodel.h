/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PATIENTMODEL_H
#define PATIENTMODEL_H

#include <patientbaseplugin/patientbase_exporter.h>

#include <QAbstractTableModel>

namespace Patients {
namespace Internal {
class PatientModelPrivate;
}

class PATIENT_EXPORT PatientModel : public QAbstractTableModel
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
        Age,
        MaritalStatus,
        Profession,
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
        FilterOnCity,
        FilterOnUuid
    };

    PatientModel(QObject *parent);
    ~PatientModel();

    static PatientModel *activeModel() {return m_ActiveModel;}
    static void setActiveModel(PatientModel *model) {m_ActiveModel = model;}

    // for ui preparations (gender, title) you can get translations from here.
    static QStringList genders();
    static QStringList titles();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void setFilter(const QString &name, const QString &surname, const QString &uuid = QString::null, const FilterOn on = FilterOnFullName);
    QString filter() const;
    int numberOfFilteredPatients() const;

//    QVariant headerData(int section, Qt::Orientation orientation,
//                                int role = Qt::DisplayRole) const;

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

    static QList<QString> patientName(const QList<QString> &uuids);


Q_SIGNALS:
    void patientAboutToChange(const QString &newUuid);
    void patientAboutToBeDeleted(const QString &uuid);
    void patientAboutToBeCreated(const QString &uuid);

    void patientChanged(const QString &uuid);
    void patientDeleted(const QString &uuid);
    void patientCreated(const QString &uuid);

public Q_SLOTS:
    bool submit();

protected Q_SLOTS:
    void changeUserUuid(const QString &uuid);

private:
    Internal::PatientModelPrivate *d;
    static PatientModel *m_ActiveModel;
};

}  // End namespace Patients


#endif // PATIENTMODEL_H

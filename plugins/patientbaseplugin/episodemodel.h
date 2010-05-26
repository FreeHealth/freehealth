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
#ifndef EPISODEMODEL_H
#define EPISODEMODEL_H

#include <QAbstractItemModel>

namespace Patients {
namespace Internal {
class EpisodeModelPrivate;
}

class EpisodeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    enum DataRepresentation {
        Label = 0,
        Date,
        Summary,
        FullContent,
        Id,
        UserUuid,
        PatientUuid,
        FormUuid,
        IsNewlyCreated,
        IsEpisode,
        XmlContent,
        MaxData
    };

    EpisodeModel(QObject *parent);
    ~EpisodeModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool insertRow(int arow, const QModelIndex &aparent = QModelIndex())        { return insertRows(arow, 1, aparent); }
    bool insertColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return insertColumns(acolumn, 1, aparent); }
    bool removeRow(int arow, const QModelIndex &aparent = QModelIndex())        { return removeRows(arow, 1, aparent); }
    bool removeColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return removeColumns(acolumn, 1, aparent); }

    bool isEpisode(const QModelIndex &index) const;
    bool isForm(const QModelIndex &index) const {return !isEpisode(index);}
    void setReadOnly(const bool state);
    bool isReadOnly() const;
    bool isDirty() const;

    bool submit();

Q_SIGNALS:
    void episodeAboutToChange(const QModelIndex &index);
    void episodeAboutToBeDeleted(const QModelIndex &index);
    void episodeAboutToBeCreated(const QModelIndex &index);

    void episodeChanged(const QModelIndex &index);
    void episodeDeleted(const QModelIndex &index);
    void episodeCreated(const QModelIndex &index);

public Q_SLOTS:
    void setCurrentUser(const QString &uuid);
    void setCurrentPatient(const QString &uuid);
    void setCurrentFormUuid(const QString &uuid);

    void onCoreDatabaseServerChanged();

private:
    Internal::EpisodeModelPrivate *d;
};

}  // End namespace Patients


#endif // End EPISODEMODEL_H

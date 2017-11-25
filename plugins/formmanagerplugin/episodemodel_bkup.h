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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EPISODEMODEL_H
#define EPISODEMODEL_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <coreplugin/icorelistener.h>
#include <coreplugin/ipatientlistener.h>

#include <QAbstractItemModel>

/**
 * \file episodemodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.6
 * \date jun 2012
*/

namespace Form {
class FormMain;
class EpisodeModel;

namespace Internal {
class EpisodeModelPrivate;

class EpisodeModelCoreListener : public Core::ICoreListener
{
    Q_OBJECT
public:
    EpisodeModelCoreListener(Form::EpisodeModel *parent);
    ~EpisodeModelCoreListener();
    bool coreAboutToClose();
private:
    Form::EpisodeModel *m_EpisodeModel;
};

class EpisodeModelPatientListener : public Core::IPatientListener
{
    Q_OBJECT
public:
    EpisodeModelPatientListener(Form::EpisodeModel *parent);
    ~EpisodeModelPatientListener();
    bool currentPatientAboutToChange();
private:
    Form::EpisodeModel *m_EpisodeModel;
};

}

class FORM_EXPORT EpisodeModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    enum DataRepresentation {
        Label = 0,
        Date,
        IsValid,
        Summary,
//        FullContent,
//        Id,
//        UserUuid,
//        PatientUuid,
        FormUuid,
//        IsNewlyCreated,
        IsEpisode,
        XmlContent,
        Icon,
        EmptyColumn1,
        EmptyColumn2,
        MaxData
    };

    EpisodeModel(Form::FormMain *rootEmptyForm, QObject *parent = 0);
    virtual ~EpisodeModel();
    void init(bool addLastEpisodeIndex = true);
    void refreshFormTree();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool isEpisode(const QModelIndex &index) const;
    bool isForm(const QModelIndex &index) const {return !isEpisode(index);}
    bool isUniqueEpisode(const QModelIndex &index) const;
    bool isNoEpisode(const QModelIndex &index);
    bool isMultiEpisode(const QModelIndex &index) const {return !isUniqueEpisode(index);}
    void setReadOnly(const bool state);
    bool isReadOnly() const;
    bool isDirty() const;
    bool isLastEpisodeIndex(const QModelIndex &index) const;

    Form::FormMain *formForIndex(const QModelIndex &index) const;
    QModelIndex indexForForm(const QString &formUid) const;

    bool submit();

Q_SIGNALS:
    void episodeAboutToChange(const QModelIndex &index);
    void episodeAboutToBeDeleted(const QModelIndex &index);
    void episodeAboutToBeCreated(const QModelIndex &index);

    void episodeChanged(const QModelIndex &index);
    void episodeDeleted(const QModelIndex &index);
    void episodeCreated(const QModelIndex &index);

private Q_SLOTS:
    void onUserChanged();
    void onPatientChanged();

public Q_SLOTS:
    bool activateEpisode(const QModelIndex &index, const QString &formUid); //const int id, const QString &formUid, const QString &xmlcontent);
    bool saveEpisode(const QModelIndex &index, const QString &formUid = QString::null);
    QString lastEpisodesSynthesis() const;

    void onCoreDatabaseServerChanged();

private:
    Internal::EpisodeModelPrivate *d;
};

}  // End namespace Form


#endif // End EPISODEMODEL_H

/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_NEW_EPISODEMODEL_H
#define FORM_NEW_EPISODEMODEL_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <coreplugin/icorelistener.h>
#include <coreplugin/ipatientlistener.h>

#include <QAbstractListModel>

/**
 * \file episodemodel.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 27 Aug 2012
*/

namespace Form {
class FormMain;
class EpisodeModel;
class FormManager;

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

}  // namespace Internal

class FORM_EXPORT EpisodeModel : public QAbstractListModel
{
    Q_OBJECT
    friend class Form::FormManager;

protected:
    EpisodeModel(Form::FormMain *rootEmptyForm, QObject *parent = 0);
    bool initialize();

public:
    enum DataRepresentation {
        Label = 0,
        IsValid,
        CreationDate,
        UserDate,
//        Summary,
        UserCreatorName,
        XmlContent,
        Icon,
        EmptyColumn1,
        EmptyColumn2,
        MaxData
    };
    virtual ~EpisodeModel();
    QString formUid() const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void setReadOnly(const bool state);
    bool isReadOnly() const;
    bool isDirty() const;

public Q_SLOTS:
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


#endif // End FORM_NEW_EPISODEMODEL_H

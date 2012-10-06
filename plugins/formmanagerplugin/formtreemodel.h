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
#ifndef FORM_FORMTREEMODEL_H
#define FORM_FORMTREEMODEL_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QStandardItemModel>

/**
 * \file formtreemodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 01 Oct 2012
*/

namespace Form {
class FormMain;
class SubFormInsertionPoint;
class FormCollection;

namespace Internal {
class FormTreeModelPrivate;
class FormManagerPrivate;
}

class FORM_EXPORT FormTreeModel : public QStandardItemModel
{
    Q_OBJECT
    friend class Form::Internal::FormManagerPrivate;

protected:
    explicit FormTreeModel(const FormCollection &collection, QObject *parent = 0);

public:
    enum DataRepresentation {
        Label = 0,
        Uuid,
        EmptyColumn1,
        EmptyColumn2,
        MaxData
    };

//    explicit FormTreeModel(Form::FormMain *emptyRootForm, QObject *parent = 0);
    ~FormTreeModel();

    void initialize();
    void refreshFormTree();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    const QString &modeUid() const;
    bool isUniqueEpisode(const QModelIndex &index) const;
    bool isNoEpisode(const QModelIndex &index) const;
    bool isMultiEpisode(const QModelIndex &index) const {return (!isUniqueEpisode(index) && !isNoEpisode(index));}
    Form::FormMain *formForIndex(const QModelIndex &index) const;

    void clearSubForms();
    bool addSubForm(const Form::SubFormInsertionPoint &insertionPoint);

public Q_SLOTS:
    bool clearFormContents();
    bool updateFormCount(const QModelIndex &index);

private Q_SLOTS:
    bool updateFormCount();
    void onPatientFormsLoaded();

private:
    Internal::FormTreeModelPrivate *d;
};

} // namespace Form

#endif // FORM_FORMTREEMODEL_H

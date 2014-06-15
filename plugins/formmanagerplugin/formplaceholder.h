/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORMPLACEHOLDER_H
#define FORMPLACEHOLDER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <formmanagerplugin/formcontextualwidget.h>
#include <coreplugin/icorelistener.h>
#include <coreplugin/ipatientlistener.h>

#include <QWidget>
#include <QTreeView>

QT_BEGIN_NAMESPACE
class  QTreeWidgetItem;
class QStackedLayout;
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file formplaceholder.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 27 Sept 2012
*/

namespace Form {
class EpisodeModel;
class FormMain;
class FormPlaceHolder;
class FormTreeModel;
class FormManager;

namespace Internal {
class FormPlaceHolderPrivate;

class FormPlaceHolderCoreListener : public Core::ICoreListener
{
    Q_OBJECT
public:
    FormPlaceHolderCoreListener(FormPlaceHolder *parent);
    ~FormPlaceHolderCoreListener();
    bool coreAboutToClose();
    QString errorMessage() const {return _errorMsg;}

private:
    FormPlaceHolder *_formPlaceHolder;
    QString _errorMsg;
};

class FormPlaceHolderPatientListener : public Core::IPatientListener
{
    Q_OBJECT
public:
    FormPlaceHolderPatientListener(FormPlaceHolder *parent);
    ~FormPlaceHolderPatientListener();
    bool currentPatientAboutToChange();
    QString errorMessage() const {return _errorMsg;}

private:
    FormPlaceHolder *_formPlaceHolder;
    QString _errorMsg;
};

}  // End namespace Internal


class FORM_EXPORT FormPlaceHolder : public Internal::FormContextualWidget
{
    Q_OBJECT
    friend class Form::FormManager;
    friend class Form::Internal::FormPlaceHolderPrivate;
    friend class Form::Internal::FormPlaceHolderCoreListener;
    friend class Form::Internal::FormPlaceHolderPatientListener;

public:
    FormPlaceHolder(QWidget *parent = 0);
    ~FormPlaceHolder();

    bool enableAction(WidgetAction action) const;
    void setFormTreeModel(FormTreeModel *model);
    QString currentFormLabel() const;

public Q_SLOTS:
    bool clear();

protected Q_SLOTS:
    void currentSelectedFormChanged(const QModelIndex &current, const QModelIndex &previous);
    void setCurrentEditingFormItem(const QModelIndex &index);
    bool createEpisode();
    bool validateCurrentEpisode();
    bool renewEpisode();
    bool saveCurrentEpisode();
    bool removeCurrentEpisode();
    bool takeScreenshotOfCurrentEpisode();
    bool addForm();
    bool removeSubForm();
    bool printFormOrEpisode();
    void episodeChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
    bool isDirty() const;

private Q_SLOTS:
    void onFormTreeModelReset();
    void saveSortOrderToSettings(int col, Qt::SortOrder sort);
    void onCurrentPatientChanged();
    void handlePressed(const QModelIndex &index);
    void handleClicked(const QModelIndex &index);
    void updateFormCount();

private:
    void changeEvent(QEvent *event);
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);

private:
    Internal::FormPlaceHolderPrivate *d;
};

}  // End namespace Form

#endif // FORMPLACEHOLDER_H

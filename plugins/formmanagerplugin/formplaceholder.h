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
#ifndef FORMPLACEHOLDER_H
#define FORMPLACEHOLDER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <formmanagerplugin/formcontextualwidget.h>

#include <QWidget>
#include <QTreeView>
#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
class  QTreeWidgetItem;
class QStackedLayout;
class QModelIndex;
QT_END_NAMESPACE

namespace Form {
class EpisodeModel;
class FormMain;
class FormPlaceHolder;
class FormTreeModel;

namespace Internal {
class FormPlaceHolderPrivate;

class FormItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FormItemDelegate(QObject *parent = 0);
    void setFormTreeModel(FormTreeModel *model);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

public:
    mutable QModelIndex pressedIndex;
    FormTreeModel *_formTreeModel;
};
}  // End namespace Internal


class FORM_EXPORT FormPlaceHolder : public Form::FormContextualWidget
{
    Q_OBJECT
    friend class FormManager;

public:
    FormPlaceHolder(QWidget *parent = 0);
    ~FormPlaceHolder();

    void setRootForm(Form::FormMain *rootForm);

public Q_SLOTS:
    void setCurrentForm(Form::FormMain *form);
    void setCurrentForm(const QString &formUid);
    void setCurrentEditingItem(const QModelIndex &index);

private Q_SLOTS:
    void episodeChanged(const QModelIndex &current, const QModelIndex &previous);
    void handlePressed(const QModelIndex &index);
    void handleClicked(const QModelIndex &index);

    void showLastEpisodeSynthesis();
    bool newEpisode();
    bool saveCurrentEditingEpisode();
    void addForm();
    void printCurrentItem();
    void validateEpisode();
    void removeEpisode();
    void takeScreenShotEpisode();

//protected Q_SLOTS:

protected:
    void changeEvent(QEvent *event);

private:
    Internal::FormPlaceHolderPrivate *d;
};

}  // End namespace Form

#endif // FORMPLACEHOLDER_H

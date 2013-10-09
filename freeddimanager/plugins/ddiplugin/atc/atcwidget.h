/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEDDIMANAGER_ATCWIDGET_H
#define FREEDDIMANAGER_ATCWIDGET_H

//#include <coreplugin/itoolpage.h>
//#include <coreplugin/ftb_constants.h>
#include <QSortFilterProxyModel>

#include <QWidget>
QT_BEGIN_NAMESPACE
class QLineEdit;
class QModelIndex;
class QToolButton;
class QAction;
QT_END_NAMESPACE

/**
 * \file atcwidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 09 Oct 2013
*/

namespace Utils {
class DetailsWidget;
}

namespace DDI {
namespace Internal {
namespace Ui {
class AtcWidget;
}
}

class TreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit TreeProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const;
    bool hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const;
};

class AtcWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AtcWidget(QWidget *parent = 0);
    ~AtcWidget();

private Q_SLOTS:
    void onAtcCodeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void onFilterChanged(const QString &filter);

private:
    void changeEvent(QEvent *e);

private:
    Internal::Ui::AtcWidget *ui;
    Utils::DetailsWidget *_details;
    TreeProxyModel *_proxyModel;
    QLineEdit *_code, *_english, *_french, *_deutsch, *_spanish;
    QToolButton *_left;
    QAction *aSearchEnglish, *aSearchCode;
};

}  //  End namespace DDI

#endif // FREEDDIMANAGER_ATCWIDGET_H

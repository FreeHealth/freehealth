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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MOLECULELINKERWIDGET_H
#define MOLECULELINKERWIDGET_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>
class QModelIndex;
class QSortFilterProxyModel;

namespace DrugsDB {
class MoleculeLinkerModel;

namespace Ui {
    class MoleculeLinkerWidget;
}

class MoleculeLinkerPage : public Core::IToolPage
{
public:
    MoleculeLinkerPage(QObject *parent = 0) : IToolPage(parent) { setObjectName("MoleculeLinkerPage"); }

    virtual QString id() const {return "MoleculeLinkerPage";}
    virtual QString name() const;
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};

class MoleculeLinkerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MoleculeLinkerWidget(QWidget *parent = 0);
    ~MoleculeLinkerWidget();

//    void processCSVFile();

protected Q_SLOTS:
    void changeDatabase(const int index);
    void activated(const QModelIndex &index);
    void pressed(const QModelIndex &index);
    void onRemoveUnreviewedRequested();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MoleculeLinkerWidget *ui;
    MoleculeLinkerModel *model;
    QSortFilterProxyModel *proxyModel;
};


}  //  End namespace DrugsDbCreator

#endif // MOLECULELINKERWIDGET_H

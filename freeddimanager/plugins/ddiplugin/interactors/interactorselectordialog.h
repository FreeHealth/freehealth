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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_INTERACTORSELECTORDIALOG_H
#define DDIMANAGER_DDIPLUGIN_INTERACTORSELECTORDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file interactorselectordialog.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 15 Oct 2013
*/

namespace DDI {
namespace Internal {
class InteractorSelectorDialogPrivate;
} // namespace Internal

class InteractorSelectorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InteractorSelectorDialog(QWidget *parent = 0);
    ~InteractorSelectorDialog();
    bool initialize();

    void setAllowMoleculeInteractorCreation(bool allow);
    void setAllowMultipleSelection(int maximumOfItemSelected);
    void setTitle(const QString &title);

    QList< QPair<QString, QString> > selectedInteractors() const;

private Q_SLOTS:
    void onSearchActionChanged(QAction *a);
    void onButtonClicked(QAbstractButton *button);
    void onMoleculeInteractorActivated(const QModelIndex &index);
    void onSelectedListActivated(const QModelIndex &index);
    void onSearchTextchanged(const QString &text);

#ifdef WITH_TESTS
public Q_SLOTS:
    void test_runTestAndAccept();
#endif

private:
    Internal::InteractorSelectorDialogPrivate *d;
};

} // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_INTERACTORSELECTORDIALOG_H


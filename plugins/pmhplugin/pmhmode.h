/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PMHMODE_H
#define PMHMODE_H

#include <coreplugin/modemanager/basemode.h>
#include <pmhplugin/pmhcontextualwidget.h>

#include <QObject>
#include <QHash>
#include <QString>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QAbstractButton;
class QPushButton;
class QToolBar;
QT_END_NAMESPACE

namespace PMH {
namespace Internal {
namespace Ui {
class PmhModeWidget;
}

class PmhModeWidget : public PMH::PmhContextualWidget
{
    Q_OBJECT

public:
    PmhModeWidget(QWidget *parent = 0);
    ~PmhModeWidget();

    int currentSelectedCategory() const;


private Q_SLOTS:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void onButtonClicked(QAbstractButton *button);
    void createCategory();
    void removeItem();
    void onPatientChanged();
    void createPmh();

private:
    void changeEvent(QEvent *e);

private:
    Ui::PmhModeWidget *ui;
    QToolBar *m_ToolBar;
    QPushButton *m_EditButton;
    QHash<QString, int> m_FormUid_StackId;
};

class PmhMode : public Core::BaseMode
{
    Q_OBJECT
public:
    explicit PmhMode(QObject *parent = 0);
    ~PmhMode();

    QString name() const;

private Q_SLOTS:
    void onPatientChanged();

private:
    QWidget *m_Widget;
    bool m_inPluginManager;
};

}  // End namespace Internal
}  // End namespace PMH

#endif // PMHMODE_H


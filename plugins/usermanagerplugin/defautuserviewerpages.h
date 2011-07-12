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
#ifndef DEFAUTUSERVIEWERPAGES_H
#define DEFAUTUSERVIEWERPAGES_H

#include <usermanagerplugin/widgets/iuserviewerpage.h>

#include <QWidget>
#include <QPointer>
class QDataWidgetMapper;

/**
 * \file defaultuserviewerpages.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 12 Jul 2011
*/

namespace UserPlugin {
namespace Internal {
namespace Ui {
class UserViewer_IdentityUI;
}

class DefautUserIdentityWidget : public QWidget
{
    Q_OBJECT
public:
    DefautUserIdentityWidget(QWidget *parent = 0);
    ~DefautUserIdentityWidget();

    void clear();
    bool submit();

private:
    Ui::UserViewer_IdentityUI *ui;
    QDataWidgetMapper *m_Mapper;
};

class DefautUserIdentityPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefautUserIdentityPage(QObject *parent = 0);
    ~DefautUserIdentityPage();

    QString uid() const;
    QString name() const;
    QStringList categoryTree() const;

    QWidget *createWidget(QWidget *parent);

    void setUserIndex(const QModelIndex &index);
    bool clear();
    bool submit();

private:
    QPointer<DefautUserIdentityWidget> m_Widget;
};



/*

class DefaultUserRightsPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserRightsPage(QObject *parent = 0);
    ~DefaultUserRightsPage();

    QString uid() const;
    QString name() const;
    QStringList categoryTree() const;

    QWidget *createWidget(QWidget *parent);

    void setUserIndex(const QModelIndex &index);
    bool clear();
    bool submit();
};
class DefaultUserProfessionalPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserPapersPage(QObject *parent = 0);
    ~DefaultUserPapersPage();

    QString uid() const;
    QString name() const;
    QStringList categoryTree() const;

    QWidget *createWidget(QWidget *parent);

    void setUserIndex(const QModelIndex &index);
    bool clear();
    bool submit();
};

class DefaultUserPapersPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserPapersPage(QObject *parent = 0);
    ~DefaultUserPapersPage();

    QString uid() const;
    QString name() const;
    QStringList categoryTree() const;

    QWidget *createWidget(QWidget *parent);

    void setUserIndex(const QModelIndex &index);
    bool clear();
    bool submit();
};

*/
}  // End namespace Internal
}  // End namespace UserPlugin

#endif // DEFAUTUSERVIEWERPAGES_H

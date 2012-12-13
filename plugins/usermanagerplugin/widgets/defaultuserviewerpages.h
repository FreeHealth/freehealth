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
#ifndef DEFAULTUSERVIEWERPAGES_H
#define DEFAULTUSERVIEWERPAGES_H

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

namespace Print {
class Printer;
class PrinterPreviewer;
}

namespace Identity {
class IdentityEditorWidget;
}

namespace ZipCodes {
class ZipCountryCompleters;
}

namespace UserPlugin {
namespace Internal {
namespace Ui {
class UserViewer_IdentityUI;
class UserViewer_ContactUI;
class UserViewer_RightsUI;
class UserViewer_ProfessionalUI;
}

class DefaultUserIdentityWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    DefaultUserIdentityWidget(QWidget *parent = 0);
    ~DefaultUserIdentityWidget();

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

private Q_SLOTS:
    void on_but_changePassword_clicked();
    void on_but_viewHistory_clicked();

private:
    void changeEvent(QEvent *e);

private:
    Ui::UserViewer_IdentityUI *ui;
    QDataWidgetMapper *m_Mapper;
    UserModel *m_Model;
};

class DefaultUserIdentityPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserIdentityPage(QObject *parent = 0);
    ~DefaultUserIdentityPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent);
};


class DefaultUserContactWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    DefaultUserContactWidget(QWidget *parent = 0);
    ~DefaultUserContactWidget();

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

private:
    void changeEvent(QEvent *e);

private:
    Identity::IdentityEditorWidget *m_identity;
    Ui::UserViewer_ContactUI *ui;
    QDataWidgetMapper *m_Mapper;
    UserModel *m_Model;
    ZipCodes::ZipCountryCompleters *m_ZipCompleter;
};

class DefaultUserContactPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserContactPage(QObject *parent = 0);
    ~DefaultUserContactPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent = 0);
};


class DefaultUserProfessionalWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    DefaultUserProfessionalWidget(QWidget *parent = 0);
    ~DefaultUserProfessionalWidget();

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

private:
    void changeEvent(QEvent *e);

private:
    Ui::UserViewer_ProfessionalUI *ui;
    QDataWidgetMapper *m_Mapper;
    UserModel *m_Model;
};

class DefaultUserProfessionalPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserProfessionalPage(QObject *parent = 0);
    ~DefaultUserProfessionalPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent = 0);

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);
    bool clear();
    bool submit();

private:
    QPointer<DefaultUserProfessionalWidget> m_Widget;
    UserModel *m_Model;
};


class DefaultUserRightsWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    DefaultUserRightsWidget(QWidget *parent = 0);
    ~DefaultUserRightsWidget();

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

private:
    void changeEvent(QEvent *e);

private:
    Ui::UserViewer_RightsUI *ui;
    QDataWidgetMapper *m_Mapper;
    UserModel *m_Model;
};

class DefaultUserRightsPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserRightsPage(QObject *parent = 0);
    ~DefaultUserRightsPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent = 0);

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);
    bool clear();
    bool submit();

private:
    QPointer<DefaultUserRightsWidget> m_Widget;
    UserModel *m_Model;
};


class DefaultUserPapersWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    DefaultUserPapersWidget(const int type, QWidget *parent = 0);
    ~DefaultUserPapersWidget();

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

private:
//    void changeEvent(QEvent *e);

private:
    Print::PrinterPreviewer *preview;
    QDataWidgetMapper *m_Mapper;
    UserModel *m_Model;
    int m_type;
    int m_row;
};

class DefaultUserPapersPage : public IUserViewerPage
{
    Q_OBJECT
public:
    enum PaperType {
        GenericPaper = 0,
        AdministrativePaper,
        PrescriptionPaper
    };

    DefaultUserPapersPage(const PaperType type, QObject *parent = 0);
    ~DefaultUserPapersPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent = 0);

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);
    bool clear();
    bool submit();

private:
    QPointer<DefaultUserPapersWidget> m_Widget;
    UserModel *m_Model;
    PaperType m_type;
};

/*
class DefaultUserProfessionalPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserPapersPage(QObject *parent = 0);
    ~DefaultUserPapersPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent = 0);

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);
    bool clear();
    bool submit();

private:
    QPointer<DefaultUserRightsWidget> m_Widget;
    UserModel *m_Model;
};

class DefaultUserPapersPage : public IUserViewerPage
{
    Q_OBJECT
public:
    DefaultUserPapersPage(QObject *parent = 0);
    ~DefaultUserPapersPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;

    QWidget *createPage(QWidget *parent = 0);

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);
    bool clear();
    bool submit();

private:
    QPointer<DefaultUserRightsWidget> m_Widget;
    UserModel *m_Model;
};

*/
}  // End namespace Internal
}  // End namespace UserPlugin

#endif // DEFAULTUSERVIEWERPAGES_H

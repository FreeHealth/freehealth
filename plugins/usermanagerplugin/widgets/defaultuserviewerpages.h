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
#ifndef DEFAULTUSERVIEWERPAGES_H
#define DEFAULTUSERVIEWERPAGES_H

#include <usermanagerplugin/widgets/iuserviewerpage.h>

#include <QWidget>
#include <QPointer>
QT_BEGIN_NAMESPACE
class QDataWidgetMapper;
QT_END_NAMESPACE

/**
 * \file defaultuserviewerpages.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Dec 2012
*/

namespace Print {
class Printer;
class PrinterPreviewer;
}

namespace Identity {
class IdentityEditorWidget;
}

namespace UserPlugin {
class UserModel;

namespace Internal {
namespace Ui {
class UserViewer_RightsUI;
class UserViewer_ProfessionalUI;
}

class DefaultUserContactWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    DefaultUserContactWidget(QWidget *parent = 0);
    ~DefaultUserContactWidget();

    void setParentPageId(const QString &id) {m_parentId=id;}
    const QString &parentUserViewerPageId() const {return m_parentId;}

    void setUserModel(UserModel *model);
    void setUserIndex(const int row);

    void clear();
    bool submit();

private Q_SLOTS:
    void passwordChanged(const QString &newClearPassword);

//private:
//    void changeEvent(QEvent *e);

private:
    Identity::IdentityEditorWidget *_identity;
    UserModel *_userModel;
    QString m_parentId;
    int _row;
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

    void setParentPageId(const QString &id) {m_parentId=id;}
    const QString &parentUserViewerPageId() const {return m_parentId;}

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

private:
    void changeEvent(QEvent *e);

private:
    Ui::UserViewer_ProfessionalUI *ui;
    QDataWidgetMapper *m_Mapper;
    UserModel *_userModel;
    QString m_parentId;
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
    UserModel *_userModel;
};


class DefaultUserRightsWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    DefaultUserRightsWidget(QWidget *parent = 0);
    ~DefaultUserRightsWidget();

    void setParentPageId(const QString &id) {m_parentId=id;}
    const QString &parentUserViewerPageId() const {return m_parentId;}

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

private:
    void changeEvent(QEvent *e);

private:
    Ui::UserViewer_RightsUI *ui;
    QDataWidgetMapper *m_Mapper;
    UserModel *_userModel;
    QString m_parentId;
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
    UserModel *_userModel;
};


class DefaultUserPapersWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    DefaultUserPapersWidget(const int type, QWidget *parent = 0);
    ~DefaultUserPapersWidget();

    void setParentPageId(const QString &id) {m_parentId=id;}
    const QString &parentUserViewerPageId() const {return m_parentId;}

    void setUserModel(UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

private:
//    void changeEvent(QEvent *e);

private:
    Print::PrinterPreviewer *preview;
    QDataWidgetMapper *m_Mapper;
    UserModel *_userModel;
    int m_type;
    int m_row;
    QString m_parentId;
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
    UserModel *_userModel;
    PaperType m_type;
};

}  // End namespace Internal
}  // End namespace UserPlugin

#endif // DEFAULTUSERVIEWERPAGES_H

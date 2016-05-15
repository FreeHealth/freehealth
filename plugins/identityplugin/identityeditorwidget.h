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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IDENTITY_IDENTITYEDITORWIDGET_H
#define IDENTITY_IDENTITYEDITORWIDGET_H

#include <identityplugin/identity_exporter.h>

#include <QWidget>
#include <QModelIndex>
#include <QDataWidgetMapper>
#include <QDate>

/**
 * \file identityeditorwidget.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 18 Dec 2012
*/

namespace Identity {
namespace Internal {
class IdentityEditorWidgetPrivate;

class IsDirtyDataWidgetMapper: public QDataWidgetMapper
{
    Q_OBJECT
public:
    IsDirtyDataWidgetMapper(QObject *parent = 0);
    void onModelSubmitted();
    bool isDirty() const;

public Q_SLOTS:
    void setCurrentIndex(int index);

private:
    void refreshCache();

private:
    QHash<QWidget *, QVariant> _original;
};
}

class IDENTITYSHARED_EXPORT IdentityEditorWidget : public QWidget
{
    Q_OBJECT
    friend class Internal::IdentityEditorWidgetPrivate;

    Q_PROPERTY(QString title READ currentTitle NOTIFY titleChanged)
    Q_PROPERTY(QString titleIndex READ currentTitleIndex NOTIFY titleIndexChanged)
    Q_PROPERTY(QString usualName READ currentUsualName NOTIFY usualNameChanged)
    Q_PROPERTY(QString otherNames READ currentOtherNames NOTIFY otherNamesChanged)
    Q_PROPERTY(QString firstName READ currentFirstName NOTIFY firstNameChanged)
    Q_PROPERTY(QDate dateOfBirth READ currentDateOfBirth NOTIFY dateOfBirthChanged)
    Q_PROPERTY(QString gender READ currentGender NOTIFY genderChanged)
    Q_PROPERTY(int genderIndex READ currentGenderIndex NOTIFY genderIndexChanged)
    Q_PROPERTY(QString language READ currentLanguage NOTIFY languageChanged)
//    QPixmap currentPhoto() const;

    Q_PROPERTY(QString clearLogin READ currentClearLogin NOTIFY clearLoginChanged)
    Q_PROPERTY(QString clearPassword READ currentClearPassword NOTIFY clearPasswordChanged)

public:
    enum AvailableWidget {
        Title                   = 0x00000001,
        TitleIndex              = 0x00000002,
        UsualName               = 0x00000004,
        OtherNames              = 0x00000008,
        FirstName               = 0x00000010,
        Gender                  = 0x00000020,
        GenderIndex             = 0x00000040,
        Language_QLocale        = 0x00000080,
        LanguageIso             = 0x00000100,
        DateOfBirth             = 0x00000200,
        DateOfDeath             = 0x08000400,
        Photo                   = 0x00000800,
        Street                  = 0x00001000,
        City                    = 0x00002000,
        Zipcode                 = 0x00004000,
        Province                = 0x00008000,
        Country_TwoCharIso      = 0x00010000,
        Country_QLocale         = 0x00020000,
        // TODO: implement the following
        Extra_Login             = 0x00040000,
        Extra_Password          = 0x00080000,
        Extra_ConfirmPassword   = 0x00100000,
        FullIdentity = TitleIndex | UsualName | OtherNames | FirstName | Gender | DateOfBirth,
        FullAddress =  Street | City | Zipcode | Province | Country_TwoCharIso |Country_QLocale,
        FullLogin = Extra_Login | Extra_Password | Extra_ConfirmPassword
    };
    Q_DECLARE_FLAGS(AvailableWidgets, AvailableWidget)

    IdentityEditorWidget(QWidget *parent = 0);
    ~IdentityEditorWidget();

    // initialize
    bool initialize();
    void setAvailableWidgets(AvailableWidgets widgets);
    void setReadOnly(bool readOnly);
    bool isReadOnly() const;
    void clear();

    // Use model
    void setModel(QAbstractItemModel *model);
    bool addMapping(AvailableWidget widget, int modelIndex);

    // Use XML
    void setXmlInOut(bool xmlonly);
    bool isXmlInOut() const;
    QString toXml() const;

    virtual bool isIdentityValid(bool warnUser = true) const;
    bool isModified() const;

    QString currentTitle() const;
    int currentTitleIndex() const;
    QString currentUsualName() const;
    QString currentOtherNames() const;
    QString currentFirstName() const;
    QString currentGender() const;
    int currentGenderIndex() const;
    QDate currentDateOfBirth() const;
    QString currentLanguage() const;

    QString currentClearLogin() const;
    QString currentClearPassword() const;
    bool isPasswordCompleted() const;

    QPixmap currentPhoto() const;
    bool hasPhoto() const;

    QString currentStreet() const;
    QString currentCity() const;
    QString currentStateProvince() const;
    QString currentCountryName() const;
    QString currentCountryIso() const;
    QString currentZipCode() const;

public Q_SLOTS:
    void setCurrentIndex(const QModelIndex &modelIndex);
    virtual bool submit();
    void updateGenderImage(int genderIndex);
    void updateGenderImage();

    // Xml management
    bool fromXml(const QString &xml);

Q_SIGNALS:
    void titleIndexChanged(int titleIndex);
    void titleChanged(const QString &title);
    void usualNameChanged(const QString &usualName);
    void otherNamesChanged(const QString &otherNames);
    void firstNameChanged(const QString &firstName);
    void dateOfBirthChanged(const QDate &date);
    void genderIndexChanged(int genderIndex);
    void genderChanged(const QString &gender);
    void languageChanged(const QString &languageName);
    void clearLoginChanged(const QString &login);
    void clearPasswordChanged(const QString &clearPassword);

    void clearLoginEditionFinished();
    void passwordConfirmed();

private:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void photoButton_clicked();
    void onCurrentPatientChanged();
    void onPhotoProviderRequested();
    void onPhotoProviderPhotoReady(const QPixmap &pixmap);

private:
    Internal::IdentityEditorWidgetPrivate *d;
};

}  // End namespace Identity

Q_DECLARE_OPERATORS_FOR_FLAGS(Identity::IdentityEditorWidget::AvailableWidgets)

#endif // IDENTITY_IDENTITYEDITORWIDGET_H

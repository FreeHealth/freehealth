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
#ifndef IDENTITY_IDENTITYEDITORWIDGET_H
#define IDENTITY_IDENTITYEDITORWIDGET_H

#include <identityplugin/identity_exporter.h>

#include <QWidget>
#include <QModelIndex>
#include <QDataWidgetMapper>

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

public:
    enum AvailableWidget {
        BirthName = 0,
        SecondName,
        FirstName,
        Gender,
        Title,
        DateOfBirth,
        DateOfDeath,
        Photo,
        Street,
        City,
        Zipcode,
        Province,
        Country
    };

    IdentityEditorWidget(QWidget *parent = 0);
    ~IdentityEditorWidget();

    void setModel(QAbstractItemModel *model);
    void addMapping(AvailableWidget widget, int modelIndex);

    void setXmlInOut(bool xmlonly);

    virtual bool isIdentityValid() const;
    virtual bool isIdentityAlreadyInDatabase() const;
    bool isModified() const;

    QString currentBirthName() const;
    QString currentSecondName() const;
    QString currentFirstName() const;
    QString currentGender() const;
    QDate currentDateOfBirth() const;

    QPixmap currentPhoto() const;
    bool hasPhoto() const;

    QString toXml() const;

public Q_SLOTS:
    void setCurrentIndex(const QModelIndex &patientIndex);
    virtual bool submit();
    void updateGenderImage(int genderIndex);
    void updateGenderImage();

    // Xml management
    bool fromXml(const QString &xml);

private:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void photoButton_clicked();
    void onCurrentPatientChanged();

private:
    Internal::IdentityEditorWidgetPrivate *d;
};

}  // End namespace Identity

#endif // IDENTITY_IDENTITYEDITORWIDGET_H

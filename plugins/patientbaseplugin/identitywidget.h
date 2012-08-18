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
#ifndef IDENTITYWIDGET_H
#define IDENTITYWIDGET_H

#include <patientbaseplugin/patientbase_exporter.h>

#include <QWidget>
#include <QModelIndex>

namespace Patients {
class PatientModel;

namespace Internal {
class IdentityWidgetPrivate;
}

class PATIENT_EXPORT IdentityWidget : public QWidget
{
    Q_OBJECT
    friend class Internal::IdentityWidgetPrivate;

public:
    enum EditMode {
        ReadOnlyMode,
        ReadWriteMode
    };

    IdentityWidget(QWidget *parent = 0, EditMode mode = ReadOnlyMode);
    ~IdentityWidget();

    void setCurrentPatientModel(Patients::PatientModel *model);
    Patients::PatientModel *patientModel() const;

    bool isIdentityValid() const;
    bool isIdentityAlreadyInDatabase() const;

    bool isModified() const;

    QString currentBirthName() const;
    QString currentSecondName() const;
    QString currentFirstName() const;
    QString currentGender() const;
    QDate currentDateOfBirth() const;

    QPixmap currentPhoto() const;
    QPixmap currentDefaultPhoto() const;
    bool hasPhoto() const;

public Q_SLOTS:
    void setCurrentIndex(const QModelIndex &patientIndex);
    bool submit();

protected:
    void changeEvent(QEvent *e);

protected Q_SLOTS:
    void photoButton_clicked();
    void setDefaultGenderPhoto();
    void updateGenderImage();

private:
    Internal::IdentityWidgetPrivate *d;
};

}  // End namespace Patients

#endif // IDENTITYWIDGET_H

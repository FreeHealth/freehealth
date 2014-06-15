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
 *   Main developers : Eric MAEKER
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_FORMCOLLECTION_H
#define FORM_FORMCOLLECTION_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>

/**
 * \file formcollection.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 15 Sept 2012
*/

namespace Form {
class FormManager;
class FormMain;
namespace Internal {
class FormCollectionPrivate;
class FormManagerPrivate;
}  // namespace Internal

class FORM_EXPORT FormCollection //: public QObject
{
//    Q_OBJECT
    friend class Form::FormManager;
    friend class Form::Internal::FormManagerPrivate;

public:
    enum CollectionType {
        CompleteForm,
        SubForm
    };

protected:
    explicit FormCollection();//(QObject *parent = 0);
    bool initialize();
    void setDuplicates(bool isDuplicates);
    void setType(CollectionType type);
    void setEmptyRootForms(const QList<Form::FormMain *> &emptyRootForms);
    void addEmptyRootForm(Form::FormMain * emptyRootForm);

public:
    ~FormCollection();

    bool isNull() const;
    bool isDuplicates() const;
    QString formUid() const;
    QString modeUid() const;
    CollectionType type() const;
    const QList<Form::FormMain *> &emptyRootForms() const;

    bool containsFormUid(const QString &formUid) const;
    bool containsIdentityForm() const;
    Form::FormMain *identityForm() const;
    Form::FormMain *form(const QString &formUid) const;

private:
    Internal::FormCollectionPrivate *d;
};

} // namespace Form

#endif  // FORM_FORMCOLLECTION_H


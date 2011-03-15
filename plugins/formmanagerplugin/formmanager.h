/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FORMMANAGER_H
#define FORMMANAGER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>

/**
 * \file formmanager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 05 Apr 2010
*/

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;
class QStackedLayout;
QT_END_NAMESPACE

namespace Core {
class UniqueIDManager;
}

namespace Form {
class FormMain;
class FormItem;
class IFormIO;
class FormPlaceHolder;

namespace Internal {
class FormManagerPrivate;
}

class FORM_EXPORT FormManager : public QObject
{
     Q_OBJECT
     FormManager(QObject *parent = 0);

public:
     static FormManager *instance();
     ~FormManager();

     Form::FormMain *loadFile(const QString &filename, const QList<Form::IFormIO *> &iolist);
     void activateMode();

     FormMain *createForm(const QString &uuid, FormMain *parent = 0);
     QList<FormMain*> forms() const;
     bool hasForm(const QString &uuid) const;
     FormMain *form(const QString &uuid) const;

     FormPlaceHolder *formPlaceHolder() const;

     template <class T>
     T* getParent(FormItem *item)
     {
         T* parent = 0;
         QObject *p = item;
         while (p) {
             parent = qobject_cast<T*>(p);
             if (!parent) {
                 p = p->parent();
             } else {
                 break;
             }
         }
         return parent;
     }

public Q_SLOTS:
     bool translateForms();
     void setCurrentPatient(const QString &uuid);

 private:
     Internal::FormManagerPrivate *d;
     static FormManager *m_Instance;
};

} // End Form

#endif  // FORMMANAGER_H

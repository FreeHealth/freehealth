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
 * \version 0.5.0
 * \date 23 Mar 2011
*/

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;
class QStackedLayout;
class QAction;
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
class FormActionHandler : public QObject
{
    Q_OBJECT
public:
    FormActionHandler(QObject *parent = 0);
    ~FormActionHandler();

private:
    QAction *aAddEpisode;
    QAction *aValidateEpisode;
    QAction *aAddForm;
};
}

class FORM_EXPORT FormManager : public Internal::FormActionHandler
{
    Q_OBJECT
    FormManager(QObject *parent = 0);

public:
    static FormManager *instance();
    ~FormManager();

    void activateMode();

    QList<FormMain *> forms() const;
    Form::FormMain *rootForm(const char *modeUniqueName);

    template <class T> T* getParent(FormItem *item)
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

    QList<Form::FormMain *> loadFormFile(const QString &formUid);

private:
    void cleanPluginManagerObjectPool();

public Q_SLOTS:
    bool loadPatientFile();
    bool loadSubForms();
    bool readPmhxCategories(const QString &formUuidOrAbsPath);

Q_SIGNALS:
    void patientFormsLoaded();

private:
    Internal::FormManagerPrivate *d;
    static FormManager *m_Instance;
};


} // End Form

#endif  // FORMMANAGER_H

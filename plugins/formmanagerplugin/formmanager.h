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
#ifndef FORMMANAGER_H
#define FORMMANAGER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>
QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;
class QStackedLayout;
class QAction;
class QPixmap;
QT_END_NAMESPACE

/**
 * \file formmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 12 Sept 2012
*/

namespace DataPack {
class Pack;
}

namespace Form {
class FormCore;
class FormPage;
class FormMain;
class FormItem;
class IFormIO;
class FormPlaceHolder;
class SubFormInsertionPoint;
class FormTreeModel;
class EpisodeModel;

namespace Internal {
class FormManagerPlugin;
class FormManagerPrivate;
}  // namespace Internal

class FORM_EXPORT FormManager : public QObject
{
    Q_OBJECT
    friend class Form::FormCore;
    friend class Form::Internal::FormManagerPrivate;

protected:
    FormManager(QObject *parent = 0);

public:
    static FormManager *instance();
    ~FormManager();
    bool initialize();

    void activateMode();

    // Form management (load, save)
    FormPage *createFormPage(const QString &uuid);
    FormMain *form(const QString &formUid) const;
    QList<FormMain *> forms() const;
    QList<FormMain *> subFormsEmptyRoot() const;
    Form::FormMain *rootForm(const char *modeUniqueName) const;
    Form::FormMain *identityRootForm() const;
    Form::FormMain *identityRootFormDuplicate() const;
    QList<Form::FormMain *> loadFormFile(const QString &formUid);
    QPixmap getScreenshot(const QString &formUid, const QString &fileName);

    // Models management
//    FormTreeModel *formTreeModel(const char* modeUniqueName);
    EpisodeModel *episodeModel(Form::FormMain *form);
    EpisodeModel *episodeModel(const QString &formUid);

public Q_SLOTS:
    // Form management
    bool loadPatientFile();
    bool insertSubForm(const SubFormInsertionPoint &insertionPoint);

    // PMHx categories of forms management
    bool readPmhxCategories(const QString &formUuidOrAbsPath);

Q_SIGNALS:
    void patientFormsLoaded();
    void subFormLoaded(const QString &uuid);

private Q_SLOTS:
    void packChanged(const DataPack::Pack &pack);

//private:
//    void executeOnloadScript(Form::FormMain *emptyRootForm);

private:
    Internal::FormManagerPrivate *d;
};

} // End Form

#endif  // FORMMANAGER_H

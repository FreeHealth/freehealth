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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_FORMMANAGER_H
#define FORM_FORMMANAGER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>
#include <QHash>

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
 * \author Eric MAEKER
 * \version 0.8.4
 * \date 16 Apr 2013
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
class SubFormRemoval;
class FormTreeModel;
class EpisodeModel;
class FormCollection;

namespace Internal {
class FormManagerPlugin;
class FormManagerPrivate;
}  // namespace Internal

class FORM_EXPORT FormManager : public QObject
{
    Q_OBJECT
    friend class Form::FormCore;
    friend class Form::Internal::FormManagerPlugin;
    friend class Form::Internal::FormManagerPrivate;

protected:
    FormManager(QObject *parent = 0);
    void checkFormUpdates();

public:
    static FormManager *instance();
    ~FormManager();
    bool initialize();

//    // Form loading
//    QList<Form::FormMain *> loadFormFile(const QString &formUid);

    // Form collections
    const FormCollection &centralFormCollection(const QString &modeUId) const;
    const FormCollection &subFormCollection(const QString &subFormUid) const;

    const FormCollection &centralFormDuplicateCollection(const QString &modeUId) const;
    const FormCollection &subFormDuplicateCollection(const QString &subFormUid) const;

    QString centralFormUid() const;
    QStringList availableModeUids() const;

    // Form pointers and models
    FormMain *form(const QString &formUid) const;
    QList<FormMain *> allEmptyRootForms() const;
    FormTreeModel *formTreeModelForMode(const QString &modeUid);
    FormTreeModel *formTreeModelForCompleteForm(const QString &formUid);
    FormTreeModel *formTreeModelForSubForm(const QString &subFormUid);

    QList<FormMain *> allDuplicatesEmptyRootForms() const;

    // Mode page creation
    FormPage *createFormPage(const QString &uuid);

    // Form pointers && extra
    Form::FormMain *rootForm(const char *modeUniqueName) const;
    Form::FormMain *identityRootForm() const;
    // Form::FormMain *identityRootFormDuplicate() const;
    QPixmap getScreenshot(const QString &formUid, const QString &fileName);
    QString extractFormFileToTmpPath(const QString &formUid, const QString &fileName);

    // Form HTML ouput
    QString formPrintHtmlOutput(Form::FormMain *formMain);
    QString formExportHtmlOutput(Form::FormMain *formMain);

    // Link with tokens
    QHash<QString, QVariant> formToTokens(Form::FormMain *form) const;

public Q_SLOTS:
    // Form management
    bool loadPatientFile();
    bool onCurrentPatientChanged();
    bool insertSubForm(const SubFormInsertionPoint &insertionPoint);
    bool removeSubForm(const SubFormRemoval &subFormRemoval);

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

#endif  // FORM_FORMMANAGER_H

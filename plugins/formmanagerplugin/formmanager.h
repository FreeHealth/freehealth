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
 * \date 28 Aug 2012
*/

namespace Core {
class UniqueIDManager;
}

namespace DataPack {
class Pack;
}

namespace Form {
class FormMain;
class FormItem;
class IFormIO;
class FormPlaceHolder;
class SubFormInsertionPoint;

namespace Internal {
class FormManagerPrivate;
class FormActionHandler : public QObject
{
    Q_OBJECT
public:
    FormActionHandler(QObject *parent = 0);
    ~FormActionHandler();

private Q_SLOTS:
    void showPatientLastEpisode();
    void updateActions();

private:
    QAction *aAddEpisode;
    QAction *aValidateEpisode;
    QAction *aAddForm;
    QAction *aPrintForm;
    QAction *aShowPatientSynthesis;
    QAction *aShowPatientLastEpisode;
};
}

class FORM_EXPORT FormManager : public Internal::FormActionHandler
{
    Q_OBJECT
    friend class Form::Internal::FormManagerPrivate;

    FormManager(QObject *parent = 0);

public:
    static FormManager *instance();
    ~FormManager();

    void activateMode();

    QList<FormMain *> forms() const;
    QList<FormMain *> subFormsEmptyRoot() const;
    Form::FormMain *rootForm(const char *modeUniqueName);

    QList<Form::FormMain *> loadFormFile(const QString &formUid);
    QPixmap getScreenshot(const QString &formUid, const QString &fileName);

private:
    void executeOnloadScript(Form::FormMain *emptyRootForm);

public Q_SLOTS:
    bool loadPatientFile();
    bool insertSubForm(const SubFormInsertionPoint &insertionPoint);
    bool readPmhxCategories(const QString &formUuidOrAbsPath);

Q_SIGNALS:
    void patientFormsLoaded();
    void subFormLoaded(const QString &uuid);

private Q_SLOTS:
    void packChanged(const DataPack::Pack &pack);

private:
    Internal::FormManagerPrivate *d;
    static FormManager *m_Instance;
};


} // End Form

#endif  // FORMMANAGER_H

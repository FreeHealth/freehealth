/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef IFORMIO_H
#define IFORMIO_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <translationutils/multilingualclasstemplate.h>
#include <translationutils/constanttranslations.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QHash>
#include <QVariant>

/**
 * \file iformio.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 08 Apr 2011
*/

namespace Form {
class FormMain;

class FORM_EXPORT FormIOQuery
{
public:
    enum TypeOfForm {
        CompleteForms  = 0x001,
        SubForms       = 0x002,
        Pages          = 0x004,
        DefaultForms   = 0x040,
        UserForms      = 0x080,
        SpecifiedForm  = 0x100
    };
    Q_DECLARE_FLAGS(TypesOfForm, TypeOfForm)

    FormIOQuery() : m_type(DefaultForms | CompleteForms), m_ForceFile(false) {}
    virtual ~FormIOQuery() {}

    void setTypeOfForms(const TypesOfForm type) {m_type=type;}
    TypesOfForm typeOfForms() const {return m_type;}

    void setIsoLanguageAndCountry(const QStringList &langs) {m_langs=langs;}
    QStringList &isoLanguagesAndCountry() {return m_langs;}

    void setSpecialties(const QStringList &spe) {m_spe=spe;}
    QStringList &specialties() {return m_spe;}

    void setAuthors(const QStringList &authors) {m_authors=authors;}
    QStringList &authors() {return m_authors;}

    void setFormUuid(const QString &uuidOrAbsPath) {m_uuid=uuidOrAbsPath;}
    QString formUuid() const {return m_uuid;}

    /** Use this to force the reader to get the informations (description, form content) directly from the file even if the form is already stored in the database. */
    void setForceFileReading(const bool force) {m_ForceFile = force;}
    bool forceFileReading() const {return m_ForceFile;}

private:
    TypesOfForm m_type;
    QStringList m_langs, m_spe, m_authors;
    QString m_uuid;
    bool m_ForceFile;
};

//class FORM_EXPORT FormIOResult
//{
//public:
//    FormIOResult(const FormIOQuery &query);
//    virtual ~FormIOResult();

//    QString originalFormUuid() const;

//    QList<Form::FormMain *> rootForms() const {return m_RootForms;}
//    void setRootForms(const QList<Form::FormMain *> &list) const {m_RootForms=list;}

//private:
//    FormIOQuery m_Query;
//    QList<Form::FormMain *> m_RootForms;
//};

}
Q_DECLARE_OPERATORS_FOR_FLAGS(Form::FormIOQuery::TypesOfForm)

namespace Form {
class IFormIO;
namespace Internal {
class FormIODescriptionPrivate;
}

class FORM_EXPORT FormIODescription
{
public:
    enum DataRepresentation {
        UuidOrAbsPath= 0,
        Author,
        Country,
        AvailableLanguages,
        Version,
        IsCompleteForm,
        IsSubForm,
        IsPage,
        CreationDate,
        LastModificationDate,
        FreeMedFormsCompatVersion,
        Category,
        ShortDescription,
        HtmlDescription,
        HtmlSynthesis,
        License,
        GeneralIcon,
        Specialties,
        TypeName,
        WebLink,
        ScreenShotsPath,
        MaxParam
    };

    FormIODescription();
    virtual ~FormIODescription();

    void setIoFormReader(IFormIO *reader) {m_reader=reader;}
    IFormIO *reader() const {return m_reader;}

    QVariant data(const int ref, const QString &lang = QString::null) const;
    bool setData(const int ref, const QVariant &value, const QString &lang = QString::null);

    void toTreeWidget(QTreeWidget *tree) const;

    void warn() const;

private:
    Internal::FormIODescriptionPrivate *d;
    IFormIO *m_reader;
};

class FORM_EXPORT IFormIO : public QObject
{
    Q_OBJECT
public:
    IFormIO(QObject *parent=0) : QObject(parent) {}
    virtual ~IFormIO() {}

    virtual QString name() const = 0;

    virtual void muteUserWarnings(bool state) = 0;

    virtual bool canReadForms(const QString &uuidOrAbsPath) const = 0;

    virtual FormIODescription *readFileInformations(const QString &uuidOrAbsPath) = 0;
    virtual QList<FormIODescription *> getFormFileDescriptions(const FormIOQuery &query) = 0;

    virtual QList<Form::FormMain *> loadAllRootForms(const QString &uuidOrAbsPath = QString::null) = 0;
    virtual bool loadPmhCategories(const QString &uuidOrAbsPath) = 0; // must invalidate all existing mime before insertion of new categories

    virtual bool saveForm(QObject *treeRoot) = 0;

    virtual QString lastError() const = 0;
};

} // end Form

#endif // IFORMIO_H

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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_GENERICDESCRIPTION_H
#define UTILS_GENERICDESCRIPTION_H

#include <utils/global_exporter.h>
#include <utils/genericupdateinformation.h>

#include <QVariant>
#include <QHash>
#include <QList>
#include <QString>
QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file genericdescsription.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 06 Jan 2012
*/

namespace Utils {
class GenericUpdateInformation;
class VersionNumber;
}

namespace Utils {

class UTILS_EXPORT GenericDescription
{
public:
    /*! Data representation tokens for retrieving description data using the data() method. */
    enum DataRepresentation {
        // Non translatable data
        Uuid = 0,                       //!< Unique ID
        Version,                        //!< Version of the data
        Author,                         //!< Author name
        Country,                        //!< Countries where this data is valid
        URL,
        AbsFileName,                    //!< absolute filename path
        Vendor,                         //!< Vendor of this data
        Validity,                       //!< Validity
        EHRCompatVersion,      //!< With which version of FreeMedForms is this data compatible
        FreeDiamsCompatVersion,         //!< With which version of FreeDiams is this data compatible
        CreationDate,                   //!< Creation date
        LastModificationDate,           //!< Last modification date
        GeneralIcon,                    //!< Representative Icon of this data

        // Translatable data
        Label,  /*14*/                  //!< Label (translatable)
        Category,                       //!< %Category (translatable)
        ToolTip,                        //!< Tooltip (translatable)
        Specialties,                    //!< Specialities (translatable)
        TypeName,                       //!< Type name (translatable)
        ShortDescription,               //!< Short description (one line, translatable)
        HtmlDescription,                //!< Long description, including HTML (translatable)
        HtmlSynthesis,                  //!< Synthesis text (translatable)
        LicenseName,                    //!< Short name of license (e.g. GPL, translatable)
        LicenseTerms,                   //!< License terms, full text (translatable)
        WebLink,                        //!< Link to data information in the Internet
        MaxParam
    };
    enum ExtraDataReference {
        NonTranslatableExtraData = 20000,   //!< Start number for data reference values that are not translatable
        TranslatableExtraData = 40000       //!< Start number for data reference values that are translatable
    };

    GenericDescription(const QString &rootTag = QString::null);
    virtual ~GenericDescription();

    void setRootTag(const QString &rootTag);
    QString rootTag() const;

    void setSourceFileName(const QString &absFileName);
    QString sourceFileName() const;

    virtual void clear();

    virtual QVariant data(const int ref, const QString &lang = QString::null) const;
    virtual bool setData(const int ref, const QVariant &value, const QString &lang = QString::null);
    virtual QStringList availableLanguages() const;

    virtual bool fromXmlContent(const QString &xmlContent);
    virtual bool fromXmlFile(const QString &absFileName);
    virtual bool fromDomElement(const QDomElement &root);
    virtual QString toXml() const;
    virtual bool toDomElement(QDomElement *root, QDomDocument *doc) const;

    // Manage update information
    void addUpdateInformation(const Utils::GenericUpdateInformation &updateInfo);
    void removeUpdateInformation(int index);
    void insertUpdateInformation(int index, const Utils::GenericUpdateInformation &updateInfo);
    QList<Utils::GenericUpdateInformation> updateInformation() const;
    QList<Utils::GenericUpdateInformation> updateInformationForVersion(const QString &version) const;
    QList<Utils::GenericUpdateInformation> updateInformationForVersion(const Utils::VersionNumber &version) const;

    QString htmlUpdateInformationForVersion(const QString &version) const;
    QString htmlUpdateInformationForVersion(const Utils::VersionNumber &version) const;

    // Manage extra data
    void addNonTranslatableExtraData(const int ref, const QString &tagName);
    void addTranslatableExtraData(const int ref, const QString &tagName);

//    // Manage unknown tags (not in nontransatable, not in translatable)
//    virtual void readUnknownElement(const QDomElement &element);

    virtual void fillTreeWidget(QTreeWidget *tree) const;

    bool operator==(const GenericDescription &other) const;
    bool operator!=(const GenericDescription &other) const {return !operator==(other);}

private:
    QMap<int, QString> nonTranslatableTagsDataReference() const;
    QMap<int, QString> translatableTagsDataReference() const;

private:
    QList<Utils::GenericUpdateInformation> m_UpdateInfos;
    QString m_RootTag, m_SourceFileName;
    QHash<int, QString> m_TranslatableExtra, m_NonTranslatableExtra;
    QHash<QString, QHash<int, QVariant> > m_Data;
};

}  // namespace Utils

#endif // UTILS_GENERICDESCRIPTION_H

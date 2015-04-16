/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 ***************************************************************************/
#ifndef EBMDATA_H
#define EBMDATA_H

#include <medicalutils/medical_exporter.h>
#include <QVariant>
#include <QDomDocument>

namespace MedicalUtils {

class MEDICALUTILS_EXPORT EbmData
{
public:
    enum DataReference {
        Link = 0,
        PMID,
        IsAPubMedExtraction,
        AbstractPlainText,
        CompleteReferences,
        ShortReferences,
        Title,
        Authors,
        ReferencesCount
    };

    EbmData();
    ~EbmData();

    void setId(const QVariant &id) {m_Id = id;}
    QVariant id() const {return m_Id;}

    void setAbstract(const QString &plainText) {m_Abstract = plainText;}
    void setReferences(const QString &plainText) {m_Ref = plainText;}
    bool setPubMedXml(const QString &xml);
    void setLink(const QString &url) {m_Link=url;}

    QString data(const int reference) const;

    QString abstract() const {return m_Abstract;}
    QString references() const {return m_Ref;}
    QString link() const {return m_Link;}
    QString xmlEncoded() const {return m_PubMedXml;}

private:
    QVariant m_Id;
    QString m_Abstract, m_ShortRef, m_Ref, m_Link, m_Title, m_PubMedXml, m_Authors;
    QDomDocument m_Doc;
};

}  // End namespace MedicalUtils

MEDICALUTILS_EXPORT QDebug operator<<(QDebug dbg, const MedicalUtils::EbmData &c);
MEDICALUTILS_EXPORT QDebug operator<<(QDebug dbg, const MedicalUtils::EbmData *c);


#endif // EBMDATA_H

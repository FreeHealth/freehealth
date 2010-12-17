/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 ***************************************************************************/
#ifndef EBMDATA_H
#define EBMDATA_H

#include <medicalutils/medical_exporter.h>
#include <QVariant>

namespace MedicalUtils {

class MEDICALUTILS_EXPORT EbmData
{
public:
    EbmData();
    ~EbmData();

    // QVector integration
//    EbmData(const EbmData &other);
//    EbmData &operator=(const EbmData &other);

    void setId(const QVariant &id) {m_Id = id;}
    QVariant id() const {return m_Id;}

    void setAbstract(const QString &plainText);
    void setReferences(const QString &plainText);
    void setLink(const QString &url);

    QString abstract() const {return m_Abstract;}
    QString references() const {return m_Ref;}
    QString link() const {return m_Link;}

    void warn() const;

private:
    QVariant m_Id;
    QString m_Abstract, m_Ref, m_Link;
};

}  // End namespace MedicalUtils

#endif // EBMDATA_H

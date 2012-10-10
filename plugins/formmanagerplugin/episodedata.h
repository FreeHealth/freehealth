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
#ifndef EPISODEDATA_H
#define EPISODEDATA_H

#include <QHash>
#include <QVariant>
#include <QVector>

namespace Form {
namespace Internal {
class EpisodeBase;

class EpisodeValidationData
{
    friend class Form::Internal::EpisodeBase;

public:
    enum DataRepresentation {
        ValidationId = 0,
        EpisodeId,
        ValidationDate,
        UserUid,
        IsValid
    };

    EpisodeValidationData();
    ~EpisodeValidationData();

    void setData(int ref, const QVariant &value);
    QVariant data(int ref) const;

    void setModified(bool state) {m_Modified = state;}
    bool isModified() const {return m_Modified;}

protected:
    int episodeId() const {return m_Data.value(EpisodeId).toInt();}
    int validationId() const {return m_Data.value(ValidationId).toInt();}

private:
    QHash<int, QVariant> m_Data;
    bool m_Modified;
};

class EpisodeModificationData
{
    friend class Form::Internal::EpisodeBase;

public:
    enum DataRepresentation {
        ModificationId = 0,
        EpisodeId,
        Date,
        UserUid,
        Trace,
        IsValid
    };

    EpisodeModificationData();
    ~EpisodeModificationData();

    void setData(int ref, const QVariant &value);
    QVariant data(int ref) const;

    void setModified(bool state) {m_Modified = state;}
    bool isModified() const {return m_Modified;}

protected:
    int episodeId() const {return m_Data.value(EpisodeId).toInt();}
    int modificationId() const {return m_Data.value(ModificationId).toInt();}

private:
    QHash<int, QVariant> m_Data;
    bool m_Modified;
};

class EpisodeData
{
    friend class Form::Internal::EpisodeBase;

public:
    enum DataRepresentation {
        Id = 0,
        Label,
        UserDate,
        LastModificationDate,
        CreationDate,
        IsValid,
        IsNewlyCreated,
//        Summary,
//        FullContent,
        UserCreatorUuid,
        PatientUuid,
        FormUuid,
        ContentId,
        IsXmlContentPopulated,
        XmlContent,
        ValidationId,
        ValidationDate
    };

    EpisodeData();
    ~EpisodeData();

    void setData(int ref, const QVariant &value);
    QVariant data(int ref) const;

    void setModified(bool state) {m_Modified = state;}
    bool isModified() const {return m_Modified;}

    void  addEpisodeValidation(EpisodeValidationData &validation);
    QVector<EpisodeValidationData> &validations() {return m_Validation;}

    void addEpisodeModification(EpisodeModificationData &modification);
    QVector<EpisodeModificationData> &modifications() {return m_Modification;}

protected:
    int episodeId() const {return m_Data.value(Id).toInt();}
    int contentId() const {return m_Data.value(ContentId).toInt();}
    int validationId() const {return m_Data.value(ValidationId).toInt();}

private:
    QHash<int, QVariant> m_Data;
    bool m_Modified;
    QVector<EpisodeValidationData> m_Validation;
    QVector<EpisodeModificationData> m_Modification;
};


}  // End namespace Internal
}  // End namespace Form

#endif // EPISODEDATA_H

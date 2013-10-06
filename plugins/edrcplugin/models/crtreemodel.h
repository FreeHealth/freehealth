/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful, *
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
#ifndef EDRC_INTERNAL_CRTREEMODEL_H
#define EDRC_INTERNAL_CRTREEMODEL_H

#include <edrcplugin/edrc_exporter.h>
#include <QStandardItemModel>

/**
 * \file crtreemodel.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 25 Sept 2013
*/

namespace eDRC {
namespace Internal {
class ConsultResult;
class CrTreeModelPrivate;

class EDRC_EXPORT CrTreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum DataRepresentation {
        Label = 0,
        DateOfExamination,
        Validity,
        DiagnosisPosition,
        MedicalFollowUp,
        Html,
        Id,
        Empty1,
        Empty2,
        Empty3,
        ColumnCount
    };

    CrTreeModel(QObject *parent = 0);
    ~CrTreeModel();

    bool setCrList(const QList<ConsultResult> &cr);
    bool addConsultResult(const ConsultResult &cr);
    void updateConsultResult(const QModelIndex &crIndex, const ConsultResult &crToUpdate);
    bool removeItems(const QModelIndex &index);

    bool isConsultResult(const QModelIndex &index) const;
    bool isHistoryIndex(const QModelIndex &index) const;
    QString htmlContent(const QModelIndex &index) const;

    ConsultResult consultResultFromIndex(const QModelIndex &index) const;
    const QList<ConsultResult> &consultResultList() const;
private:
    CrTreeModelPrivate *d;
};

} // namespace Internal
} // namespace eDRC

#endif  // EDRC_INTERNAL_CRTREEMODEL_H

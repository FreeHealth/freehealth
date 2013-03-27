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
 ***************************************************************************/
#ifndef ICDDIALOG_H
#define ICDDIALOG_H

#include <icdplugin/icd_exporter.h>

#include <QDialog>

namespace ICD {
namespace Internal {
class IcdAssociation;
}

class IcdViewer;

class ICD_EXPORT IcdDialog : public QDialog
{
    Q_OBJECT
public:
    explicit IcdDialog(const QVariant &SID, QWidget *parent = 0);

    bool isSelectionValid() const;
    bool isUniqueCode() const;
    bool isAssociation() const;

    QVariant getSidCode() const;
    QVector<Internal::IcdAssociation> getAssocation() const;

public Q_SLOTS:
    void done(int result);

private:
    IcdViewer *m_View;

};


}  // End namespace ICD

#endif // ICDDIALOG_H

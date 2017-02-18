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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ICDVIEWER_H
#define ICDVIEWER_H

#include <icdplugin/icd_exporter.h>

#include <QWidget>


namespace ICD {
namespace Internal {
class IcdViewerPrivate;
}  // End namespace Internal
class FullIcdCodeModel;

namespace Ui {
    class IcdViewer;
}

class ICD_EXPORT IcdViewer : public QWidget
{
    Q_OBJECT

public:
    explicit IcdViewer(QWidget *parent = 0);
    ~IcdViewer();

    FullIcdCodeModel *icdModel() const;

public Q_SLOTS:
    void setCodeSid(const QVariant &sid);

protected:
    void changeEvent(QEvent *e);

private:
    Internal::IcdViewerPrivate *d;
};


}  // End namespace ICD


#endif // ICDVIEWER_H

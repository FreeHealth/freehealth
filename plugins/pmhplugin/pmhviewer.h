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
#ifndef PMHVIEWER_H
#define PMHVIEWER_H

#include <QWidget>

/**
 * \file pmhviewer.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 25 July 2012
*/

namespace Category {
class CategoryItem;
}

namespace PMH {
namespace Internal {
class PmhViewerPrivate;
class PmhData;
}  // End namespace Internal

class PmhViewer : public QWidget
{
    Q_OBJECT

public:
    enum EditMode {
        ReadOnlyMode,
        ReadWriteMode
    };
    enum ViewMode {
        ExtendedMode,
        SimpleMode
    };

    PmhViewer(QWidget *parent = 0, EditMode editMode = ReadOnlyMode, ViewMode viewMode = SimpleMode);
    ~PmhViewer();

    void setPatientInfoVisible(bool visible);
    void setEditMode(EditMode mode);
    void setPmhData(Internal::PmhData *pmh);
    void setCategoryForPmh(Category::CategoryItem *category);
    void createNewPmh();
    void revert();

    Internal::PmhData *modifiedPmhData() const;

protected Q_SLOTS:
    void onSimpleViewIcdClicked();
    void onSimpleViewLabelChanged(const QString &text);

protected:
    void changeEvent(QEvent *e);

private:
    Internal::PmhViewerPrivate *d;
};

}  // End namespace PMH

#endif // PMHVIEWER_H

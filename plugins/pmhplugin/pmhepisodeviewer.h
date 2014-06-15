/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef PMHEPISODEVIEWER_H
#define PMHEPISODEVIEWER_H

#include <QWidget>
class QModelIndex;

/**
 * \file pmhepisodeviewer.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 25 July 2012
*/

namespace PMH {
namespace Internal {
class PmhEpisodeViewerPrivate;
class PmhData;
}

namespace Ui {
    class PmhEpisodeViewer;
}

class PmhEpisodeViewer : public QWidget
{
    Q_OBJECT

public:
    explicit PmhEpisodeViewer(QWidget *parent = 0);
    ~PmhEpisodeViewer();

    void setPmhData(Internal::PmhData *pmh);
    void clear();

private Q_SLOTS:
    void itemActivated(const QModelIndex &item);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PmhEpisodeViewer *ui;
    Internal::PmhEpisodeViewerPrivate *d;
};

}  // End namespace PMH

#endif // PMHEPISODEVIEWER_H

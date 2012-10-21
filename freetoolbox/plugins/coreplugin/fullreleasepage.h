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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FULLRELEASEPAGE_H
#define FULLRELEASEPAGE_H

#include <coreplugin/itoolpage.h>

#include <QLabel>
#include <QHash>
class QSpacerItem;

namespace Core {

namespace Ui {
class FullReleasePage;
}

class FullReleasePage : public Core::IToolPage
{
public:
    FullReleasePage(QObject *parent = 0);
    ~FullReleasePage();

    QString id() const {return "FullReleasePage";}
    QString name() const {return tr("Full release data set");}
    QString category() const {return tr("General");}
    QIcon icon() const;

    // widget will be deleted after the show
    QWidget *createPage(QWidget *parent = 0);

    void addDownloadingProcess(const QString &message, const QString &id);
    void endDownloadingProcess(const QString &id);

    void addRunningProcess(const QString &message);
    void endLastAddedProcess();

private:
    QWidget *m_Widget, *m_CentralWidget;
    Ui::FullReleasePage *ui;
    QHash<QString, QLabel *> m_IconLabels;
    QSpacerItem *m_Spacer;
};

}  //  End namespace Core

#endif // FULLRELEASEPAGE_H

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
#ifndef COREABOUTDIALOG_H
#define COREABOUTDIALOG_H

#include <QDialog>
#include <QHash>

/**
 * \file aboutdialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.8
 * \date 08 Sept 2009
*/

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QWidget;
class QStackedLayout;
QT_END_NAMESPACE


namespace Core {
    class IAboutPage;
namespace Internal {
  namespace Ui {
      class AboutDialog;
  }
}
}

namespace Core {
namespace Internal {

class AboutDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(AboutDialog)

public:
    explicit AboutDialog(QWidget *parent = 0);
    virtual ~AboutDialog();

private:
    Ui::AboutDialog *m_ui;
};

} // End Internal
} // End Core

#endif // COREABOUTDIALOG_H

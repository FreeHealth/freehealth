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
 ***************************************************************************/
#include "imainwindow.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QMovie>
#include <QApplication>

#include <QDebug>

/**
  \class Core::IMainWindow
  Main window of the Core.
*/


using namespace Core;
using namespace Core::Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace {
    QDialog *dlg = 0;
    QWidget *widget = 0;
}

//IMainWindow::IMainWindow() : MainWindowActionHandler() {}

void IMainWindow::startProcessingSpinner()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//    if (dlg)
//        delete dlg;
//    dlg = new QDialog(this);
//    QGridLayout *lay = new QGridLayout(dlg);
//    QLabel *mov = new QLabel(this);
//    QMovie *movie = new QMovie(theme()->iconFullPath(Core::Constants::ICONSPINNER));
//    movie->start();
//    mov->setMovie(movie);
//    QLabel *t = new QLabel(this);
//    t->setText(tr("Loading data"));
//    dlg->setLayout(lay);
//    lay->addWidget(mov, 1, 0);
//    lay->addWidget(t, 1, 1);
//    dlg->show();
//    Utils::centerWidget(dlg, this);
}

void IMainWindow::endProcessingSpinner()
{
    QApplication::restoreOverrideCursor();
    if (widget) {
        widget->hide();
        delete widget;
        widget = 0;
    }
    if (dlg) {
        dlg->hide();
        delete dlg;
        dlg = 0;
    }
}

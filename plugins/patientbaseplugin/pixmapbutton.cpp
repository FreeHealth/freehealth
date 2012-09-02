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
 *   Main Developer : Christian A. Reiter <christian.a.reiter@gmail.com>   *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pixmapbutton.h"
#include <QtGui/QMenu>
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>

using namespace Patients;

/*!
 * \brief PixmapButton::PixmapButton
 * \param parent Parent of the Button, reached through to QPushButton.
 *
 * Just calls the QPushButton constructor and initializes the internal Pixmap.
 */
PixmapButton::PixmapButton(QWidget *parent) :
    QPushButton(parent),
    m_pixmap(QPixmap())
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    m_actionDeletePhoto = new QAction(Core::ICore::instance()->theme()->icon(Core::Constants::ICONREMOVE),
                                      tr("Delete photo"), this);
    connect(m_actionDeletePhoto, SIGNAL(triggered()),this, SLOT(clearPixmap()));
    addAction(m_actionDeletePhoto);
    m_actionDeletePhoto->setEnabled(false);

}

QPixmap PixmapButton::pixmap() const
{
    return m_pixmap;
}

void PixmapButton::setPixmap(const QPixmap& pixmap)
{
    setIcon(QIcon(pixmap));
    m_pixmap = pixmap;
    m_actionDeletePhoto->setEnabled(!pixmap.isNull());
}

void PixmapButton::clearPixmap()
{
    setPixmap(QPixmap());
}

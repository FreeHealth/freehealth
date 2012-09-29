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
    m_pixmap(QPixmap()),
    m_deletePhotoAction(0),
    m_defaultAction(0)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    m_deletePhotoAction = new QAction(Core::ICore::instance()->theme()->icon(Core::Constants::ICONREMOVE),
                                      tr("Delete photo"), this);
    connect(m_deletePhotoAction, SIGNAL(triggered()),this, SLOT(clearPixmap()));
    addAction(m_deletePhotoAction);
    m_deletePhotoAction->setEnabled(false);
}

QPixmap PixmapButton::pixmap() const
{
    return m_pixmap;
}

void PixmapButton::setDefaultAction(QAction *action)
{
    // don't accept the deleteaction as default!
    if (action == m_deletePhotoAction)
        return;

    // if there is only one action in the list, choose this, regarding what the given action is (e.g. NULL)
    if (actions().count() == 1) {
        m_defaultAction = actions().first();
        return;
    }

    // only set default action if it is already in action list.
    // this prevents setting actions that are from another widget.
    if (actions().contains(action))
        m_defaultAction = action;
}

QAction *PixmapButton::defaultAction() const
{
    return m_defaultAction;
}

void PixmapButton::setPixmap(const QPixmap& pixmap)
{
    setIcon(QIcon(pixmap));
    m_pixmap = pixmap;
    m_deletePhotoAction->setEnabled(!pixmap.isNull());
}

void PixmapButton::clearPixmap()
{
    setPixmap(QPixmap());
}

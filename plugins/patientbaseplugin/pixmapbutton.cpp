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
#include <coreplugin/itheme.h>

using namespace Patients;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

/*!
 * \brief Default constructor of the PixmapButton class.
 * \param parent parent of the button, reached through to QToolButton.
 *
 * Just calls the QPushButton constructor and initializes the internal Pixmap.
 */
PixmapButton::PixmapButton(QWidget *parent) :
    QToolButton(parent),
    m_pixmap(QPixmap()),
    m_deletePhotoAction(0),
    m_separator(0),
    m_defaultAction(0)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    // add a delete function to the actions menu
    m_deletePhotoAction = new QAction(Core::ICore::instance()->theme()->icon(Core::Constants::ICONREMOVE),
                                      tr("Delete photo"), this);
    connect(m_deletePhotoAction, SIGNAL(triggered()),this, SLOT(clearPixmap()));
    addAction(m_deletePhotoAction);
    m_deletePhotoAction->setEnabled(false);

    // add a separator to the actions menu
    m_separator = new QAction(this);
    m_separator->setSeparator(true);
    addAction(m_separator);
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
    if (action == m_separator)  // eh, nearly impossible, are we paranoid?
        return;

    // if there is only one action in the list (+separator +delete = 3), choose this one,
    // no matter what the given action is (e.g. NULL)
    if (actions().count() == 3) {
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

QAction *PixmapButton::deletePhotoAction() const
{
    return m_deletePhotoAction;
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
    m_deletePhotoAction->setEnabled(false);
}

void PixmapButton::setGenderImage(int genderIndex)
{
        // check if there is a has a real pixmap
        // if there is a pixmap, DON'T change the photo!
        if (m_pixmap.isNull()) {
            // if null, set default gendered icon
            QIcon icon;
            switch (genderIndex) {
            case 0:  icon = QIcon(QPixmap(theme()->iconFullPath(Core::Constants::ICONMALE, Core::ITheme::BigIcon))); break;
            case 1:  icon = QIcon(QPixmap(theme()->iconFullPath(Core::Constants::ICONFEMALE, Core::ITheme::BigIcon))); break;
            case 2:  icon = QIcon(QPixmap(theme()->iconFullPath(Core::Constants::ICONHERMAPHRODISM , Core::ITheme::BigIcon))); break;
            default: icon = QIcon();
            }
            //    set an empty underlying pixmap, but set the displayed button icon to the default placeholder icon
            setPixmap(QPixmap());
            setIcon(icon);
        }
}

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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *   Contributors:                                                         *
 *       Eric Maeker                                                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/*!
 * \class Identity::Internal::ThemedGenderButton
 * \brief This class provides a QPushButton with a displayed Pixmap on it.
 * \internal
 *
 * \property Pixmap The normal Qt QPushButton only can store a QIcon and has
 * no pixmap property. This
 * can be unconvenient when using the button as display widget in a MVC pattern as
 * widget - a QDataWidgetMapper doesn't know how to handle a QPushbutton.
 * Here comes the Identity::Internal::ThemedGenderButton. \n
 * It still displays a QIcon, but stores a QPixmap underneath.
 * It also provides a \e pixmap property for easy interacting with a QDataWidgetMapper.
 *
 * Theme connection: \n
 * This button is automatically connected to the application central theme and
 * correctly manages the Gender Pixmap.
 */

#include "themedgenderbutton.h"
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/itheme.h>

#include <QMenu>

#include <QDebug>

using namespace Identity;
using namespace Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

/*!
 * Default constructor of the ThemedGenderButton class.
 * \param parent parent of the button, passed through to QToolButton.
 * Just calls the QPushButton constructor and initializes the internal Pixmap.
 */
ThemedGenderButton::ThemedGenderButton(QWidget *parent) :
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

/** Return the current pixmap of the button */
QPixmap ThemedGenderButton::pixmap() const
{
    return m_pixmap;
}

/** Set the default action of the button */
void ThemedGenderButton::setDefaultAction(QAction *action)
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

/** Return the default action of the button */
QAction *ThemedGenderButton::defaultAction() const
{
    return m_defaultAction;
}

/** Delete the photo action */
QAction *ThemedGenderButton::deletePhotoAction() const
{
    return m_deletePhotoAction;
}

/** Set the pixmap of the button */
void ThemedGenderButton::setPixmap(const QPixmap &pixmap)
{
    setIcon(QIcon(pixmap));
    m_pixmap = pixmap;
    m_deletePhotoAction->setEnabled(!pixmap.isNull());
    Q_EMIT pixmapChanged(pixmap);
}

/** Clear the internal pixmap of the button */
void ThemedGenderButton::clearPixmap()
{
    setPixmap(QPixmap());
    m_deletePhotoAction->setEnabled(false);
}

/**
 * Set the gender pixmap to the \e genderIndex
 * \sa Trans::ConstantTranslations::genders(), Core::IPatient::GenderIdex
*/
void ThemedGenderButton::setGenderImage(int genderIndex)
{
    // check if there is a has a real pixmap
    // if there is a pixmap, DON'T change the photo!
    if (m_pixmap.isNull()) {
        // if null, set default gendered icon
        QPixmap genderPix;
        //TODO: install a "Gender" enum, see http://code.google.com/p/freemedforms/issues/detail?id=184
        genderPix = theme()->defaultGenderPixmap(genderIndex);

        //    set an empty underlying pixmap, but set the displayed button icon to the default placeholder icon
        setPixmap(genderPix);
    }
}

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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "staticalertwidgets.h"
#include "alertitem.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <QFileInfo>

using namespace Alert;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace {
static QIcon getIcon(const AlertItem &item)
{
    if (item.themedIcon().isEmpty() || !QFileInfo(theme()->iconFullPath(item.themedIcon())).exists()) {
        // get the level icon
        QString icon;
        switch (item.priority()) {
        case AlertItem::High: icon = Core::Constants::ICONCRITICAL; break;
        case AlertItem::Medium: icon = Core::Constants::ICONWARNING; break;
        case AlertItem::Low: icon = Core::Constants::ICONINFORMATION; break;
        }
        return theme()->icon(icon, Core::ITheme::SmallIcon);
    }
    return theme()->icon(item.themedIcon(), Core::ITheme::SmallIcon);
}

static QString getToolTip(const AlertItem &item)
{
    QString toolTip;
    if (item.category().isEmpty())
        toolTip = QString("<p>%1</p>").arg(item.label());
    else
        toolTip = QString("<p><b>%1</b>: %2</p>").arg(item.category()).arg(item.label());
    if (!item.description().isEmpty())
        toolTip += QString("<p style=\"color:gray;margin-left:10px;margin-top=0;margin-bottom:0px\">%1</p>").arg(item.description());
    return toolTip;
}

}  // namespace anonymous


/**
  \class Alert::StaticAlertLabel
  Create a QToolButton for any static view type Alert::AlertItem.
*/
StaticAlertToolButton::StaticAlertToolButton(QWidget *parent) :
    QToolButton(parent)
{
    setMinimumSize(QSize(16,16));
}

/** Define the Alert::AlertItem to use for this button. */
void StaticAlertToolButton::setAlertItem(const AlertItem &item)
{
    setIcon(getIcon(item));
    setToolTip(getToolTip(item));
}


/**
  \class Alert::StaticAlertLabel
  Create a QLabel for any static view type Alert::AlertItem. The QLabel will only present the icon
  of the alert and its label/category as tooltip. It is a 16x16 sized QLabel.
*/
StaticAlertLabel::StaticAlertLabel(QWidget *parent) :
    QLabel(parent)
{
    setMinimumSize(QSize(16,16));
}

/** Define the Alert::AlertItem to use for this button. */
void StaticAlertLabel::setAlertItem(const AlertItem &item)
{
    setPixmap(getIcon(item).pixmap(16,16));
    setToolTip(getToolTip(item));
}

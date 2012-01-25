/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

#include <QApplication>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>

#include "core.h"
#include "iservermanager.h"
#include "servermodel.h"

using namespace DataPack;
using namespace Trans::ConstantTranslations;

static const char *ICON_SERVER_CONNECTED = "connect_established.png";
static const char *ICON_SERVER_NOT_CONNECTED = "connect_no.png";
static const char *ICON_SERVER_ASKING_CONNECTION = "connect_creating.png";
static const char *ICON_SERVER_LOCAL = "server-local.png";

static inline QIcon icon(const QString &name, DataPack::Core::ThemePath path = DataPack::Core::MediumPixmaps) { return QIcon(Core::instance().icon(name, path)); }

ServerModel::ServerModel(QObject *parent) : QAbstractListModel(parent) {

}

QVariant ServerModel::data(const QModelIndex &index, int role) const {
    IServerManager &serverManager = *Core::instance().serverManager();
    const Server &s = serverManager.getServerAt(index.row());

    switch (role) {
    case Qt::DisplayRole:
        {
            QString label = s.description().data(ServerDescription::Label).toString();
            if (label.isEmpty())
                label = tkTr(Trans::Constants::UNKNOWN_SERVER);
            label = QString("<span style=\"color:black;font-weight:bold;\">%1</span>")
                    .arg(label);

            if (s.isConnected()) {
                label += QString("<br /><span style=\"color:gray; font-size:small;\">%2 (%3: %4)</span>")
                         .arg(tkTr(Trans::Constants::CONNECTED))
                         .arg(tkTr(Trans::Constants::LAST_CHECK))
                         .arg(s.lastChecked().toString(QLocale().dateFormat(QLocale::LongFormat)));
            } else {
                label += QString("<br /><small>%2</small>")
                         .arg(tkTr(Trans::Constants::NOT_CONNECTED));
            }

            label += QString("<br /><span style=\"color:gray; font-size:small;\">%1 %2</span>")
                     .arg(serverManager.getPackForServer(s).count())
                     .arg(tkTr(Trans::Constants::PACKAGES));
            return label;
        }
    case Qt::ToolTipRole:
        {
            QString toolTip = QString("<b>%1</b>:&nbsp;%2<br/><b>%3</b>:&nbsp;%4<br/><b>%5</b>:&nbsp;%6")
                .arg(QApplication::translate("Server", "Native Url").replace(" ", "&nbsp;"))
                .arg(s.nativeUrl())
                .arg(QApplication::translate("Server", "Recommended update checking"))
                .arg(Trans::ConstantTranslations::checkUpdateLabel(s.recommendedUpdateFrequency()))
                .arg(QApplication::translate("Server", "Url Style"))
                .arg(s.urlStyleName());
            return toolTip.replace(" ", "&nbsp;");
        }
    case Qt::DecorationRole:
        if (s.isLocalServer())
            return icon(::ICON_SERVER_LOCAL);
        if (s.isConnected())
            return icon(::ICON_SERVER_CONNECTED);
        return icon(::ICON_SERVER_NOT_CONNECTED);
    default: return QVariant();
    }

    return QVariant();
}

int ServerModel::rowCount(const QModelIndex &parent) const {
    return Core::instance().serverManager()->serverCount();
}


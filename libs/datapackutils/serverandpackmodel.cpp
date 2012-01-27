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
#include "serverandpackmodel.h"
#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>

#include <QApplication>
#include <QFont>
#include <QString>

#include <QDebug>


using namespace DataPack;
using namespace Trans::ConstantTranslations;

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }
static inline Internal::ServerManager *serverManager() { return qobject_cast<Internal::ServerManager*>(core().serverManager()); }
static inline QIcon icon(const QString &name, DataPack::DataPackCore::ThemePath path = DataPack::DataPackCore::MediumPixmaps) { return QIcon(core().icon(name, path)); }

namespace {

const int IS_SERVER = 1;
const int IS_PACK = 2;

const char *const ICON_SERVER_CONNECTED = "connect_established.png";
const char *const ICON_SERVER_NOT_CONNECTED = "connect_no.png";
const char *const ICON_SERVER_ASKING_CONNECTION = "connect_creating.png";
const char *const ICON_SERVER_LOCAL = "server-local.png";

const char *const ICON_PACKAGE = "package.png";

static void appendPackToModel(const Server &server, QStandardItem *rootItem)
{
    const QList<Pack> &packs = serverManager()->getPackForServer(server);
    for(int i=0; i < packs.count(); ++i) {
        QString pack = QString("<span style=\"color:black;font-weight:bold\">%1</span><br />"
                               "<span style=\"color:gray; font-size:small\">%2: %3</span>")
                .arg(packs.at(i).description().data(PackDescription::Label).toString())
                .arg(tkTr(Trans::Constants::VERSION))
                .arg(packs.at(i).description().data(PackDescription::Version).toString());
        QStandardItem *packItem = new QStandardItem(pack);
        QString f = packs.at(i).description().data(PackDescription::GeneralIcon).toString();
        if (f.isEmpty()) {
            packItem->setIcon(icon(::ICON_PACKAGE));
        } else {
            f = f.remove("__theme__/");
            packItem->setIcon(icon(f));
        }
        packItem->setData(::IS_PACK);
        rootItem->appendRow(packItem);
    }
}

static void createModel(QStandardItemModel *model)
{
    Internal::ServerManager *manager = serverManager();
    QFont bold;
    bold.setBold(true);
    model->clear();
    QStandardItem *root = model->invisibleRootItem();
    for(int i = 0; i < manager->serverCount(); ++i) {
        const Server &s = manager->getServerAt(i);
        QString label = s.description().data(ServerDescription::Label).toString();
        if (label.isEmpty())
            label = tkTr(Trans::Constants::UNKNOWN_SERVER);
        label = QString("<span style=\"color:black;font-weight:bold\">%1</span>")
                .arg(label);

        if (s.isConnected()) {
            label += QString("<br /><span style=\"color:gray; font-size:small\">%2 (%3: %4)</span>")
                    .arg(tkTr(Trans::Constants::CONNECTED))
                    .arg(tkTr(Trans::Constants::LAST_CHECK))
                    .arg(s.lastChecked().toString(QLocale().dateFormat(QLocale::LongFormat)));
        } else {
            label += QString("<br /><small>%2</small>")
                    .arg(tkTr(Trans::Constants::NOT_CONNECTED));
        }

        label += QString("<br /><span style=\"color:gray; font-size:small\">%1 %2</span>")
                .arg(serverManager()->getPackForServer(s).count())
                .arg(tkTr(Trans::Constants::PACKAGES));

        QString tooltip = QString("<b>%1</b>:&nbsp;%2<br/><b>%3</b>:&nbsp;%4<br/><b>%5</b>:&nbsp;%6")
                .arg(QApplication::translate("Server", "Native Url").replace(" ", "&nbsp;"))
                .arg(s.nativeUrl())
                .arg(QApplication::translate("Server", "Recommended update checking"))
                .arg(Trans::ConstantTranslations::checkUpdateLabel(s.recommendedUpdateFrequency()))
                .arg(QApplication::translate("Server", "Url Style"))
                .arg(s.urlStyleName())
                ;
        QStandardItem *server = new QStandardItem(label);
        server->setData(::IS_SERVER);
        server->setToolTip(tooltip.replace(" ", "&nbsp;"));

        root->appendRow(server);

        if (s.isLocalServer()) {
            server->setIcon(icon(::ICON_SERVER_LOCAL));
        } else if (s.isConnected()) {
            server->setIcon(icon(::ICON_SERVER_CONNECTED));
        } else {
            server->setIcon(icon(::ICON_SERVER_NOT_CONNECTED));
        }

        // Add packdescriptions
        appendPackToModel(s, server);
    }
}

}  //  End namespace anonymous


ServerAndPackModel::ServerAndPackModel(QObject *parent) :
    QStandardItemModel(parent),
    m_InstallChecking(false),
    m_PackCheckable(false)
{
    setObjectName("ServerAndPackModel");
    createModel(this);
    connect(serverManager(), SIGNAL(serverAdded(int)), this, SLOT(updateModel()));
    connect(serverManager(), SIGNAL(serverRemoved(int)), this, SLOT(updateModel()));
    connect(serverManager(), SIGNAL(serverConnected(Server,ServerIdentification)), this, SLOT(updateModel()));
    connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), this, SLOT(updateModel()));
}

/** When setting the installChecker feature to \e on, the model computes the packages dependencies. */
void ServerAndPackModel::setInstallChecker(const bool onOff)
{
    m_InstallChecking = onOff;
    reset();
}

/** Allow user to check package (for installation/desinstallation). */
void ServerAndPackModel::setPackCheckable(const bool checkable)
{
    m_PackCheckable = checkable;
    reset();
}

QVariant ServerAndPackModel::data(const QModelIndex &index, int role) const
{
    if (m_PackCheckable && role==Qt::CheckStateRole && index.parent()!=QModelIndex()) {
        if (QStandardItemModel::data(index, role).isNull())
            return Qt::Unchecked;
        return QStandardItemModel::data(index, role);
    }
    return QStandardItemModel::data(index, role);
}

bool ServerAndPackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (m_PackCheckable && role==Qt::CheckStateRole && index.parent()!=QModelIndex()) {
        /** \todo check pack dependencies */
//        const Server &server = serverManager()->getServerAt(index.parent().row());
//        const Pack &pack = serverManager()->getPackForServer(server).at(index.row());
//        const PackDependencies &dep = pack.dependencies();
//        for(int i = 0; i < dep.dependenciesCount(); ++i) {
//            if (dep.dependenciesAt(i).type()==PackDependencyData::Depends) {

//            }
//        }
        QStandardItemModel::setData(index, value, role);
    }
    return QStandardItemModel::setData(index, value, role);
}

Qt::ItemFlags ServerAndPackModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QStandardItemModel::flags(index);
    if (m_PackCheckable && index.parent()!=QModelIndex())
        f |= Qt::ItemIsUserCheckable;
    return f;
}

/** Refresh the model using the ServerManager data. */
void ServerAndPackModel::updateModel()
{
    createModel(this);
}

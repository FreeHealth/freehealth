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
#include "packmodel.h"
#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>

#include <coreplugin/constants_tokensandsettings.h>

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

const char *const ICON_PACKAGE = "package.png";

static QString packToHtml(const Pack &p, bool isInstalled)
{
    QString inst;
    if (isInstalled)
        inst = tkTr(Trans::Constants::INSTALLED);
    inst += "</span>";
    return QString("<span style=\"color:black;font-weight:bold\">%1</span><br />"
                           "<span style=\"color:gray; font-size:small\">%2: %3 %4")
            .arg(p.description().data(PackDescription::Label).toString())
            .arg(tkTr(Trans::Constants::VERSION))
            .arg(p.description().data(PackDescription::Version).toString())
            .arg(inst);
}

static QIcon iconForPack(const Pack &p)
{
    QString f = p.description().data(PackDescription::GeneralIcon).toString();
    if (f.isEmpty()) {
        return icon(::ICON_PACKAGE);
    }
    f = f.remove("__theme__/");
    return icon(f);
}

}  //  End namespace anonymous


PackModel::PackModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_InstallChecking(false),
    m_PackCheckable(false)
{
    setObjectName("DataPack::PackModel");
    getAllAvailablePacks();
    connect(serverManager(), SIGNAL(serverAdded(int)), this, SLOT(onServerAdded(int)));
    connect(serverManager(), SIGNAL(serverAboutToBeRemoved(int)), this, SLOT(onServerRemoved(int)));
    connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), this, SLOT(updateModel()));

}

/** When setting the installChecker feature to \e on, the model computes the packages dependencies. */
void PackModel::setInstallChecker(const bool onOff)
{
    m_InstallChecking = onOff;
    reset();
}

/** Allow user to check package (for installation/desinstallation). */
void PackModel::setPackCheckable(const bool checkable)
{
    m_PackCheckable = checkable;
    reset();
}

int PackModel::rowCount(const QModelIndex &) const
{
    return m_AvailPacks.count();
}

QVariant PackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    if (row < 0 || row > m_AvailPacks.count())
        return QVariant();


    if (role==Qt::DisplayRole && index.column()==Label) {
        return packToHtml(m_AvailPacks.at(row), m_UserCheckModif.value(row, m_IsInstalledCache.at(row)));
    } else if (m_PackCheckable && role==Qt::CheckStateRole && index.column()==Label) {
        if (m_UserCheckModif.value(row, m_IsInstalledCache.at(row))) {
            return Qt::Checked;
        } else {
            return Qt::Unchecked;
        }
    } else if (role==Qt::DecorationRole && index.column()==Label) {
        QString iconFileName = m_AvailPacks.at(row).description().data(PackDescription::GeneralIcon).toString();
        if (iconFileName.startsWith(Core::Constants::TAG_APPLICATION_THEME_PATH))
            iconFileName = iconFileName.remove(Core::Constants::TAG_APPLICATION_THEME_PATH);
        qWarning() << iconFileName;
        if (!iconFileName.isEmpty())
            return icon(iconFileName);
    }
    return QVariant();
}

bool PackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (m_PackCheckable && role==Qt::CheckStateRole && index.column()==Label) {
        m_UserCheckModif.insert(index.row(), value.toInt());
        Q_EMIT dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags PackModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (m_PackCheckable && index.parent()!=QModelIndex() && index.column()==Label)
        f |= Qt::ItemIsUserCheckable;
    return f;
}

/** Refresh the model using the ServerManager data. */
void PackModel::updateModel()
{
    getAllAvailablePacks();
    checkInstalledPack();
}

/** Prepare the model: get all available packs from server manager. */
void PackModel::getAllAvailablePacks()
{
    for(int i=0; i < serverManager()->serverCount(); ++i) {
        foreach(const Pack &p, serverManager()->getPackForServer(serverManager()->getServerAt(i))) {
            if (m_AvailPacks.contains(p))
                continue;
            m_AvailPacks.append(p);
        }
    }
    reset();
}

/** Manage model when a server is added to the server manager */
void PackModel::onServerAdded(const int index)
{
    // Add packs from this server if not already included
    foreach(const Pack &p, serverManager()->getPackForServer(serverManager()->getServerAt(index))) {
        if (m_AvailPacks.contains(p))
            continue;
        m_AvailPacks.append(p);
    }
}

/** Manage model when a server is about to be removed from the server manager */
void PackModel::onServerRemoved(const int index)
{
    // Reload all packages
    m_AvailPacks.clear();
    getAllAvailablePacks();
}

/** Check all installed packages. */
void PackModel::checkInstalledPack()
{
    m_InstalledPack.clear();
    m_IsInstalledCache.clear();
    if (!m_PackCheckable)
        return;
    /** \todo add installed packages not available in the configured servers. */
    for(int i=0; i< m_AvailPacks.count(); ++i) {
        const Pack &p = m_AvailPacks.at(i);
        if (serverManager()->isDataPackInstalled(p)) {
            m_InstalledPack << p;
            m_IsInstalledCache.append(true);
        } else {
            m_IsInstalledCache.append(false);
        }
    }
}

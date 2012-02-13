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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "packmodel.h"
#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>
#include <datapackutils/packmanager.h>

#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/versionnumber.h>

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
static inline Internal::PackManager *packManager() { return qobject_cast<Internal::PackManager*>(core().packManager()); }
static inline QIcon icon(const QString &name, DataPack::DataPackCore::ThemePath path = DataPack::DataPackCore::MediumPixmaps) { return QIcon(core().icon(name, path)); }

namespace {

const char *const ICON_PACKAGE = "package.png";

struct PackItem {
    PackItem(const Pack &p) :
        pack(p),
        isInstalled(false), isAnUpdate(false),
        fromServerId(-1),
        userCheckState(Qt::Unchecked)
    {}

    bool isAvailableOnServer() const {return fromServerId>=0;}

    Pack pack;
    bool isInstalled, isAnUpdate;
    int fromServerId;
    Qt::CheckState userCheckState;
};

static QString packToHtml(const PackItem &item)
{
    QString inst, color;
    color = "gray";
    if (item.isInstalled) {
        if (item.userCheckState!=Qt::Checked) {
            inst = QCoreApplication::translate("Datapack::PackModel", "Deletion requested");
            color = "red";
        } else {
            inst = tkTr(Trans::Constants::CURRENTLY_INSTALLED);
        }
    } else {
        if (item.userCheckState==Qt::Checked) {
            inst = QCoreApplication::translate("Datapack::PackModel", "Installation requested");
            color = "red";
        } else if (item.isAnUpdate) {
            if (item.userCheckState==Qt::Unchecked) {
                inst = QCoreApplication::translate("Datapack::PackModel", "Deletion requested");
            } else {
                inst = tkTr(Trans::Constants::UPDATE_AVAILABLE);
            }
            color = "blue";
        }
    }
    if (!inst.isEmpty()) {
        inst.prepend(QString("<span style=\"color:%1; font-size:small\">&nbsp;").arg(color));
        inst.append("</span>");
    }
    return QString("<span style=\"color:black;font-weight:bold\">%1</span><br />"
                   "<span style=\"color:gray; font-size:small\">%2: %3</span>%4")
            .arg(item.pack.name())
            .arg(tkTr(Trans::Constants::VERSION))
            .arg(item.pack.description().data(PackDescription::Version).toString())
            .arg(inst);
}

static QString packTooltip(const PackItem &item)
{
    const PackDescription &descr = item.pack.description();
    return QString("<p style=\"font-weight:bold;font-size:large;\">%1</p>"
                      "<p style=\"font-size:small;margin-left:20px;color:gray\">"
                      "%2: %3<br />"
                      "%4: %5<br />"
                      "%6: %7<br />"
                      "%8: %9<br />"
                      "%10: %11"
                      "</p>"
                       )
            .arg(descr.data(PackDescription::Label).toString().replace(" ","&nbsp;"))
            .arg(tkTr(Trans::Constants::VERSION))
            .arg(descr.data(PackDescription::Version).toString())
            .arg(tkTr(Trans::Constants::LAST_MODIFICATION))
            .arg(descr.data(PackDescription::LastModificationDate).toDate().toString("dd MM yyyy"))
            .arg(tkTr(Trans::Constants::AUTHOR))
            .arg(descr.data(PackDescription::Author).toString())
            .arg(tkTr(Trans::Constants::VENDOR))
            .arg(item.pack.vendor())
            .arg(tkTr(Trans::Constants::DATA_TYPE))
            .arg(item.pack.dataTypeName())
            ;
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

namespace DataPack {
namespace Internal {
class PackModelPrivate
{
public:
    PackModelPrivate() :
        m_InstallChecking(false),
        m_PackCheckable(false)
    {}

    // Get all packs available from a server (avoid duplicates) : populate m_AvailPacks list
    void scanServerPack(const int index)
    {
        qWarning() << "Scanning server" << serverManager()->getServerAt(index).uuid();
        foreach(const Pack &p, serverManager()->getPackForServer(serverManager()->getServerAt(index))) {
//            qWarning() << "   ?? " << p.uuid() << p.version();
            // Add to the package list if not already included
            if (!p.isValid())
                continue;
//            qWarning() << "   valid " << p.uuid() << p.version();
            if (m_AvailPacks.contains(p))
                continue;
//            qWarning() << "   adding" << p.uuid() << p.version();
            m_AvailPacks << p;
        }
    }

    // Return the index of the highest version of the pack in the m_AvailPacks list
    int highestVersionPack(const QString &packUuid)
    {
        Utils::VersionNumber highest("0.0.0");
        int id = -1;
        for(int i=0; i < m_AvailPacks.count(); ++i) {
            const Pack &p = m_AvailPacks.at(i);
            if (p.uuid() == packUuid) {
                // keep only highest version
                Utils::VersionNumber testing(p.version());
//                qWarning() << "testing version" << packUuid << testing << highest;
                if (testing > highest) {
                    highest = testing;
                    id = i;
                }
            }
        }
        return id;
    }

    void createPackItem()
    {
        // Get all packages from servers
        QList<Pack> installedPacks = packManager()->installedPack();
        for(int i=0; i < serverManager()->serverCount(); ++i) {
//            qWarning() << "scanning server" << i;
            scanServerPack(i);
        }

        // Add installed package to the availPacks list
//        qWarning() << "Scanning installed";
        foreach(const Pack &p, installedPacks) {
            if (m_AvailPacks.contains(p))
                continue;
            m_AvailPacks << p;
//            qWarning() << "   adding" << p.uuid() << p.version();
        }

        // Keep only highest pack versions
        QList<int> idInUse; // id of Pack to use (from m_AvailPacks)
        QStringList processed;
        for(int i=0; i < m_AvailPacks.count(); ++i) {
            const Pack &p = m_AvailPacks.at(i);
            if (processed.contains(p.uuid()))
                continue;
            idInUse << highestVersionPack(p.uuid());
            processed << p.uuid();
        }

//        qWarning() << idInUse;
//        qWarning() << m_AvailPacks;

        // Create PackItem list
        foreach(int id, idInUse) {
            const Pack &p = m_AvailPacks.at(id);
            PackItem item(p);
            /** \todo keep trace of the server ID/UUID ? */
            item.isInstalled = packManager()->isDataPackInstalled(p);
            if (!item.isInstalled) {
                // Pack is installed with a lower version ?
                bool installedWithLowerVersion = packManager()->isDataPackInstalled(p.uuid());
                if (installedWithLowerVersion) {
                    item.isInstalled = false;
                    item.isAnUpdate = true;
                    item.userCheckState = Qt::PartiallyChecked;
                }
            } else {
                item.userCheckState = Qt::Checked;
            }
            m_Items << item;
        }
    }

    void serverAdded(const int index)
    {
        /** \todo improve this */
        Q_UNUSED(index);
        m_Items.clear();
        m_AvailPacks.clear();
        createPackItem();
    }

    void serverRemoved(const int index)
    {
        /** \todo improve this */
        Q_UNUSED(index);
        m_Items.clear();
        m_AvailPacks.clear();
        createPackItem();
    }

public:
    bool m_InstallChecking, m_PackCheckable;
    QList<PackItem> m_Items;
    QList<Pack> m_AvailPacks;
    Pack m_InvalidPack;
};
}
}


PackModel::PackModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::PackModelPrivate)
{
    setObjectName("DataPack::PackModel");
    d->createPackItem();
//    connect(serverManager(), SIGNAL(serverAdded(int)), this, SLOT(onServerAdded(int)));
    connect(serverManager(), SIGNAL(serverAboutToBeRemoved(int)), this, SLOT(onServerRemoved(int)));
    connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), this, SLOT(updateModel()));
}

PackModel::~PackModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** When setting the installChecker feature to \e on, the model computes the packages dependencies. */
void PackModel::setInstallChecker(const bool onOff)
{
    d->m_InstallChecking = onOff;
    reset();
}

/** Allow user to check package (for installation/desinstallation). */
void PackModel::setPackCheckable(const bool checkable)
{
    d->m_PackCheckable = checkable;
    reset();
}

/** Return true is the model contains user modifications. */
bool PackModel::isDirty() const
{
    foreach(const PackItem &item, d->m_Items) {
        // Installed packs asked for removal ?
        if (item.isInstalled && item.userCheckState!=Qt::Checked)
            return true;
        // Update packs changes
        if (item.isAnUpdate && item.userCheckState!=Qt::PartiallyChecked)
            return true;
        // Not installed packs
        if (!item.isInstalled && item.userCheckState==Qt::Checked)
            return true;
    }
    return false;
}

int PackModel::rowCount(const QModelIndex &) const
{
    return d->m_Items.count();
}

QVariant PackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    if (row < 0 || row >= d->m_Items.count())
        return QVariant();

    if (role==Qt::DisplayRole) {
        switch (index.column()) {
        case Label: return packToHtml(d->m_Items.at(row));
        case IsInstalled: return d->m_Items.at(row).isInstalled;
        case IsAnUpdate: return d->m_Items.at(row).isAnUpdate;
        }
    } else if (role==Qt::ToolTipRole && index.column()==Label) {
        return packTooltip(d->m_Items.at(row));
    } else if (d->m_PackCheckable && role==Qt::CheckStateRole) {
        return d->m_Items.at(row).userCheckState;
    } else if (role==Qt::DecorationRole) {
        QString iconFileName = d->m_Items.at(row).pack.description().data(PackDescription::GeneralIcon).toString();
        if (iconFileName.startsWith(Core::Constants::TAG_APPLICATION_THEME_PATH))
            iconFileName = iconFileName.remove(Core::Constants::TAG_APPLICATION_THEME_PATH);
        if (!iconFileName.isEmpty())
            return icon(iconFileName);
    }
    return QVariant();
}

bool PackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (d->m_PackCheckable && role==Qt::CheckStateRole && index.column()==Label) {
        Qt::CheckState save = d->m_Items[index.row()].userCheckState;
        // Manage a tristate bug in model/view
        if (flags(index) & Qt::ItemIsTristate) {
            int v = (d->m_Items[index.row()].userCheckState + 1) % 3;
            d->m_Items[index.row()].userCheckState = Qt::CheckState(v);
        } else {
            d->m_Items[index.row()].userCheckState = Qt::CheckState(value.toInt());
        }

//        Q_EMIT packStatusChanged(d->m_Items[index.row()].pack, save, d->m_Items[index.row()].userCheckState);
        Q_EMIT dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags PackModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (d->m_PackCheckable && index.column()==Label) {
        f |= Qt::ItemIsUserCheckable;
        if (index.row()>=0 && index.row()<d->m_Items.count()) {
            if (d->m_Items.at(index.row()).isAnUpdate)
                f |= Qt::ItemIsTristate;
        }
    }
    return f;
}

/** Return the package at row \e index */
const Pack &PackModel::packageAt(const int index) const
{
    if (index>=0 && index<d->m_Items.count())
        return d->m_Items.at(index).pack;
    return d->m_InvalidPack;
}

/** Return the list of user selected packages for install. */
QList<Pack> PackModel::packageToInstall() const
{
    QList<Pack> toReturn;
    foreach(const PackItem &it, d->m_Items) {
        if (!it.isInstalled && !it.isAnUpdate && it.userCheckState==Qt::Checked)
            toReturn << it.pack;
    }
    return toReturn;
}

/** Return the list of user selected packages for update. */
QList<Pack> PackModel::packageToUpdate() const
{
    QList<Pack> toReturn;
    foreach(const PackItem &it, d->m_Items) {
        if (it.isAnUpdate && it.userCheckState==Qt::Checked)
            toReturn << it.pack;
    }
    return toReturn;
}

/** Return the list of user selected packages for deletion. */
QList<Pack> PackModel::packageToRemove() const
{
    QList<Pack> toReturn;
    foreach(const PackItem &it, d->m_Items) {
        if (it.isInstalled && it.userCheckState!=Qt::Checked)
            toReturn << it.pack;
    }
    return toReturn;
}

/** Refresh the model. */
void PackModel::updateModel()
{
    d->m_Items.clear();
    d->m_AvailPacks.clear();
    d->createPackItem();
    reset();
}

/** Manage model when a server is added to the server manager */
void PackModel::onServerAdded(const int index)
{
    d->serverAdded(index);
    reset();
}

/** Manage model when a server is about to be removed from the server manager */
void PackModel::onServerRemoved(const int index)
{
    Q_UNUSED(index);
    d->serverRemoved(index);
    reset();
}


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
#include <datapackutils/core.h>
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

static inline DataPack::Core &core() { return DataPack::Core::instance(); }
static inline Internal::ServerManager *serverManager() { return qobject_cast<Internal::ServerManager*>(core().serverManager()); }
static inline QIcon icon(const QString &name, DataPack::Core::ThemePath path = DataPack::Core::MediumPixmaps) { return QIcon(core().icon(name, path)); }

namespace {

const char *const ICON_PACKAGE = "package.png";

static QString packToHtml(const Pack &p)
{
    return QString("<span style=\"color:black;font-weight:bold\">%1</span><br />"
                           "<span style=\"color:gray; font-size:small\">%2: %3</span>")
            .arg(p.description().data(PackDescription::Label).toString())
            .arg(tkTr(Trans::Constants::VERSION))
            .arg(p.description().data(PackDescription::Version).toString());
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
    setObjectName("PackModel");
    getAllAvailablePacks();
    connect(serverManager(), SIGNAL(serverAdded(int)), this, SLOT(updateModel()));
    connect(serverManager(), SIGNAL(serverRemoved(int)), this, SLOT(updateModel()));
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

QVariant PackModel::data(const QModelIndex &index, int role) const
{
//    if (m_PackCheckable && role==Qt::CheckStateRole && index.parent()!=QModelIndex()) {
//        if (QStandardItemModel::data(index, role).isNull())
//            return Qt::Unchecked;
//        return QStandardItemModel::data(index, role);
//    }
//    return QStandardItemModel::data(index, role);
    return QVariant();
}

bool PackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    if (m_PackCheckable && role==Qt::CheckStateRole && index.parent()!=QModelIndex()) {
//        /** \todo check pack dependencies */
//        QStandardItemModel::setData(index, value, role);
//    }
//    return QStandardItemModel::setData(index, value, role);
    return false;
}

Qt::ItemFlags PackModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (m_PackCheckable && index.parent()!=QModelIndex())
        f |= Qt::ItemIsUserCheckable;
    return f;
}

/** Refresh the model using the ServerManager data. */
void PackModel::updateModel()
{
    getAllAvailablePacks();
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

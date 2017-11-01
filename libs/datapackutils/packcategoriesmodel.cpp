/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "packcategoriesmodel.h"
#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>
#include <datapackutils/packmanager.h>

#include <utils/log.h>
#include <utils/versionnumber.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>
#include <translationutils/trans_drugs.h>

#include <QApplication>
#include <QFont>
#include <QString>
#include <QIcon>
#include <QFontMetrics>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }
static inline Internal::ServerManager *serverManager() { return qobject_cast<Internal::ServerManager*>(core().serverManager()); }
static inline Internal::PackManager *packManager() { return qobject_cast<Internal::PackManager*>(core().packManager()); }
static inline QIcon icon(const QString &name, DataPack::DataPackCore::ThemePath path = DataPack::DataPackCore::MediumPixmaps) { return QIcon(core().icon(name, path)); }

namespace {
    const int UnTranslatedNameRole = Qt::UserRole+1;
    const int PackCountRole = Qt::UserRole+2;
    const int PackDataTypeRole = Qt::UserRole+3;
}

namespace DataPack {
namespace Internal {
class PackCategoriesModelPrivate
{
public:
    PackCategoriesModelPrivate(PackCategoriesModel *parent) :
//        m_InstallChecking(false),
//        m_PackCheckable(false),
        q(parent)
    {}

    void clear()
    {
        q->clear();
        _categories.clear();
        m_AvailPacks.clear();
    }

    void createCategories(QString vendor, QStandardItem *rootItem = 0)
    {
        if (!rootItem)
            rootItem = q->invisibleRootItem();

        QStringList subCategories;
        subCategories << "drugs/withddi" << "drugs/withoutddi"
                      << "diseases/icd10"
                      << "forms/fullsets" << "forms/subforms"
                      << "documents"
                      << "zipcodes"
                      << "alertpacks"
                      << "binaries"
                      << "various";

        QStandardItem *main = new QStandardItem(vendor);
        QFont mainFont;
        mainFont.setBold(true);
        mainFont.setPointSize(mainFont.pointSize() + 2);
        main->setFont(mainFont);

        if (vendor==tkTr(Trans::Constants::EHR_COMMUNITY))
            vendor="fmf";

        foreach(const QString &s, subCategories) {
            QStandardItem *parent = 0;
            QString subcat = vendor;

            QStringList items = s.split("/");
            foreach(const QString &sub, items) {
                // already exists
                if (_categories.value(subcat+"/"+sub) != 0) {
                    parent = _categories.value(subcat+"/"+sub);
                    subcat += "/"+sub;
                    continue;
                }

                QStandardItem *subitem = new QStandardItem(sub);
                parent = _categories.value(subcat, parent);
                if (!parent) {
                    main->appendRow(subitem);
                } else {
                    parent->appendRow(subitem);
                }
                parent = subitem;
                subcat += "/" + sub;
                _categories.insert(subcat, subitem);
            }
        }
        _categories.insert(vendor, main);
        rootItem->appendRow(main);

        _categories.value(vendor+"/drugs")->setData(Trans::Constants::DRUGS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/drugs/withddi")->setData(Trans::Constants::DRUGS_WITH_INTERACTIONS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/drugs/withoutddi")->setData(Trans::Constants::DRUGS_WITHOUT_INTERACTIONS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/diseases")->setData(Trans::Constants::DISEASES_CLASSIFICATIONS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/diseases/icd10")->setData(Trans::Constants::DISEASES_ICD, ::UnTranslatedNameRole);
        _categories.value(vendor+"/forms")->setData(Trans::Constants::FORMS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/forms/fullsets")->setData(Trans::Constants::FORMS_FULL_SET, ::UnTranslatedNameRole);
        _categories.value(vendor+"/forms/subforms")->setData(Trans::Constants::FORMS_SUBFORMS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/documents")->setData(Trans::Constants::USER_DOCUMENTS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/zipcodes")->setData(Trans::Constants::ZIP_CODES, ::UnTranslatedNameRole);
        _categories.value(vendor+"/alertpacks")->setData(Trans::Constants::ALERT_PACKS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/binaries")->setData(Trans::Constants::BINARY_PACKS, ::UnTranslatedNameRole);
        _categories.value(vendor+"/various")->setData(Trans::Constants::VARIOUS, ::UnTranslatedNameRole);

        _categories.value(vendor+"/drugs")->setIcon(icon("drugs.png", DataPackCore::SmallPixmaps));
        _categories.value(vendor+"/diseases")->setIcon(icon("chromosome.png", DataPackCore::SmallPixmaps));
        _categories.value(vendor+"/forms")->setIcon(icon("forms.png", DataPackCore::SmallPixmaps));
        _categories.value(vendor+"/documents")->setIcon(icon("user.png", DataPackCore::SmallPixmaps));
        _categories.value(vendor+"/zipcodes")->setIcon(icon("package.png", DataPackCore::SmallPixmaps));
        _categories.value(vendor+"/alertpacks")->setIcon(icon("package.png", DataPackCore::SmallPixmaps));
        _categories.value(vendor+"/binaries")->setIcon(icon("package.png", DataPackCore::SmallPixmaps));
        _categories.value(vendor+"/various")->setIcon(icon("package.png", DataPackCore::SmallPixmaps));

        _categories.value(vendor+"/drugs/withddi")->setData(Pack::DrugsWithInteractions, ::PackDataTypeRole);
        _categories.value(vendor+"/drugs/withoutddi")->setData(Pack::DrugsWithoutInteractions, ::PackDataTypeRole);
        _categories.value(vendor+"/diseases/icd10")->setData(Pack::ICD, ::PackDataTypeRole);
        _categories.value(vendor+"/forms/fullsets")->setData(Pack::FormSubset, ::PackDataTypeRole);
        _categories.value(vendor+"/forms/subforms")->setData(Pack::SubForms, ::PackDataTypeRole);
        _categories.value(vendor+"/documents")->setData(Pack::UserDocuments, ::PackDataTypeRole);
        _categories.value(vendor+"/zipcodes")->setData(Pack::ZipCodes, ::PackDataTypeRole);
        _categories.value(vendor+"/alertpacks")->setData(Pack::AlertPacks, ::PackDataTypeRole);
        _categories.value(vendor+"/binaries")->setData(Pack::Binaries, ::PackDataTypeRole);
        _categories.value(vendor+"/various")->setData(Pack::UnknownType, ::PackDataTypeRole);

        QFont bold;
        bold.setBold(true);
        _categories.value(vendor+"/drugs")->setFont(bold);
        _categories.value(vendor+"/diseases")->setFont(bold);
        _categories.value(vendor+"/forms")->setFont(bold);
        _categories.value(vendor+"/documents")->setFont(bold);
        _categories.value(vendor+"/zipcodes")->setFont(bold);
        _categories.value(vendor+"/alertpacks")->setFont(bold);
        _categories.value(vendor+"/binaries")->setFont(bold);
        _categories.value(vendor+"/various")->setFont(bold);
    }

    void scanServerPack(const int index)
    {
        foreach(const Pack &p, serverManager()->getPackForServer(serverManager()->getServerAt(index))) {
            // Add to the package list if not already included
            if (!p.isValid())
                continue;
            if (m_AvailPacks.contains(p))
                continue;
            m_AvailPacks << p;
        }
    }

    QStandardItem *parentItemForDataType(QString vendor, const int type)
    {
        if (vendor==tkTr(Trans::Constants::EHR_COMMUNITY))
            vendor = "fmf";
        switch (type) {
        case Pack::NoType: vendor.clear(); break;
        case Pack::FormSubset: vendor += "/forms/fullsets"; break;
        case Pack::SubForms: vendor += "/forms/subforms"; break;
        case Pack::DrugsWithInteractions: vendor += "/drugs/withddi"; break;
        case Pack::DrugsWithoutInteractions: vendor += "/drugs/withoutddi"; break;
        case Pack::UserDocuments: vendor += "/documents"; break;
        case Pack::ZipCodes: vendor += "/zipcodes"; break;
        case Pack::ICD: vendor += "/diseases/icd10"; break;
        case Pack::AlertPacks: vendor += "/alertpacks"; break;
        case Pack::Binaries: vendor += "/binaries"; break;
        default: vendor += "/various"; break;
        }
        return _categories.value(vendor, q->invisibleRootItem());
    }

    void removeUnpopulatedCategories(QStandardItem *item = 0)
    {
        if (!item)
            item = q->invisibleRootItem();
        for(int i=item->rowCount()-1; i > -1; --i) {
            if (item->child(i)->data(::PackCountRole).toInt() == 0)
                item->removeRow(i);
            else
                removeUnpopulatedCategories(item->child(i));
        }
    }

    void checkAvailablePack()
    {
        clear();
        // Get all packages from servers
        QList<Pack> installedPacks = packManager()->installedPack();
        for(int i=0; i < serverManager()->serverCount(); ++i) {
            scanServerPack(i);
        }

        // Add installed package to the availPacks list
        foreach(const Pack &p, installedPacks) {
            if (m_AvailPacks.contains(p))
                continue;
            m_AvailPacks << p;
        }

        // Keep only application compatible Packs
        int appId = PackDescription::EHRCompatVersion;
        if (qApp->applicationName().contains("freediams", Qt::CaseInsensitive)) {
            appId = PackDescription::FreeDiamsCompatVersion;
        }
        Utils::VersionNumber appVersion(qApp->applicationVersion());
        for(int i = m_AvailPacks.count()-1; i >= 0; --i) {
            const Pack &p = m_AvailPacks.at(i);
            Utils::VersionNumber packCompatVersion(p.description().data(appId).toString());
            // qWarning() << "pack version" << packCompatVersion << "app" << appVersion << "keep" << p.uuid() << (appVersion >= packCompatVersion);
            if (appVersion < packCompatVersion) // appVersion >= packVersion -> keep it
                m_AvailPacks.removeAt(i);
        }

        // Create categories for all vendors
        QStringList vendors;
        for(int id=0; id < m_AvailPacks.count(); ++id) {
            const Pack &p = m_AvailPacks.at(id);
            const QString &vendor = p.vendor();
            if (vendors.contains(vendor))
                continue;
            vendors << vendor;
            createCategories(vendor);
        }

        // Count Packs for each vendors && categories
        for(int id=0; id < m_AvailPacks.count(); ++id) {
            const Pack &p = m_AvailPacks.at(id);
            QStandardItem *category = parentItemForDataType(p.vendor(), p.dataType());
            int nb = category->data(::PackCountRole).toInt();
            category->setData(++nb, ::PackCountRole);
            QStandardItem *parent = category->parent();
            while (parent!=q->invisibleRootItem() && parent) {
                nb = parent->data(::PackCountRole).toInt();
                parent->setData(++nb, ::PackCountRole);
                parent = parent->parent();
            }
            // TODO: split packs in: installed, available, update available?
        }

        // Remove non-populated categories
        removeUnpopulatedCategories();
    }

    void serverAdded(const int index)
    {
        // TODO: improve this
        Q_UNUSED(index);
        checkAvailablePack();
    }

    void serverRemoved(const int index)
    {
        // TODO: improve this
        Q_UNUSED(index);
        checkAvailablePack();
    }

public:
    QHash<QString, QStandardItem *> _categories;
    QList<Pack> m_AvailPacks;

private:
    PackCategoriesModel *q;
};
}
}

PackCategoriesModel::PackCategoriesModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::PackCategoriesModelPrivate(this))
{
    setObjectName("DataPack::PackCategoriesModel");
    d->createCategories(tkTr(Trans::Constants::EHR_COMMUNITY));
//    connect(serverManager(), SIGNAL(serverAdded(int)), this, SLOT(onServerAdded(int)));
    connect(serverManager(), SIGNAL(serverAboutToBeRemoved(int)), this, SLOT(onServerRemoved(int)));
    connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), this, SLOT(updateModel()));
}

PackCategoriesModel::~PackCategoriesModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

QVariant PackCategoriesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole) {
        QStandardItem *item = itemFromIndex(index);
        QString tr;
        if (index.parent()==QModelIndex()) {
            tr = item->data(role).toString();
        } else {
            tr = tkTr(item->data(::UnTranslatedNameRole).toString().toUtf8());
        }
        int nb = item->data(::PackCountRole).toInt();
        if (nb)
            return QString("%1 (%2)")
                    .arg(tr)
                    .arg(nb);
        return tr;
    } else if (role==Qt::SizeHintRole) {
        QSize size = QStandardItemModel::data(index, role).toSize();
        QFont font = QStandardItemModel::data(index, Qt::FontRole).value<QFont>();
        QFontMetrics fm(font);
        size.setHeight(fm.height() + 10);
        return size;
    }
    return QStandardItemModel::data(index, role);
}

/** Returns the data type of the category at index \e index  */
QList<Pack::DataType> PackCategoriesModel::datatype(const QModelIndex &index) const
{
    QStandardItem *item = itemFromIndex(index);
    QList<Pack::DataType> types;
    if (item) {
        types << Pack::DataType(item->data(::PackDataTypeRole).toInt());
        for(int i=0; i<item->rowCount(); ++i) {
            types << datatype(indexFromItem(item->child(i)));
        }
    }
    return types;
}

/** Returns the vendor name of the \e index */
QString PackCategoriesModel::vendor(const QModelIndex &index) const
{
    // get vendor
    QModelIndex parent = index;
    while (parent.parent() != QModelIndex())
        parent = parent.parent();
    QStandardItem *item = itemFromIndex(parent);
    return item->data(Qt::DisplayRole).toString();
}

/** Refresh the model. */
void PackCategoriesModel::updateModel()
{
    beginResetModel();
    d->checkAvailablePack();
    endResetModel();
}

/** Manage model when a server is added to the server manager */
void PackCategoriesModel::onServerAdded(const int index)
{
    beginResetModel();
    d->serverAdded(index);
    endResetModel();
}

/** Manage model when a server is about to be removed from the server manager */
void PackCategoriesModel::onServerRemoved(const int index)
{
    beginResetModel();
    d->serverRemoved(index);
    endResetModel();
}


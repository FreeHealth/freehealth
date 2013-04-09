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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::Internal::ChequePrintFormatModel
 */

#include "chequeprintformatmodel.h"
#include "chequeprintformat.h"
#include "chequeconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/global.h>
#include <translationutils/constants.h>

#include <QDir>
#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

namespace Tools {
namespace Internal {
class ChequePrintFormatModelPrivate
{
public:
    ChequePrintFormatModelPrivate(ChequePrintFormatModel *parent) :
        q(parent)
    {
    }
    
    ~ChequePrintFormatModelPrivate()
    {
    }
    
    void toItem(const ChequePrintFormat &format)
    {
        QStandardItem *root = new QStandardItem(format.label());
        if (format.isDefault()) {
            QFont bold;
            bold.setBold(true);
            root->setFont(bold);
        }
        q->invisibleRootItem()->appendRow(root);
    }

    void readDatapackFiles()
    {
        _formats.clear();
        QDir dir(settings()->path(Core::ISettings::DataPackInstallPath) + Constants::DATAPACK_PATH);
        if (!dir.exists())
            return;
        QFileInfoList files = Utils::getFiles(dir, "*.xml", Utils::Recursively);
        QList<ChequePrintFormat> formats;
        foreach(const QFileInfo &info, files) {
            formats << ChequePrintFormat::fromXmlFile(info.absoluteFilePath());
        }
        qSort(formats.begin(), formats.end(), ChequePrintFormat::defaultLessThan);
        ChequePrintFormat defaultFormat = formats.takeFirst();
        qSort(formats.begin(), formats.end(), ChequePrintFormat::labelLessThan);
        formats.prepend(defaultFormat);
        foreach(const ChequePrintFormat &format, formats) {
            toItem(format);
        }
        _formats = formats;
    }

public:
    QList<ChequePrintFormat> _formats;
    
private:
    ChequePrintFormatModel *q;
};
} // namespace Internal
} // end namespace Tools


/*! Constructor of the Tools::Internal::ChequePrintFormatModel class */
ChequePrintFormatModel::ChequePrintFormatModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new ChequePrintFormatModelPrivate(this))
{
    setColumnCount(1);
}

/*! Destructor of the Tools::Internal::ChequePrintFormatModel class */
ChequePrintFormatModel::~ChequePrintFormatModel()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ChequePrintFormatModel::initialize()
{
    d->readDatapackFiles();
    return true;
}

Qt::ItemFlags ChequePrintFormatModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

const ChequePrintFormat &ChequePrintFormatModel::chequePrintFormat(const QModelIndex &index) const
{
    QModelIndex parent = index;
    while (parent.isValid()) {
        if (parent.parent().isValid())
            parent = parent.parent();
        else
            break;
    }
    return d->_formats.at(index.row());
}


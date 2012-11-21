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
 * \class DataPackPlugin::Internal::DataPackWidget
 * Widget of the datapack page.
 *
 * \sa DataPackPlugin::Internal::DataPackPage
 */

#include "datapackwidget.h"
#include "datapackcore.h"

#include <utils/widgets/detailswidget.h>
#include <datapackutils/serverdescription.h>
#include <translationutils/constants.h>

#include <QDir>

#include <QDebug>

#include "ui_datapackwidget.h"
#include "ui_serverwidget.h"

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DataPackPlugin::DataPackCore *datapackCore() {return DataPackPlugin::DataPackCore::instance();}

namespace DataPackPlugin {
namespace Internal {
class DataPackWidgetPrivate
{
public:
    DataPackWidgetPrivate(DataPackWidget *parent) :
        q(parent)
    {
        ui = new Ui::DataPackWidget;
    }
    
    ~DataPackWidgetPrivate()
    {
        delete ui;
        qDeleteAll(_sui);
    }
    
public:
    Ui::DataPackWidget *ui;
    QHash<QString, Ui::ServerWidget *> _sui;

private:
    DataPackWidget *q;
};
}  // namespace Internal
} // end namespace DataPackPlugin

/*! Constructor of the DataPackPlugin::Internal::DataPackWidget class */
DataPackWidget::DataPackWidget(QWidget *parent) :
    QWidget(parent),
    d(new DataPackWidgetPrivate(this))
{
    d->ui->setupUi(this);
}

/*! Destructor of the DataPackPlugin::Internal::DataPackWidget class */
DataPackWidget::~DataPackWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*!
 * Initializes the object with the default values: add all registered servers from the datapackcore
 * to the view.
 * Return true if initialization was completed.
 */
bool DataPackWidget::initialize()
{
    QList<DataPackServerQuery> servers = datapackCore()->servers();
    foreach(const DataPackServerQuery &s, servers)
        if (!addServer(s.serverInternalUuid()))
            return false;
    return true;
}

/** Add a server to the view */
bool DataPackWidget::addServer(const QString &serverUid)
{
    const DataPackServerQuery &server = datapackCore()->server(serverUid);
    if (!server.isValid())
        return false;

    // Avoid duplicates
    if (d->_sui.contains(serverUid))
        return true;

    // Create the ui
    QWidget *w = new QWidget(this);
    Ui::ServerWidget *sui = new Ui::ServerWidget;
    d->_sui.insert(serverUid, sui);
    sui->setupUi(w);

    // Populate the ui with server data
    //    add server fields
    sui->internalUid->setText(serverUid);
    sui->installPath->setText(QDir::cleanPath(server.outputServerAbsolutePath()));
    //    add description fields
    const DataPack::ServerDescription &descr = datapackCore()->serverDescription(serverUid);
    sui->descriptionUid->setText(descr.data(DataPack::ServerDescription::Uuid).toString());
    foreach(const QString &lang, descr.availableLanguages()) {
        // create labels
        QLabel *l = new QLabel(this);
        l->setText(QString("%1: %2").arg(lang).arg(descr.data(DataPack::ServerDescription::Label, lang).toString()));
        sui->labelsLayout->addWidget(l);
        // create labels
        QLabel *dc = new QLabel(this);
        dc->setWordWrap(true);
        dc->setText(QString("%1: %2").arg(lang).arg(descr.data(DataPack::ServerDescription::HtmlDescription, lang).toString()));
        sui->descriptionsLayout->addWidget(dc);
    }
    //   add datapacks
    QList<DataPackQuery> packs = datapackCore()->serverRegisteredDatapacks(serverUid);
    foreach(const DataPackQuery &query, packs) {
        QLabel *l = new QLabel(this);
        l->setWordWrap(true);
        l->setText(QString("%1: %2\n"
                           "%3: %4")
                   .arg(tr("File").arg(query.originalContentFileAbsolutePath()))
                   .arg(tr("Description").arg(query.descriptionFileAbsolutePath()))
                   );
        sui->datapacksGroupLayout->addWidget(l);
    }

    // Create the detail widget
    Utils::DetailsWidget *detail = new Utils::DetailsWidget(this);
    detail->setSummaryText(descr.data(DataPack::ServerDescription::Label).toString());
    detail->setWidget(w);
    detail->setSummaryFontBold(true);
    detail->setState(Utils::DetailsWidget::Expanded);
    d->ui->scrollLayout->insertWidget(d->ui->scrollLayout->count() - 1, detail);
    return true;
}

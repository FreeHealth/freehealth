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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DataPackPlugin::Internal::DataPackWidget
 * Widget of the datapack page.
 *
 * \sa DataPackPlugin::Internal::DataPackPage
 */

#include "servercreationwidget.h"
#include "datapackcore.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/widgets/detailswidget.h>
#include <datapackutils/serverdescription.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>

#include <QDir>
#include <QToolButton>

#include <QDebug>

#include "ui_servercreationwidget.h"
#include "ui_serverwidget.h"

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline DataPackPlugin::DataPackCore *datapackCore() {return DataPackPlugin::DataPackCore::instance();}

namespace DataPackPlugin {
namespace Internal {
class DataPackWidgetPrivate
{
public:
    DataPackWidgetPrivate(DataPackWidget *parent) :
        q(parent)
    {
        ui = new Ui::ServerCreationWidget;
    }

    ~DataPackWidgetPrivate()
    {
        delete ui;
        qDeleteAll(_sui);
    }

    // Create the tool buttons for the server actions (refresh datapacks, create server)
    QWidget *createServerButtons(const QString &serverUid, QWidget *parent)
    {
        // container
        QWidget *w = new QWidget(parent);
        QHBoxLayout *layout = new QHBoxLayout(w);
        w->setLayout(layout);
        layout->addStretch(0);

        // buttons
        QToolButton *refreshPacks = new QToolButton(w);
        refreshPacks->setIcon(theme()->icon(Core::Constants::ICON_PACKAGE, Core::ITheme::MediumIcon));
        refreshPacks->setText(QApplication::translate("DataPackWidget", "Refresh datapacks"));
        refreshPacks->setToolTip(refreshPacks->text());
        refreshPacks->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        _toolButtonToServerUid.insert(refreshPacks, serverUid);

        QToolButton *createServer = new QToolButton(w);
        createServer->setIcon(theme()->icon(Core::Constants::ICON_SERVER_CONNECTED, Core::ITheme::MediumIcon)); // TODO: find a better icon
        createServer->setText(QApplication::translate("DataPackWidget", "Create server"));
        createServer->setToolTip(createServer->text());
        createServer->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        _toolButtonToServerUid.insert(createServer, serverUid);

        // add buttons in layout
        layout->addWidget(createServer);
        layout->addWidget(refreshPacks);

        // connect buttons
        QObject::connect(refreshPacks, SIGNAL(clicked()), q, SLOT(refreshServerDatapacks()));
        QObject::connect(createServer, SIGNAL(clicked()), q, SLOT(createServer()));
        return w;
    }

    // Add datapacks labels to the server widget (the one inside the details buttons)
    void addDataPacksToUi(const QString &serverUid, Ui::ServerWidget *sui)
    {
        // remove all previously inserted datapacks
        QList<QLabel*> labels = sui->datapacksGroup->findChildren<QLabel*>();
        for(int i=0; i < labels.count(); ++i) {
            sui->datapacksGroup->layout()->removeWidget(labels.at(i));
            delete labels.at(i);
        }

        // request datapacks from core && update the view
        QList<DataPackQuery> packs = datapackCore()->serverRegisteredDatapacks(serverUid);
        foreach(const DataPackQuery &query, packs) {
            QLabel *l = new QLabel(sui->datapacksGroup);
            l->setWordWrap(true);
            l->setText(QString("%1\n%2")
                       .arg(tkTr(Trans::Constants::_1_COLON_2)
                            .arg(tkTr(Trans::Constants::M_FILE_TEXT).remove("&"))
                            .arg(query.originalContentFileAbsolutePath()))
                       .arg(tkTr(Trans::Constants::_1_COLON_2)
                            .arg(tkTr(Trans::Constants::DESCRIPTION))
                            .arg(query.descriptionFileAbsolutePath()))
                       );
            sui->datapacksGroup->layout()->addWidget(l);
        }
    }

public:
    Ui::ServerCreationWidget *ui;
    QHash<QString, Ui::ServerWidget *> _sui;
    QHash<QToolButton *, QString> _toolButtonToServerUid;

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
    // add server fields
    sui->internalUid->setText(serverUid);
    sui->installPath->setText(QDir::cleanPath(server.outputServerAbsolutePath()));
    sui->descriptionLabel->hide();

    // add description fields
    const DataPack::ServerDescription &descr = datapackCore()->serverDescription(serverUid);
    sui->descriptionUid->setText(descr.data(DataPack::ServerDescription::Uuid).toString());
    foreach(const QString &lang, descr.availableLanguages()) {
        // create labels
        QLabel *l = new QLabel(this);
        l->setText(tkTr(Trans::Constants::_1_COLON_2).arg(lang).arg(descr.data(DataPack::ServerDescription::Label, lang).toString()));
        sui->labelsLayout->addWidget(l);
//        // create descriptions
//        QLabel *dc = new QLabel(this);
//        dc->setWordWrap(true);
//        dc->setText(tkTr(Trans::Constants::_1_COLON_2).arg(lang).arg(descr.data(DataPack::ServerDescription::HtmlDescription, lang).toString()));
//        sui->descriptionsLayout->addWidget(dc);
    }

    // Add datapacks to the view
    d->addDataPacksToUi(serverUid, sui);

    // Create the detail widget
    Utils::DetailsWidget *detail = new Utils::DetailsWidget(this);
    detail->setSummaryText(descr.data(DataPack::ServerDescription::Label).toString());
    detail->setWidget(w);
    detail->setSummaryFontBold(true);
    detail->setState(Utils::DetailsWidget::Expanded);

    w->layout()->addWidget(d->createServerButtons(serverUid, w)); // TODO: use the toolWidget of the DetailsWidget

    // Insert the container in the scrollarea
    d->ui->scrollLayout->insertWidget(d->ui->scrollLayout->count() - 1, detail);
    return true;
}

/**
 * Slot connected to server actions, will refresh all registered datapacks.
 * \sa DataPackPlugin::DataPackCore::refreshServerDataPacks()
 * \internal
 */
bool DataPackWidget::refreshServerDatapacks()
{
    // Get sender serverUid
    QToolButton *button = qobject_cast<QToolButton*>(sender());
    if (!button)
        return false;
    const QString &serverUid = d->_toolButtonToServerUid.value(button);

    // Ask core to update datapacks
    datapackCore()->refreshServerDataPacks(serverUid);

    // Update the server UI
    d->addDataPacksToUi(serverUid, d->_sui.value(serverUid));

    return true;
}

/**
 * Slot connected to server actions, will refresh all registered datapacks and create the server.
 * \sa DataPackPlugin::DataPackCore::refreshServerDataPacks()
 * \internal
 */
bool DataPackWidget::createServer()
{
    // Get sender serverUid
    QToolButton *button = qobject_cast<QToolButton*>(sender());
    if (!button)
        return false;
    const QString &serverUid = d->_toolButtonToServerUid.value(button);
    // Ask core to create datapack server
    datapackCore()->createServer(serverUid);
    return true;
}

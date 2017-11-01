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
 * \class DataPackPlugin::Internal::DataPackModeWidget
 */

#include "datapackmodewidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/pathchooser.h>
#include <translationutils/constants.h>

#include <QProgressDialog>
#include <QPointer>
#include <QVector>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>

#include <QDebug>

#include "ui_datapackmodewidget.h"

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace DataPackPlugin {
namespace Internal {

class DataPackModeWidgetPrivate
{
public:
    DataPackModeWidgetPrivate(DataPackModeWidget *parent) :
        ui(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~DataPackModeWidgetPrivate()
    {
    }

    void setupUi()
    {
        ui = new Ui::DataPackModeWidget();
        ui->setupUi(q);
    }

public:
    Ui::DataPackModeWidget *ui;

private:
    DataPackModeWidget *q;
};
}  // namespace Internal
}  // namespace DataPackPlugin

/*! Constructor of the DataPackPlugin::Internal::DataPackModeWidget class */
DataPackModeWidget::DataPackModeWidget(QWidget *parent) :
    QWidget(parent),
    d(new DataPackModeWidgetPrivate(this))
{
    d->setupUi();
    d->ui->serverPath->setInitialBrowsePathBackup(settings()->path(Core::ISettings::UserDocumentsPath));
    d->ui->serverPath->setPromptDialogTitle(d->ui->selectServerPathLabel->text());
}

//TODO: add a ListView dependencies to this plugin

/*! Destructor of the DataPackPlugin::Internal::DataPackModeWidget class */
DataPackModeWidget::~DataPackModeWidget()
{
    delete d->ui;
    if (d)
        delete d;
    d = 0;
}

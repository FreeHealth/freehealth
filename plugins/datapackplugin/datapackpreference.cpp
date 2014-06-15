/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "datapackpreference.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_menus.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
//#include <extensionsystem/pluginmanager.h>

#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>
#include <datapackutils/widgets/serverpackeditor.h>

#include <QHBoxLayout>

#include "ui_datapackpreference.h"

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }
//static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  DataPackPreferencePage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DataPackPreferencePage::DataPackPreferencePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("DataPackPreferencePage");
}

DataPackPreferencePage::~DataPackPreferencePage()
{
    if (m_Widget) {
        delete m_Widget;
        m_Widget = 0;
    }
}

QString DataPackPreferencePage::id() const { return objectName(); }
QString DataPackPreferencePage::displayName() const { return tkTr(Trans::Constants::DATAPACK_MANAGER); }
QString DataPackPreferencePage::category() const { return tkTr(Trans::Constants::DATAPACK); }
QString DataPackPreferencePage::title() const {return tkTr(Trans::Constants::DATAPACK_MANAGER); }
int DataPackPreferencePage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_MAIN + 20;
}

void DataPackPreferencePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void DataPackPreferencePage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}
void DataPackPreferencePage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_DATABASE_PATHS, QVariant());
//    defaultvalues.insert(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, QString(DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT));

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k).isNull())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

void DataPackPreferencePage::finish() { delete m_Widget; }
QString DataPackPreferencePage::helpPage()
{
//    QString l = QLocale().name().left(2);
//    if (l=="fr")
//        return Constants::H_PREFERENCES_DBSELECTOR_FR;
//    return Constants::H_PREFERENCES_DBSELECTOR_EN;
    // TODO: add a reference to documentation
    return QString();
}

QWidget *DataPackPreferencePage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DataPackPreferenceWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  DataPackPreferenceWidget  ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DataPackPreferenceWidget::DataPackPreferenceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataPackPreference)
{
    ui->setupUi(this);
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setMargin(0);
    setLayout(lay);
    DataPack::ServerPackEditor *packEditor = new DataPack::ServerPackEditor(this);
    lay->addWidget(packEditor);
    setDataToUi();
    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packInstalled(DataPack::Pack)));
}

DataPackPreferenceWidget::~DataPackPreferenceWidget()
{
    delete ui;
}

void DataPackPreferenceWidget::setDataToUi()
{
}

void DataPackPreferenceWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    Core::ISettings *set = s;
//    if (!set) {
//        set = settings();
//    }
//    Utils::Log::addMessage("DataPackPreferenceWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DataPackPreferenceWidget"));
//    set->setValue(DrugsDB::Constants::S_DATABASE_PATHS, QVariant());
//    changeDrugsDatabase(set, DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT);
}

void DataPackPreferenceWidget::saveToSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }
}

void DataPackPreferenceWidget::packInstalled(const DataPack::Pack &pack)
{
    WARN_FUNC << pack;
}

void DataPackPreferenceWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "networkpreferences.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>

#include <utils/log.h>
#include <utils/serializer.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>

#include <QNetworkProxyQuery>
#include <QNetworkProxy>
#include <QUrl>

#include "ui_proxypreferenceswidget.h"

using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

ProxyPreferencesWidget::ProxyPreferencesWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ProxyPreferencesWidget)
{
    setObjectName("ProxyPreferencesWidget");
    ui->setupUi(this);
    ui->proxyUserName->setIcon(theme()->icon(Core::Constants::ICONEYES));
    ui->proxyUserPassword->setIcon(theme()->icon(Core::Constants::ICONEYES));
    ui->proxyUserPassword->toogleEchoMode();
    setDataToUi();
    connect(ui->autoDetect, SIGNAL(clicked()), this, SLOT(autoDetectProxy()));
}

void ProxyPreferencesWidget::setDataToUi()
{
    ui->proxyHostName->clear();
    ui->proxyPort->setValue(0);
    ui->proxyUserName->clear();
    ui->proxyUserPassword->clear();
    const QString &proxyString = settings()->value(Core::Constants::S_PROXY).toString();
    if (!proxyString.isEmpty()) {
        QNetworkProxy proxy;
        if (!Utils::Serializer::deserializeProxy(proxyString, proxy)) {
            LOG_ERROR("Proxy serialized string corrupted");
            return;
        }
        ui->proxyHostName->setText(proxy.hostName());
        ui->proxyPort->setValue(proxy.port());
        ui->proxyUserName->setText(proxy.user());
        ui->proxyUserPassword->setText(proxy.password());
    }
}

void ProxyPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    if (ui->proxyHostName->text().isEmpty()) {
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    } else {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(ui->proxyHostName->text());
        proxy.setPort(ui->proxyPort->value());
        proxy.setUser(ui->proxyUserName->text());
        proxy.setPassword(ui->proxyUserPassword->text());
        s->setValue(Core::Constants::S_PROXY, Utils::Serializer::serializeProxy(proxy));
        QNetworkProxy::setApplicationProxy(proxy);
    }
}

void ProxyPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    //    qWarning() << "---------> writedefaults";
    LOG_FOR("ApplicationGeneralPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FreeDiamsMainWindow"));
}

void ProxyPreferencesWidget::autoDetectProxy()
{
    LOG("Trying to find system proxy.");
    QNetworkProxy proxy;
//    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QNetworkProxyQuery npq(QUrl("http://www.google.com"));
    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);
    foreach(const QNetworkProxy &p, listOfProxies) {
        if (p.type()==QNetworkProxy::HttpProxy && !p.hostName().isEmpty()) {
            LOG("Using proxy " + p.hostName());
            proxy = p;
            break;
        }
    }

    if (proxy.hostName().isEmpty()) {
        ui->proxyHostName->clear();
        ui->proxyPort->setValue(0);
        ui->proxyUserName->clear();
        ui->proxyUserPassword->clear();
    } else {
        ui->proxyHostName->setText(proxy.hostName());
        ui->proxyPort->setValue(proxy.port());
        ui->proxyUserName->setText(proxy.user());
        ui->proxyUserPassword->setText(proxy.password());
    }
}

void ProxyPreferencesWidget::changeEvent(QEvent *e)
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


ProxyPreferencesPage::ProxyPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("ProxyPreferencesPage"); }

ProxyPreferencesPage::~ProxyPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString ProxyPreferencesPage::id() const { return objectName(); }
QString ProxyPreferencesPage::displayName() const { return tkTr(Trans::Constants::PROXY); }
QString ProxyPreferencesPage::title() const {return tkTr(Trans::Constants::INTERNET_CONNECTION_PREFERENCES);}
QString ProxyPreferencesPage::category() const { return tkTr(Trans::Constants::GENERAL_PREFERENCES); }
int ProxyPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_MAIN + 10;
}

void ProxyPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void ProxyPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void ProxyPreferencesPage::finish() { delete m_Widget; }

void ProxyPreferencesPage::checkSettingsValidity()
{
    // Define the ApplicationProxy if needed
    const QString &proxyString = settings()->value(Core::Constants::S_PROXY).toString();
    if (!proxyString.isEmpty()) {
        QNetworkProxy proxy;
        if (!Utils::Serializer::deserializeProxy(proxyString, proxy)) {
            LOG_ERROR("Proxy serialized string corrupted");
            return;
        }
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    }
}

QWidget *ProxyPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new Internal::ProxyPreferencesWidget(parent);
    return m_Widget;
}


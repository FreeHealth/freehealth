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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "userpreferences.h"

#include <icdplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <texteditorplugin/texteditor.h>

#include <printerplugin/printer.h>
#include <printerplugin/textdocumentextra.h>

#include <QPixmap>

using namespace ICD;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  IcdUserOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
IcdUserOptionsPage::IcdUserOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("IcdUserOptionsPage"); }

IcdUserOptionsPage::~IcdUserOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString IcdUserOptionsPage::id() const { return objectName(); }
QString IcdUserOptionsPage::name() const { return tkTr(Trans::Constants::USER); }
QString IcdUserOptionsPage::category() const { return tkTr(Trans::Constants::GENERAL); }

void IcdUserOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void IcdUserOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void IcdUserOptionsPage::finish() { delete m_Widget; }

void IcdUserOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(ICD::Constants::S_USER_HEADER, QVariant());
    defaultvalues.insert(ICD::Constants::S_USER_FOOTER, QVariant());
    defaultvalues.insert(ICD::Constants::S_USER_WATERMARK, QVariant());

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *IcdUserOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new IcdUserWidget(parent);
    return m_Widget;
}

IcdUserWidget::IcdUserWidget(QWidget *parent) :
        QWidget(parent), header(0), footer(0), wm(0)
{
    header = new Print::TextDocumentExtra;
    footer = new Print::TextDocumentExtra;
    wm = new Print::TextDocumentExtra;
    setupUi(this);
    previewer = Print::Printer::previewer(this);
    userLayout->addWidget(previewer);
    setDatasToUi();
}

void IcdUserWidget::setDatasToUi()
{
    header = Print::TextDocumentExtra::fromXml(settings()->value(ICD::Constants::S_USER_HEADER).toString());
    footer = Print::TextDocumentExtra::fromXml(settings()->value(ICD::Constants::S_USER_FOOTER).toString());
    wm = Print::TextDocumentExtra::fromXml(settings()->value(ICD::Constants::S_USER_WATERMARK).toString());

    previewer->setHeader(header);
    previewer->setFooter(footer);
    previewer->setWatermark(wm);
}

void IcdUserWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    Print::TextDocumentExtra *tmp = new Print::TextDocumentExtra;

    previewer->headerToPointer(tmp);
    s->setValue(ICD::Constants::S_USER_HEADER, tmp->toXml());

    previewer->footerToPointer(tmp);
    s->setValue(ICD::Constants::S_USER_FOOTER, tmp->toXml());

    previewer->watermarkToPointer(tmp);
    s->setValue(ICD::Constants::S_USER_WATERMARK, tmp->toXml());
}

void IcdUserWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("IcdUserWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("IcdUserWidget"));
    s->sync();
}

void IcdUserWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

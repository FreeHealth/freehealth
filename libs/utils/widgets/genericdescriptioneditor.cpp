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
#include "genericdescriptioneditor.h"

#include <utils/genericdescription.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include "ui_genericdescriptioneditor.h"

#include <QDebug>

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

GenericDescriptionEditor::GenericDescriptionEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::GenericDescriptionEditor),
    m_desc(0),
    m_LastEditingUpdateIndex(-1)
{
    ui->setupUi(this);
    ui->creationDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    ui->date->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));

    // populate combo
    ui->langSelector->addItems(QStringList() << "xx" << "en" << "fr" << "de" << "es");
    ui->langSelectorUpdate->addItems(QStringList() << "xx" << "en" << "fr" << "de" << "es");
}

GenericDescriptionEditor::~GenericDescriptionEditor()
{
    delete ui;
}

void GenericDescriptionEditor::setDescription(const Utils::GenericDescription &descr)
{
    m_desc = descr;

    // Setup Non translated
    ui->uuid->setText(descr.data(Utils::GenericDescription::Uuid).toString());
    ui->currentVersion->setText(descr.data(Utils::GenericDescription::Version).toString());
    ui->authors->setText(descr.data(Utils::GenericDescription::Author).toString());
    ui->countries->setText(descr.data(Utils::GenericDescription::Country).toString());
    ui->vendor->setText(descr.data(Utils::GenericDescription::Vendor).toString());
    ui->creationDate->setDate(descr.data(Utils::GenericDescription::CreationDate).toDate());
    ui->icon->setText(descr.data(Utils::GenericDescription::GeneralIcon).toString());

    // Setup translated
    ui->url->setText(descr.data(Utils::GenericDescription::URL).toString());
    ui->absFileName->setText(descr.data(Utils::GenericDescription::AbsFileName).toString());
    ui->fmfCompat->setText(descr.data(Utils::GenericDescription::EHRCompatVersion).toString());
    ui->fdCompat->setText(descr.data(Utils::GenericDescription::FreeDiamsCompatVersion).toString());
    m_PreviousLang.clear();
    on_langSelector_activated(ui->langSelector->currentText());

    // Setup Update
    setUpdateInformation();
}

void GenericDescriptionEditor::setUpdateInformation()
{
    ui->updateVersions->clear();
    for(int i = 0; i < m_desc.updateInformation().count(); ++i) {
        ui->updateVersions->addItem(tkTr(Trans::Constants::FROM_1_TO_2)
                                    .arg(m_desc.updateInformation().at(i).fromVersion())
                                    .arg(m_desc.updateInformation().at(i).toVersion()));
    }
    if (m_desc.updateInformation().count() > 0)
        on_updateVersions_activated(0);
    else
        m_LastEditingUpdateIndex = -1;
}

void GenericDescriptionEditor::on_updateVersions_activated(const int index)
{
    int count = m_desc.updateInformation().count();
    if (index < count && index >= 0) {
        // get changes
        if (m_LastEditingUpdateIndex < count && m_LastEditingUpdateIndex >= 0) {
            GenericUpdateInformation info = m_desc.updateInformation().at(m_LastEditingUpdateIndex);;
            info.setFromVersion(ui->from->text());
            info.setToVersion(ui->to->text());
            info.setToVersion(ui->updateAuthor->text());
            info.setIsoDate(ui->date->date().toString(Qt::ISODate));
            info.setAuthor(ui->updateAuthor->text());
            info.setText(ui->updateText->toPlainText(), ui->langSelectorUpdate->currentText());
            m_desc.removeUpdateInformation(m_LastEditingUpdateIndex);
            m_desc.insertUpdateInformation(m_LastEditingUpdateIndex, info);
        }
        // update view
        const GenericUpdateInformation &info = m_desc.updateInformation().at(index);
        ui->from->setText(info.fromVersion());
        ui->to->setText(info.toVersion());
        ui->date->setDate(info.date());
        ui->updateAuthor->setText(info.author());
        ui->updateText->setText(info.text(ui->langSelectorUpdate->currentText()));
        m_LastEditingUpdateIndex = index;
    }
    ui->xml->setText(m_desc.toXml());
}

void GenericDescriptionEditor::on_langSelector_activated(const QString &text)
{
    if (m_PreviousLang.isEmpty()) {
        m_PreviousLang = ui->langSelector->currentText();
    } else {
        // Save changes to our private description
        m_desc.setData(Utils::GenericDescription::Category, ui->category->text(), m_PreviousLang);
        m_desc.setData(Utils::GenericDescription::HtmlDescription, ui->htmlDescr->toHtml(), m_PreviousLang);
        m_desc.setData(Utils::GenericDescription::LicenseName, ui->license->toPlainText(), m_PreviousLang);
        m_desc.setData(Utils::GenericDescription::ShortDescription, ui->shortDescr->toPlainText(), m_PreviousLang);
        m_desc.setData(Utils::GenericDescription::Specialties, ui->spe->text(), m_PreviousLang);
        m_desc.setData(Utils::GenericDescription::ToolTip, ui->tooltip->text(), m_PreviousLang);
        m_PreviousLang = text;
    }
    ui->category->setText(m_desc.data(Utils::GenericDescription::Category, text).toString());
    ui->htmlDescr->setHtml(m_desc.data(Utils::GenericDescription::HtmlDescription, text).toString());
    ui->license->setPlainText(m_desc.data(Utils::GenericDescription::LicenseName, text).toString());
    ui->shortDescr->setPlainText(m_desc.data(Utils::GenericDescription::ShortDescription, text).toString());
    ui->spe->setText(m_desc.data(Utils::GenericDescription::Specialties, text).toString());
    ui->tooltip->setText(m_desc.data(Utils::GenericDescription::ToolTip, text).toString());
}

void GenericDescriptionEditor::on_langSelectorUpdate_activated(const QString &text)
{
    int count = m_desc.updateInformation().count();
    if (m_PreviousUpdateLang.isEmpty()) {
        m_PreviousUpdateLang = ui->langSelectorUpdate->currentText();
    } else {
        // Save changes to our private description
        if (m_LastEditingUpdateIndex < count && m_LastEditingUpdateIndex >= 0) {
            GenericUpdateInformation info = m_desc.updateInformation().at(m_LastEditingUpdateIndex);
            info.setText(ui->updateText->toPlainText(), m_PreviousUpdateLang);
            m_desc.removeUpdateInformation(m_LastEditingUpdateIndex);
            m_desc.insertUpdateInformation(m_LastEditingUpdateIndex, info);
        }
        m_PreviousUpdateLang = text;
    }
    if (m_LastEditingUpdateIndex < count && m_LastEditingUpdateIndex >= 0) {
        const GenericUpdateInformation &info = m_desc.updateInformation().at(m_LastEditingUpdateIndex);
        ui->updateText->setText(info.text(text));
    }
    ui->xml->setText(m_desc.toXml());
}

Utils::GenericDescription GenericDescriptionEditor::submit()
{
    m_desc.setData(Utils::GenericDescription::Uuid, ui->uuid->text());
    m_desc.setData(Utils::GenericDescription::Version, ui->currentVersion->text());
    m_desc.setData(Utils::GenericDescription::Author, ui->authors->text());
    m_desc.setData(Utils::GenericDescription::Country, ui->countries->text());
    m_desc.setData(Utils::GenericDescription::Vendor, ui->vendor->text());
    m_desc.setData(Utils::GenericDescription::CreationDate, ui->creationDate->date());
    m_desc.setData(Utils::GenericDescription::GeneralIcon, ui->icon->text());

    m_desc.setData(Utils::GenericDescription::URL, ui->url->text());
    m_desc.setData(Utils::GenericDescription::AbsFileName, ui->absFileName->text());
    m_desc.setData(Utils::GenericDescription::EHRCompatVersion, ui->fmfCompat->text());
    m_desc.setData(Utils::GenericDescription::FreeDiamsCompatVersion, ui->fdCompat->text());
    m_desc.setData(Utils::GenericDescription::Category, ui->category->text(), ui->langSelector->currentText());
    m_desc.setData(Utils::GenericDescription::HtmlDescription, ui->htmlDescr->toHtml(), ui->langSelector->currentText());
    m_desc.setData(Utils::GenericDescription::LicenseName, ui->license->toPlainText(), ui->langSelector->currentText());
    m_desc.setData(Utils::GenericDescription::ShortDescription, ui->shortDescr->toPlainText(), ui->langSelector->currentText());
    m_desc.setData(Utils::GenericDescription::Specialties, ui->spe->text(), ui->langSelector->currentText());
    m_desc.setData(Utils::GenericDescription::ToolTip, ui->tooltip->text(), ui->langSelector->currentText());

    int count = m_desc.updateInformation().count();
    if (m_LastEditingUpdateIndex < count && m_LastEditingUpdateIndex >= 0) {
        GenericUpdateInformation info = m_desc.updateInformation().at(m_LastEditingUpdateIndex);;
        info.setFromVersion(ui->from->text());
        info.setToVersion(ui->to->text());
        info.setToVersion(ui->updateAuthor->text());
        info.setIsoDate(ui->date->date().toString(Qt::ISODate));
        info.setAuthor(ui->updateAuthor->text());
        info.setText(ui->updateText->toPlainText(), ui->langSelectorUpdate->currentText());
        m_desc.removeUpdateInformation(m_LastEditingUpdateIndex);
        m_desc.insertUpdateInformation(m_LastEditingUpdateIndex, info);
    }

    return m_desc;
}

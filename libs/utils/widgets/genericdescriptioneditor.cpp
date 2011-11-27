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
#include "genericdescriptioneditor.h"

#include <utils/genericdescription.h>

#include "ui_genericdescriptioneditor.h"

#include <QDebug>

using namespace Utils;
using namespace Internal;

GenericDescriptionEditor::GenericDescriptionEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenericDescriptionEditor),
    m_desc(0)
{
    ui->setupUi(this);
    // populate combo
    ui->langSelector->addItems(QStringList() << "xx" << "en" << "fr" << "de" << "es");
}

GenericDescriptionEditor::~GenericDescriptionEditor()
{
    delete ui;
}

void GenericDescriptionEditor::setDescription(const Utils::GenericDescription &descr)
{
    m_desc = descr;
    qWarning() << descr.toXml();

    // Setup ui
    ui->uuid->setText(descr.data(Utils::GenericDescription::Uuid).toString());
    ui->currentVersion->setText(descr.data(Utils::GenericDescription::Version).toString());
    ui->authors->setText(descr.data(Utils::GenericDescription::Author).toString());
    ui->countries->setText(descr.data(Utils::GenericDescription::Country).toString());
    ui->vendor->setText(descr.data(Utils::GenericDescription::Vendor).toString());
    ui->creationDate->setDate(descr.data(Utils::GenericDescription::CreationDate).toDate());
    ui->icon->setText(descr.data(Utils::GenericDescription::GeneralIcon).toString());

    ui->url->setText(descr.data(Utils::GenericDescription::URL).toString());
    ui->absFileName->setText(descr.data(Utils::GenericDescription::AbsFileName).toString());
    ui->fmfCompat->setText(descr.data(Utils::GenericDescription::FreeMedFormsCompatVersion).toString());
    ui->fdCompat->setText(descr.data(Utils::GenericDescription::FreeDiamsCompatVersion).toString());
    ui->faCompat->setText(descr.data(Utils::GenericDescription::FreeAccountCompatVersion).toString());
    on_langSelector_activated(ui->langSelector->currentText());
}

void GenericDescriptionEditor::on_langSelector_activated(const QString &text)
{
//    Q_ASSERT(m_desc);
//    if (!m_desc)
//        return;
    ui->category->setText(m_desc.data(Utils::GenericDescription::Category, text).toString());
    ui->htmlDescr->setHtml(m_desc.data(Utils::GenericDescription::HtmlDescription, text).toString());
    ui->license->setPlainText(m_desc.data(Utils::GenericDescription::GlobalLicense, text).toString());
    ui->shortDescr->setPlainText(m_desc.data(Utils::GenericDescription::ShortDescription, text).toString());
    ui->spe->setText(m_desc.data(Utils::GenericDescription::Specialties, text).toString());
    ui->tooltip->setText(m_desc.data(Utils::GenericDescription::ToolTip, text).toString());
}

bool GenericDescriptionEditor::submit()
{
    return true;
}

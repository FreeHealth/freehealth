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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "genericupdateinformationeditor.h"

#include <translationutils/constants.h>

#include "ui_genericupdateinformationeditor.h"

using namespace Utils;
using namespace Trans::ConstantTranslations;

GenericUpdateInformationEditor::GenericUpdateInformationEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenericUpdateInformationEditor)
{
    ui->setupUi(this);
    ui->date->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    ui->langSelector->addItems(QStringList() << "xx" << "en" << "fr" << "de" << "es");
}

GenericUpdateInformationEditor::~GenericUpdateInformationEditor()
{
    delete ui;
}

void GenericUpdateInformationEditor::setUpdateInformation(const Utils::GenericUpdateInformation &info)
{
    m_info = info;

    on_langSelector_activated(ui->langSelector->currentText());
}

void GenericUpdateInformationEditor::on_langSelector_activated(const QString &text)
{
    if (m_PreviousLang.isEmpty()) {
        m_PreviousLang = ui->langSelector->currentText();
    } else {
        // Save changes to our private description
        m_info.setText(ui->updateText->toHtml(), m_PreviousLang);
        m_PreviousLang = text;
    }
    ui->updateText->setHtml(m_info.text(text));
}

GenericUpdateInformation GenericUpdateInformationEditor::submit()
{
    m_info.setFromVersion(ui->from->text());
    m_info.setToVersion(ui->to->text());
    m_info.setIsoDate(ui->date->date().toString(Qt::ISODate));
    m_info.setText(ui->updateText->toHtml(), ui->langSelector->currentText());
    return m_info;
}

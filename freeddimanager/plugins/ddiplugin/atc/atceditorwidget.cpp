/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DDI::AtcEditorWidget
 * ATC classification visualisation page.
 */

#include "atceditorwidget.h"
#include "atctablemodel.h"
#include <ddiplugin/ddicore.h>

#include "ui_atceditorwidget.h"

#include <QDataWidgetMapper>
#include <QDebug>

using namespace DDI;
using namespace Internal;
//using namespace Trans::ConstantTranslations;

static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}

namespace DDI {
namespace Internal {
class AtcEditorWidgetPrivate
{
public:
    AtcEditorWidgetPrivate(AtcEditorWidget *parent) :
        ui(0),
        _atcTableModel(0),
        _mapper(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~AtcEditorWidgetPrivate()
    {
        delete ui;
    }

public:
    Ui::AtcEditorWidget *ui;
    AtcTableModel *_atcTableModel;
    QDataWidgetMapper *_mapper;

private:
    AtcEditorWidget *q;
};
}
}
AtcEditorWidget::AtcEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new AtcEditorWidgetPrivate(this))
{
    d->ui = new Ui::AtcEditorWidget;
    d->ui->setupUi(this);
    d->_mapper = new QDataWidgetMapper(this);
}

AtcEditorWidget::~AtcEditorWidget()
{
    if (d)
        delete d;
    d = 0;
}

/** Clear the editor (reset the mapper & clear the ui editors) */
void AtcEditorWidget::clear()
{
    d->_mapper->setCurrentModelIndex(QModelIndex());
    d->ui->code->setText("");
    d->ui->fr->setText("");
    d->ui->en->setText("");
    d->ui->de->setText("");
    d->ui->sp->setText("");
//    d->ui->dateCreation->setDate(QDate());
//    d->ui->dateUpdate->setDate(QDate());
//    d->ui->whoYear->setDate(QDate());
    d->ui->comment->setPlainText("");
}

/** Define the atc model to use in this editor */
void AtcEditorWidget::setAtcTableModel(AtcTableModel *model)
{
    d->_atcTableModel = model;
    d->_mapper->setModel(model);
    d->_mapper->addMapping(d->ui->code, AtcTableModel::Code, "text");
    d->_mapper->addMapping(d->ui->uid, AtcTableModel::Uid, "text");
    d->_mapper->addMapping(d->ui->fr, AtcTableModel::LabelFr, "text");
    d->_mapper->addMapping(d->ui->en, AtcTableModel::LabelEn, "text");
    d->_mapper->addMapping(d->ui->de, AtcTableModel::LabelDe, "text");
    d->_mapper->addMapping(d->ui->sp, AtcTableModel::LabelEs, "text");
    d->_mapper->addMapping(d->ui->dateCreation, AtcTableModel::DateCreation, "text");
    d->_mapper->addMapping(d->ui->dateUpdate, AtcTableModel::DateUpdate, "date");
    d->_mapper->addMapping(d->ui->whoYear, AtcTableModel::WhoUpdateYear, "date");
    d->_mapper->addMapping(d->ui->comment, AtcTableModel::Comment, "text");
}

/** Set the current editing ATC to \e index */
void AtcEditorWidget::setCurrentIndex(const QModelIndex &index)
{
    qWarning() << "AtcEditorWidget::setCurrentIndex" << index << d->_atcTableModel << index.isValid();
    clear();
    if (!index.isValid())
        return;
    d->_mapper->setCurrentIndex(index.row());
}

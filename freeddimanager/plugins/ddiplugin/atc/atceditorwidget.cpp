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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DDI::AtcEditorWidget
 * ATC classification editor widget.
 * Data mapper over the DDI::AtcTableModel for one row
 */

#include "atceditorwidget.h"
#include "atctablemodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/constants.h>
#include <ddiplugin/database/ddidatabase.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/widgets/uppercasevalidator.h>
#include "ui_atceditorwidget.h"

#include <QDataWidgetMapper>
#include <QDebug>

using namespace DDI;
using namespace Internal;
//using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}
static inline DDI::Internal::DDIDatabase &ddiBase() {return DDI::DDICore::instance()->database();}

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
    d->ui->dateCreation->setClearIcon(theme()->iconFullPath(Core::Constants::ICONCLEARLINEEDIT));
    d->ui->dateCreation->setDateIcon(theme()->iconFullPath(Core::Constants::ICONDATE));
    d->ui->dateUpdate->setClearIcon(theme()->iconFullPath(Core::Constants::ICONCLEARLINEEDIT));
    d->ui->dateUpdate->setDateIcon(theme()->iconFullPath(Core::Constants::ICONDATE));
    d->ui->whoYear->setClearIcon(theme()->iconFullPath(Core::Constants::ICONCLEARLINEEDIT));
    d->ui->whoYear->setDateIcon(theme()->iconFullPath(Core::Constants::ICONDATE));
    d->ui->code->setValidator(new Utils::UpperCaseValidator(this));
    d->ui->fr->setValidator(new Utils::UpperCaseValidator(this));
    d->ui->en->setValidator(new Utils::UpperCaseValidator(this));
    d->ui->de->setValidator(new Utils::UpperCaseValidator(this));
    d->ui->sp->setValidator(new Utils::UpperCaseValidator(this));

    d->_mapper = new QDataWidgetMapper(this);
    d->_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
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
    d->ui->dateCreation->setDate(QDate());
    d->ui->dateUpdate->setDate(QDate());
    d->ui->whoYear->setDate(QDate());
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
    d->_mapper->addMapping(d->ui->sp, AtcTableModel::LabelSp, "text");
    d->_mapper->addMapping(d->ui->dateCreation, AtcTableModel::DateCreation, "date");
    d->_mapper->addMapping(d->ui->dateUpdate, AtcTableModel::DateUpdate, "date");
    d->_mapper->addMapping(d->ui->whoYear, AtcTableModel::WhoUpdateYear, "date");
    d->_mapper->addMapping(d->ui->comment, AtcTableModel::Comment, "text");
}

/** Enabled/Disabled the edition of the item */
void AtcEditorWidget::setEditorEnabled(bool enabled)
{
    d->ui->code->setEnabled(enabled);
    d->ui->uid->setEnabled(enabled);
    d->ui->fr->setEnabled(enabled);
    d->ui->en->setEnabled(enabled);
    d->ui->de->setEnabled(enabled);
    d->ui->sp->setEnabled(enabled);
    d->ui->dateCreation->setEnabled(enabled);
    d->ui->dateUpdate->setEnabled(enabled);
    d->ui->whoYear->setEnabled(enabled);
    d->ui->comment->setEnabled(enabled);
}

/** Returns \e true if the editor is enabled. */
bool AtcEditorWidget::isEditorEnabled() const
{
    return d->ui->code->isEnabled();
}

/** Set the current editing ATC to \e index */
void AtcEditorWidget::setCurrentIndex(const QModelIndex &index)
{
    clear();
    if (!index.isValid())
        return;

    // Populate the mapper with the index
    d->_mapper->setCurrentIndex(index.row());
    populateParents();
}

/** Submit modifications to the model */
bool AtcEditorWidget::submit()
{
    return d->_mapper->submit();
}

void AtcEditorWidget::populateParents()
{
    d->ui->parents->clear();
    QString code = d->ui->code->text();
    int i = 0;
    QString parent;
    while (parent.size() < code.size()) {
        switch (i) { // A DD A A DD (switch 0 1 2 3 4)
        case 0: parent = code.left(1); break;
        case 1: parent = code.left(3); break;
        case 2: parent = code.left(4); break;
        case 3: parent = code.left(5); break;
        case 4: parent = code.left(7); break;
        }
        ++i;
        d->ui->parents->addItem(QString("%1 - %2")
                                .arg(parent)
                                .arg(ddiBase().atcLabelForCode(parent, QLocale().name().left(2))));
    }
}

#ifdef WITH_TESTS
#include <coreplugin/imainwindow.h>
#include <utils/randomizer.h>
#include <QTest>

static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }

void AtcEditorWidget::test_runAllTests()
{
    test_views();
    test_actions();
    test_itemCreation();
    test_edition();
}

void AtcEditorWidget::test_views()
{
}

void AtcEditorWidget::test_editorEnabledState(bool state)
{
    Q_UNUSED(state);
}

void AtcEditorWidget::test_actions()
{
}

void AtcEditorWidget::test_itemCreation()
{
}

void AtcEditorWidget::test_edition()
{
}

#endif // WITH_TESTS

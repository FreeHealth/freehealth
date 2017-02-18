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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Script::Internal::ScriptWriterDialog
 */

#include "scriptwriterdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>

#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>

#include <QAction>
#include <QToolBar>

#include <QDebug>

#include "ui_scriptwriterdialog.h"

using namespace Script;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IScriptManager *scriptManager()  { return Core::ICore::instance()->scriptManager(); }

namespace Script {
namespace Internal {
class ScriptWriterDialogPrivate
{
public:
    ScriptWriterDialogPrivate(ScriptWriterDialog *parent) :
        ui(new Ui::ScriptWriterDialog),
        aReadFile(0),
        aSaveFile(0),
        aExecute(0),
        _toolBar(0),
        _initialized(false),
        q(parent)
    {
    }

    ~ScriptWriterDialogPrivate()
    {
        delete ui;
    }

    void setupUi()
    {
       ui->setupUi(q);
       ui->toolBarLayout->setMargin(0);
       ui->toolBarLayout->setSpacing(0);
    }

    void createActions()
    {
        aReadFile = new QAction(q);
        aSaveFile = new QAction(q);
        aExecute = new QAction(q);
        aReadFile->setText(tkTr(Trans::Constants::FILEOPEN_TEXT));
        aSaveFile->setText(tkTr(Trans::Constants::FILESAVE_TEXT));
        aExecute->setText("Execute");
        aReadFile->setToolTip(aReadFile->text());
        aSaveFile->setToolTip(aSaveFile->text());
        aExecute->setToolTip(aExecute->text());
    }

    void createToolBar()
    {
        _toolBar = new QToolBar(q);
        _toolBar->addAction(aReadFile);
        _toolBar->addAction(aSaveFile);
        _toolBar->addSeparator();
        _toolBar->addAction(aExecute);
        ui->toolBarLayout->addWidget(_toolBar);
    }

    void connectActions()
    {
        QObject::connect(aReadFile, SIGNAL(triggered()), q, SLOT(onReadFileTriggered()));
        QObject::connect(aSaveFile, SIGNAL(triggered()), q, SLOT(onSaveFileTriggered()));
        QObject::connect(aExecute, SIGNAL(triggered()), q, SLOT(onExecuteScriptTriggered()));
    }

public:
    Ui::ScriptWriterDialog *ui;
    QAction *aReadFile, *aSaveFile, *aExecute;
    QToolBar *_toolBar;
    bool _initialized;

private:
    ScriptWriterDialog *q;
};
} // namespace Internal
} // end namespace Script


/*! Constructor of the Script::Internal::ScriptWriterDialog class */
ScriptWriterDialog::ScriptWriterDialog(QWidget *parent) :
    QDialog(parent),
    d(new ScriptWriterDialogPrivate(this))
{
    setObjectName("ScriptWriterDialog");
    setWindowTitle(tr("Script writer"));
}

/*! Destructor of the Script::Internal::ScriptWriterDialog class */
ScriptWriterDialog::~ScriptWriterDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ScriptWriterDialog::initialize()
{
    if (d->_initialized)
        return true;
    d->setupUi();
    layout()->setMargin(0);
    layout()->setSpacing(0);
    d->createActions();
    d->createToolBar();
    d->connectActions();
    d->_initialized = true;
    return true;
}

void ScriptWriterDialog::onReadFileTriggered()
{}

void ScriptWriterDialog::onSaveFileTriggered()
{}

void ScriptWriterDialog::onExecuteScriptTriggered()
{
    scriptManager()->evaluate(d->ui->scriptEdit->toPlainText());
}

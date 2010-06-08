/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "formfilesselectorwidget.h"
#include "ui_formfilesselectorwidget.h"
#include "iformio.h"

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <QFileSystemModel>

#include <QDebug>

using namespace Form::Internal;

//inline static Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
inline static ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
inline static QList<Form::IFormIO*> refreshIOPlugs() {return pluginManager()->getObjects<Form::IFormIO>();}


FormFilesSelectorWidget::FormFilesSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::FormFilesSelectorWidget)
{
    ui->setupUi(this);
    // get IFormIO
    ios = refreshIOPlugs();
    // construct filters
    QStringList filters;
    foreach(const Form::IFormIO *io, ios) {
        filters.append(io->fileFilters());
    }

    dirModel = new QFileSystemModel(this);
//    dirModel->setReadOnly(true);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    dirModel->setRootPath(settings()->path(Core::ISettings::SampleFormsPath));
//    dirModel->setNameFilters(filters);

    ui->listView->setModel(dirModel);
    ui->listView->setRootIndex(dirModel->index(settings()->path(Core::ISettings::SampleFormsPath)));

    QFont bold;
    bold.setBold(true);
    for(int i=0; i < dirModel->rowCount(dirModel->index(settings()->path(Core::ISettings::SampleFormsPath))); ++i) {
        QModelIndex index = dirModel->index(i,0);
        const QString &file = index.data().toString();
        foreach(Form::IFormIO *io, ios) {
            if (io->setFileName(settings()->path(Core::ISettings::SampleFormsPath) + QDir::separator() + file) && io->canReadFile()) {
                dirModel->setData(index, bold, Qt::FontRole);
            }
        }
    }
}

FormFilesSelectorWidget::~FormFilesSelectorWidget()
{
    delete ui;
}

void FormFilesSelectorWidget::on_useButton_clicked()
{
    if (!ui->listView->selectionModel()->hasSelection())
        return;
}

void FormFilesSelectorWidget::on_listView_activated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    // get the fileName
    const QString &file = ui->listView->currentIndex().data().toString();
    // ask ios to generate description in the treeview
    foreach(Form::IFormIO *io, ios) {
        if (io->setFileName(settings()->path(Core::ISettings::SampleFormsPath) + QDir::separator() + file) && io->canReadFile()) {
            io->readFileInformations();
            io->formDescriptionToTreeWidget(ui->treeWidget, QLocale().name().left(2));
            return;
        }
    }
    ui->treeWidget->clear();
}

void FormFilesSelectorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (ui)
            ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

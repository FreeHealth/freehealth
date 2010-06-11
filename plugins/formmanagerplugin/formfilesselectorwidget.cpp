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
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QModelIndex>

#include <QDebug>

using namespace Form;
using namespace Internal;

//inline static Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
inline static ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
inline static QList<Form::IFormIO*> refreshIOPlugs() {return pluginManager()->getObjects<Form::IFormIO>();}


namespace Form {
namespace Internal {
class FormFilesIcons : public QFileIconProvider
{
public:
    FormFilesIcons() : QFileIconProvider() {}

    QIcon icon(const QFileInfo &info) const
    {
        if (!m_Files.contains(info.fileName()))
            testFile(info);

        if (m_Files.value(info.fileName()))
            return theme()->icon(Core::Constants::ICONOK);
        else
            return theme()->icon(Core::Constants::ICONHELP);

        return QIcon();
    }

private:
    void testFile(const QFileInfo &file) const
    {
        QList<Form::IFormIO*> ios = refreshIOPlugs();
        const QString &path = settings()->path(Core::ISettings::SampleFormsPath);
        const QString &fileName = file.fileName();
        foreach(Form::IFormIO *io, ios) {
            io->muteUserWarnings(true);
            if (io->setFileName(path + QDir::separator() + fileName) && io->canReadFile()) {
                m_Files.insert(file.fileName(), true);
                return;
            }
        }
        m_Files.insert(file.fileName(), false);
    }

    mutable QHash<QString, bool> m_Files;
};

class FormFilesSelectorWidgetPrivate
{
public:
    FormFilesSelectorWidgetPrivate() : ui(new Ui::FormFilesSelectorWidget) {}

    ~FormFilesSelectorWidgetPrivate()
    {
        delete ui;
        delete m_IconProvider;
    }

public:
    Ui::FormFilesSelectorWidget *ui;
    QFileSystemModel *dirModel;
    FormFilesIcons *m_IconProvider;
    QList<Form::IFormIO*> ios;
};

}  // End namespace Internal
}  // End namespace Form



FormFilesSelectorWidget::FormFilesSelectorWidget(QWidget *parent) :
    QWidget(parent),
    d(new FormFilesSelectorWidgetPrivate)
{
    d->ui->setupUi(this);
    d->ui->actualFile->setText(settings()->value(Core::Constants::S_PATIENTFORMS_FILENAME).toString());
    // get IFormIO
    d->ios = refreshIOPlugs();
    // construct filters
    QStringList filters;
    foreach(const Form::IFormIO *io, d->ios) {
        filters.append(io->managedFileExtension());
    }

    d->dirModel = new QFileSystemModel(this);
//    d->dirModel->setReadOnly(true);
    d->dirModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    d->dirModel->setIconProvider(d->m_IconProvider = new Internal::FormFilesIcons());
    d->dirModel->setRootPath(settings()->path(Core::ISettings::SampleFormsPath));
//    d->dirModel->setNameFilters(filters);

    d->ui->listView->setModel(d->dirModel);
    d->ui->listView->setRootIndex(d->dirModel->index(settings()->path(Core::ISettings::SampleFormsPath)));

    connect(d->ui->listView, SIGNAL(activated(QModelIndex)),this, SLOT(on_listView_activated(QModelIndex)));
    connect(d->ui->useButton, SIGNAL(clicked()),this, SLOT(on_useButton_clicked()));
}

FormFilesSelectorWidget::~FormFilesSelectorWidget()
{
    delete d;
}

void FormFilesSelectorWidget::on_useButton_clicked()
{
    if (!d->ui->listView->selectionModel()->hasSelection())
        return;
    QFileInfo selected = d->dirModel->fileInfo(d->ui->listView->currentIndex());
    settings()->setValue(Core::Constants::S_PATIENTFORMS_FILENAME, selected.absoluteFilePath());
}

void FormFilesSelectorWidget::on_listView_activated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    d->ui->treeWidget->clear();
    // get the fileName
    const QString &file = d->ui->listView->currentIndex().data().toString();
    // ask ios to generate description in the treeview
    foreach(Form::IFormIO *io, d->ios) {
        if (io->setFileName(settings()->path(Core::ISettings::SampleFormsPath) + QDir::separator() + file)) {
            if (io->canReadFile()) {
                io->readFileInformations();
                io->formDescriptionToTreeWidget(d->ui->treeWidget, QLocale().name().left(2));
                return;
            } else {
                QTreeWidgetItem *item = new QTreeWidgetItem(d->ui->treeWidget, QStringList() << io->name());
                item->setExpanded(true);
                QFont bold;
                bold.setBold(true);
                item->setFont(0, bold);
                foreach(const QString &err, io->lastError().split("\n"))
                    new QTreeWidgetItem(item, QStringList() << err);
            }
        }
    }
}

void FormFilesSelectorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (d->ui)
            d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

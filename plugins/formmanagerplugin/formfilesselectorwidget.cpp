/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
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
#include <coreplugin/constants_tokensandsettings.h>

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
        if (!m_Files.contains(info.absoluteFilePath()))
            testFile(info);

        if (m_Files.value(info.absoluteFilePath()))
            return theme()->icon(Core::Constants::ICONOK);
        else
            return theme()->icon(Core::Constants::ICONHELP);

        return QIcon();
    }

private:
    void testFile(const QFileInfo &file) const
    {
        QList<Form::IFormIO*> ios = refreshIOPlugs();
        foreach(Form::IFormIO *io, ios) {
            io->muteUserWarnings(true);
            if (io->setFileName(file.absoluteFilePath()) && io->canReadFile()) {
                m_Files.insert(file.absoluteFilePath(), true);
                return;
            }
        }
        m_Files.insert(file.absoluteFilePath(), false);
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
    QString actual = settings()->value(Core::Constants::S_PATIENTFORMS_FILENAME).toString();
    actual = actual.mid(settings()->path(Core::ISettings::CompleteFormsPath).length() + 1);
    d->ui->actualFile->setText(actual);
    // get IFormIO
    d->ios = refreshIOPlugs();
    // construct filters
    QStringList filters;
    foreach(const Form::IFormIO *io, d->ios) {
        filters.append(io->managedFileExtension());
    }

    d->dirModel = new QFileSystemModel(this);
//    d->dirModel->setReadOnly(true);
    d->dirModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    d->dirModel->setIconProvider(d->m_IconProvider = new Internal::FormFilesIcons());
    d->dirModel->setRootPath(settings()->path(Core::ISettings::CompleteFormsPath));
//    d->dirModel->setNameFilters(filters);

    d->ui->listView->setModel(d->dirModel);
    d->ui->listView->setRootIndex(d->dirModel->index(settings()->path(Core::ISettings::CompleteFormsPath)));

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
        if (io->setFileName(settings()->path(Core::ISettings::CompleteFormsPath) + QDir::separator() + file)) {
            if (io->canReadFile()) {
                Form::FormIODescription desc = io->readFileInformations();
                desc.formDescriptionToTreeWidget(d->ui->treeWidget);
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

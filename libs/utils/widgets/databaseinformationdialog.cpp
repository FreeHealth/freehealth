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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Utils::DatabaseInformationDialog
 * Database information dialog (read-only data).
 */

#include "databaseinformationdialog.h"

#include <utils/global.h>
#include <utils/database.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QFileDialog>

#include <QDebug>

using namespace Utils;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {
class DatabaseInformationDialogPrivate
{
public:
    DatabaseInformationDialogPrivate(DatabaseInformationDialog *parent) :
        _headerTree(0),
        _contentTree(0),
        _title(0),
        q(parent)
    {
    }
    
    ~DatabaseInformationDialogPrivate()
    {
    }

    QString getTreeItemText(QTreeWidgetItem *item, int indent = 0)
    {
        if (!item)
            return QString::null;
        QString text;
        if (item->columnCount() == 2) {
            text = QString("%1%2\n%3\n")
                    .arg(QString().fill(' ', indent))
                    .arg(item->text(0).leftJustified(35, ' '))
                    .arg(Utils::indentString(item->text(1), 10+indent));
        } else {
            text = Utils::indentString(item->text(0), indent) + "\n";
        }
        for(int i=0; i < item->childCount(); ++i)
            text += getTreeItemText(item->child(i), indent + 4);
        return text;
    }
    
public:
    QTreeWidget *_headerTree, *_contentTree;
    QLabel *_title;

private:
    DatabaseInformationDialog *q;
};
} // namespace Internal
} // end namespace UserPlugin


/*! Constructor of the UserPlugin::DatabaseInformationDialog class */
DatabaseInformationDialog::DatabaseInformationDialog(QWidget *parent) :
    QDialog(parent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint),
    d(new Internal::DatabaseInformationDialogPrivate(this))
{
    QGridLayout *lay = new QGridLayout(this);

    d->_title = new QLabel(this);
    QFont bold;
    bold.setBold(true);
    d->_title->setFont(bold);
    d->_title->setAlignment(Qt::AlignCenter);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    d->_headerTree = new QTreeWidget(this);
    d->_headerTree->setColumnCount(2);
    d->_headerTree->header()->hide();
    d->_headerTree->hide();
    d->_headerTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    d->_contentTree = new QTreeWidget(this);
    d->_contentTree->setColumnCount(2);
    d->_contentTree->header()->hide();
    d->_contentTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    QDialogButtonBox *box = new QDialogButtonBox(this);
    box->setStandardButtons(QDialogButtonBox::Ok);
    QPushButton *save = box->addButton(tr("Save this report"), QDialogButtonBox::ActionRole);
    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(save, SIGNAL(clicked()), this, SLOT(saveContent()));

    setTitle(tkTr(Trans::Constants::DATABASE_INFORMATION));

    lay->addWidget(d->_title);
    lay->addWidget(line);
    lay->addWidget(d->_headerTree);
    lay->addWidget(d->_contentTree);
    lay->addWidget(box);
    setLayout(lay);
    Utils::resizeAndCenter(this, parent);
}

/*! Destructor of the UserPlugin::DatabaseInformationDialog class */
DatabaseInformationDialog::~DatabaseInformationDialog()
{
    if (d)
        delete d;
    d = 0;
}

/** Define the title of the dialog */
void DatabaseInformationDialog::setTitle(const QString &title)
{
    d->_title->setText(title);
    setWindowTitle(title);
}

/**
 * Set the database to use for the dialog (populate the content qtreewidget).
 * If you want to set yourself the content of the tree widget
 * use the getHeaderTreeWidget() and getDescriptionTreeWidget().
 */
bool DatabaseInformationDialog::setDatabase(const Utils::Database &database)
{
    if (!d->_contentTree)
        return false;
    database.toTreeWidget(d->_contentTree);
    return true;
}

/**
 * Returns the QTreeWidget used as header.
 */
QTreeWidget *DatabaseInformationDialog::getHeaderTreeWidget()
{
    d->_headerTree->show();
    return d->_headerTree;
}

/**
 * Returns the QTreeWidget used to show the full description.
 */
QTreeWidget *DatabaseInformationDialog::getDescriptionTreeWidget()
{
    return d->_contentTree;
}

/**
 * Save the content of the treewidgets into a single textual file.
 * The user is asked for the place and name of the report file name.
 */
bool DatabaseInformationDialog::saveContent()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save the current report"),
                                                QDir::homePath(), "*.txt");
    if (file.isEmpty())
        return false;
    QString report;
    for(int i=0; i < d->_headerTree->invisibleRootItem()->childCount(); ++i)
        report += d->getTreeItemText(d->_headerTree->invisibleRootItem()->child(i));
    report += "\n\n";
    for(int i=0; i < d->_contentTree->invisibleRootItem()->childCount(); ++i)
        report += d->getTreeItemText(d->_contentTree->invisibleRootItem()->child(i));

    qWarning() << report;

    return Utils::saveStringToFile(report, file, Utils::Overwrite, Utils::WarnUser, this);
}

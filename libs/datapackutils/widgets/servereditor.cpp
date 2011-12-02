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
/**
  \class DataPack::ServerEditor
  Widget used to modify the server list of the current DataPack::IServerManager
*/

#include "servereditor.h"

#include <translationutils/constanttranslations.h>

#include <datapackutils/core.h>
#include <datapackutils/servermanager.h>

#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QTextDocument>

#include <QDebug>

#include "ui_servereditor.h"

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {
const char *const ICON_SERVER_CONNECTED = "connect_established.png";
const char *const ICON_SERVER_NOT_CONNECTED = "connect_no.png";
const char *const ICON_SERVER_ASKING_CONNECTION = "connect_creating.png";
const char *const ICON_PACKAGE = "package.png";

const char * const CSS =
        "QTreeView::item:hover {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
        "}"
        "QTreeView::item:selected:active{"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        "QTreeView::item:selected:!active {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}";

class Delegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    int leftIndent = 0;
    QVariant decoration = index.data(Qt::DecorationRole);
    if (!decoration.isNull()) {
        QIcon icon = decoration.value<QIcon>();
        if (!icon.isNull()) {
            leftIndent = icon.actualSize(QSize(32,32)).width();
        }
    }

    QTextDocument doc;
    QString text = options.text;
    if (option.state & QStyle::State_Selected) {
        text.replace("color:gray", "color:lightgray");
        text.replace("color:black", "color:white");
    }
    doc.setHtml(text);

    /* Call this to get the focus rect and selection background. */
    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter, options.widget);

    /* Draw using our rich text document. */
    painter->translate(options.rect.left() + leftIndent, options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}

QSize Delegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}

}  // End anonymous namespace

static inline DataPack::Core *core() {return DataPack::Core::instance();}
static inline ServerManager *serverManager() {return qobject_cast<ServerManager*>(core()->serverManager());}
static inline QIcon icon(const QString &name) { qWarning() << DataPack::Core::instance()->icon(name, DataPack::Core::BigPixmaps); return QIcon(DataPack::Core::instance()->icon(name, DataPack::Core::BigPixmaps));}

static void managerToModel(QStandardItemModel *model)
{
    ServerManager *manager = serverManager();
    QFont bold;
    bold.setBold(true);
    model->clear();
    QStandardItem *root = model->invisibleRootItem();
    for(int i = 0; i < manager->serverCount(); ++i) {
        const Server &s = manager->getServerAt(i);
        QString label = QString("<span style=\"color:black;font-weight:bold\">%1</span>")
                .arg(s.description().data(ServerDescription::Label).toString());
        if (s.isConnected()) {
            label += QString("<br /><span style=\"color:gray; font-size:small\">%2 (%3: %4)</span>")
                    .arg(tkTr(Trans::Constants::CONNECTED))
                    .arg(tkTr(Trans::Constants::LAST_CHECK))
                    .arg(s.lastChecked().toString(QLocale().dateFormat(QLocale::LongFormat)));
        } else {
            label += QString("<br /><small>%2</small>")
                    .arg(tkTr(Trans::Constants::NOT_CONNECTED));
        }

        QStandardItem *server = new QStandardItem(label);
        root->appendRow(server);
        root->appendRow(new QStandardItem("<b>bla<br />bla</b>"));
        if (s.isConnected()) {
            server->setIcon(icon(::ICON_SERVER_CONNECTED));
            // Add packdescriptions
            const QList<PackDescription> &packs = manager->getPackDescription(s);
            for(int i=0; i < packs.count(); ++i) {
                QString pack = QString("<span style=\"color:black;font-weight:bold\">%1</span><br />"
                                       "<span style=\"color:gray; font-size:small\">%2: %3</span>")
                        .arg(packs.at(i).data(PackDescription::Label).toString())
                        .arg(tkTr(Trans::Constants::VERSION))
                        .arg(packs.at(i).data(PackDescription::Version).toString());
                QStandardItem *packItem = new QStandardItem(icon(::ICON_PACKAGE), pack);
                server->appendRow(packItem);
            }
        } else {
            server->setIcon(icon(::ICON_SERVER_NOT_CONNECTED));
        }
    }
}


ServerEditor::ServerEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerEditor)
{
    ui->setupUi(this);
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(1);
    managerToModel(model);
    ui->treeView->setModel(model);
    ui->treeView->header()->hide();
    Delegate *delegate = new Delegate;
    ui->treeView->setItemDelegate(delegate);
    ui->treeView->setStyleSheet(::CSS);

}

ServerEditor::~ServerEditor()
{
    delete ui;
}

bool ServerEditor::submitChanges()
{
    return true;
}

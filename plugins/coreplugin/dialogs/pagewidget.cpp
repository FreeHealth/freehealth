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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Core::PageWidget
  The PageWidget can be used to present a tree of Core::IGenericPage. The views are automatically
  created. \n
  You can populate the widget with your pages using the template member setPages(). The template must
  be a derivated of the Core::IGenericPage. \n
  There is an automatic settings state saving and restoring of the last page in view. Set the
  settings key using the setSettingKey().\n
  Generate the widget using setupUi().
  Usage:
  \code
    // Assuming that
    class IOptionsPage : public IGenericPage
    [...]

    // Create and feed the widget like this
    PageWidget *widget = new PageWidget(this);
    widget->setPages<IOptionsPage>(pages);
    widget->setSettingKey("Dialogs/Settings");
    widget->setupUi();
  \endcode
*/

#include "pagewidget.h"
#include "ui_pagewidget.h"

#include <utils/global.h>

#include <extensionsystem/pluginmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/igenericpage.h>
// #include <coreplugin/dialogs/helpdialog.h>

#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidgetItem>

#include <QDebug>

namespace {
    struct PageData {
        int index;
        QString category;
        QString id;
    };
}

Q_DECLARE_METATYPE(::PageData)

using namespace Core;
using namespace Core::Internal;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

PageWidget::PageWidget(QWidget *parent) :
        QWidget(parent), m_applied(false)
{
    m_ui = new Ui::PageWidget();
    m_ui->setupUi(this);
}

void PageWidget::setupUi()
{
    QString initialCategory = m_currentCategory;
    QString initialPage = m_currentPage;
    if (initialCategory.isEmpty() && initialPage.isEmpty() && !m_settingKey.isEmpty()) {
        initialCategory = settings()->value(m_settingKey+"/LastCategory", QVariant(QString())).toString();
        initialPage = settings()->value(m_settingKey+"/LastPage", QVariant(QString())).toString();
    }

    m_ui->splitter->setCollapsible(1, false);
    m_ui->pageTree->header()->setVisible(false);

    connect(m_ui->pageTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
        this, SLOT(pageSelected()));

    QMap<QString, QTreeWidgetItem *> categories;

    int index = 0;
    foreach(IGenericPage *page, m_pages) {
        PageData pageData;
        pageData.index = index;
        qWarning() << page->name() << page->id();

        pageData.category = page->category();
        pageData.id = page->id();

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, page->name());
        item->setData(0, Qt::UserRole, qVariantFromValue(pageData));

        QIcon icon = page->icon();
        if (!icon.isNull())
            item->setIcon(0, icon);

        QStringList categoriesId = page->category().split(QLatin1Char('|'));
        QStringList trCategories = page->category().split(QLatin1Char('|'));
        QString currentCategory = categoriesId.at(0);

        QTreeWidgetItem *treeitem;
        if (!categories.contains(currentCategory)) {
            treeitem = new QTreeWidgetItem(m_ui->pageTree);
            treeitem->setText(0, trCategories.at(0));
            treeitem->setData(0, Qt::UserRole, qVariantFromValue(pageData));
            categories.insert(currentCategory, treeitem);
        }

        int catCount = 1;
        while (catCount < categoriesId.count()) {
            if (!categories.contains(currentCategory + QLatin1Char('|') + categoriesId.at(catCount))) {
                treeitem = new QTreeWidgetItem(categories.value(currentCategory));
                currentCategory +=  QLatin1Char('|') + categoriesId.at(catCount);
                treeitem->setText(0, trCategories.at(catCount));
                treeitem->setData(0, Qt::UserRole, qVariantFromValue(pageData));
                categories.insert(currentCategory, treeitem);
            } else {
                currentCategory +=  QLatin1Char('|') + categoriesId.at(catCount);
            }
            ++catCount;
        }

        categories.value(currentCategory)->addChild(item);

        // add pageWidget
        m_ui->stackedPages->addWidget(createPageWidget(page));

        if (page->id() == initialPage && currentCategory == initialCategory) {
            m_ui->stackedPages->setCurrentIndex(m_ui->stackedPages->count());
            m_ui->pageTree->setCurrentItem(item);
        }

        index++;
    }

    QList<int> sizes;
    sizes << 150 << 300;
    m_ui->splitter->setSizes(sizes);

    m_ui->pageTree->sortItems(0, Qt::AscendingOrder);

    m_ui->splitter->setStretchFactor(m_ui->splitter->indexOf(m_ui->pageTree), 0);
    m_ui->splitter->setStretchFactor(m_ui->splitter->indexOf(m_ui->layoutWidget), 1);

    // resize and center window
    Utils::resizeAndCenter(this);
}

PageWidget::~PageWidget()
{
}

QWidget *PageWidget::createPageWidget(IGenericPage *page)
{
    QWidget *w = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setMargin(0);
    w->setLayout(lay);
    // add title and line
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize()+1);
    QLabel *title = new QLabel(w);
    title->setFont(bold);
    title->setText(page->title());
    title->setIndent(5);
    title->setContentsMargins(5,5,5,5);
//    title->setAlignment(Qt::AlignCenter);
//    title->setStyleSheet("background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(176, 246, 255, 149), stop:1 rgba(255, 255, 255, 0))");
    title->setStyleSheet("background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))");
    QFrame *line = new QFrame(w);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lay->addWidget(title);
    lay->addWidget(line);
    QWidget *p = page->createPage(m_ui->stackedPages);
    p->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addWidget(p);
    return w;
}

void PageWidget::pageSelected()
{
    QTreeWidgetItem *item = m_ui->pageTree->currentItem();
    PageData data = item->data(0, Qt::UserRole).value<PageData>();
    int index = data.index;
    m_currentCategory = data.category;
    m_currentPage = data.id;
    m_ui->stackedPages->setCurrentIndex(index);
}

IGenericPage *PageWidget::currentPage() const
{
    QTreeWidgetItem *item = m_ui->pageTree->currentItem();
    PageData data = item->data(0, Qt::UserRole).value<PageData>();
    if (data.index < m_pages.count() && data.index >= 0)
        return m_pages.at(data.index);
    return 0;
}

void PageWidget::saveState()
{
    if (!m_settingKey.isEmpty()) {
        settings()->setValue(m_settingKey+"/LastCategory", m_currentCategory);
        settings()->setValue(m_settingKey+"/LastPage", m_currentPage);
    }
}

void PageWidget::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        // recreate the widget
        m_ui->pageTree->clear();
        for(int i = 0; i < m_ui->stackedPages->count(); ++i) {
            m_ui->stackedPages->removeWidget(m_ui->stackedPages->widget(i));
        }
        setupUi();
    }
}

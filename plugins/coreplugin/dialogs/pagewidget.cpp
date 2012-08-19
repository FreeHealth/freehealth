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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QScrollArea>

#include <QDebug>

namespace {
    struct PageData {
        int index;
        QString category;
        QString id;
    };

    const char * const CSS = "text-indent:5px;padding:5px;font-weight:bold;background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))";
}

Q_DECLARE_METATYPE(::PageData)

using namespace Core;
using namespace Core::Internal;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

PageWidget::PageWidget(QWidget *parent) :
    QWidget(parent), m_applied(false), m_categoryInBold(true)
{
    m_ui = new Ui::PageWidget();
    m_ui->setupUi(this);
    m_ui->splitter->setCollapsible(0, true);
    m_ui->splitter->setCollapsible(1, false);
    m_ui->pageTree->header()->setVisible(false);

    connect(m_ui->pageTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
        this, SLOT(pageSelected()));
}

/** Setup the ui. You can specify if you want the category view to be sorted or not using the \e sortCategoryView. */
void PageWidget::setupUi(bool sortCategoryView)
{
    // clear ui
    m_ui->pageTree->clear();
    m_Categories.clear();
    m_Labels.clear();
    m_Items.clear();
    m_AddedWidgets.clear();  // widgets are deleted with the "delete w" above.
    for(int i = m_ui->stackedPages->count(); i > -1; --i) {
        QWidget *w = m_ui->stackedPages->widget(i);
        m_ui->stackedPages->removeWidget(w);
        delete w;
        w = 0;
    }

    // create ui
    QString initialCategory = m_currentCategory;
    QString initialPage = m_currentPage;
    if (initialCategory.isEmpty() && initialPage.isEmpty() && !m_settingKey.isEmpty()) {
        initialCategory = settings()->value(m_settingKey+"/LastCategory", QVariant(QString())).toString();
        initialPage = settings()->value(m_settingKey+"/LastPage", QVariant(QString())).toString();
    }

    // sort pages
    qSort(m_pages.begin(), m_pages.end(), Core::IGenericPage::lessThan);

    QMap<QString, QTreeWidgetItem *> categories;
    QFont bold;
    bold.setBold(true);
    int index = 0;
    foreach(IGenericPage *page, m_pages) {
        PageData pageData;
        pageData.index = index;
        pageData.category = page->category();
        pageData.id = page->id();

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, page->name());
        item->setData(0, Qt::UserRole, qVariantFromValue(pageData));
        m_Items.insert(page, item);

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
            if (m_categoryInBold) {
                treeitem->setFont(0, bold);
            }
            categories.insert(currentCategory, treeitem);
            m_Categories.insert(page, treeitem);
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

//    if (sortCategoryView)
//        m_ui->pageTree->sortItems(0, Qt::AscendingOrder);

    m_ui->stackedPages->layout()->setMargin(0);
    m_ui->layoutWidget->layout()->setContentsMargins(12,0,12,0);
//    m_ui->stackedPages->layout()->setSpacing(0);

    // resize and center window
//    Utils::resizeAndCenter(this);
}

/** Expand all categories in the category treeView. */
void PageWidget::expandAllCategories()
{
    m_ui->pageTree->expandAll();
}

void PageWidget::expandFirstCategories()
{
//    m_ui->pageTree->expandItem(m_ui->pageTree->);
}

/** Set the sizes to the splitter (sa QSplitter documentation) */
void PageWidget::setSplitterSizes(const QList<int> &sizes)
{
    m_ui->splitter->setSizes(sizes);
}

PageWidget::~PageWidget()
{
}

QWidget *PageWidget::createPageWidget(IGenericPage *page)
{
    QWidget *w = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setMargin(0);
//    lay->setContentsMargins(0,0,0,0);
    w->setLayout(lay);

    // add title and line
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setMargin(0);
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize()+1);
    QLabel *title = new QLabel(w);
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    title->setFont(bold);
    title->setWordWrap(true);
    title->setText(page->title());
    title->setStyleSheet(::CSS);
    m_Labels.insert(page, title);
    QToolButton *button = new QToolButton(w);
    button->setIcon(theme()->icon(Core::Constants::ICONFULLSCREEN));
    connect(button, SIGNAL(clicked()), this, SLOT(expandView()));
    m_Buttons.append(button);
    titleLayout->addWidget(title);
    titleLayout->addWidget(button);

    QFrame *line = new QFrame(w);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lay->addLayout(titleLayout);
    lay->addWidget(line);

    // add a scrollarea with the widget's page to add
    QWidget *p = page->createPage(w);
    m_AddedWidgets << p;
    p->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    if (p->layout())
        p->layout()->setMargin(0);
    QScrollArea *scroll = new QScrollArea(w);
    scroll->setWidget(p);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    lay->addWidget(scroll);

//    QFrame *bottomline = new QFrame(w);
//    bottomline->setFrameShape(QFrame::HLine);
//    bottomline->setFrameShadow(QFrame::Sunken);
//    lay->addWidget(bottomline);

    return w;
}

/** Slot connected to the page selection. */
void PageWidget::pageSelected()
{
    QTreeWidgetItem *item = m_ui->pageTree->currentItem();
    PageData data = item->data(0, Qt::UserRole).value<PageData>();

    int index = data.index;
    m_currentCategory = data.category;
    m_currentPage = data.id;
    m_ui->stackedPages->setCurrentIndex(index);
}

void PageWidget::expandView()
{
    setViewExpanded(!isViewExpanded());
}

/** Return the current selected and activated page. */
IGenericPage *PageWidget::currentPage() const
{
    QTreeWidgetItem *item = m_ui->pageTree->currentItem();
    PageData data = item->data(0, Qt::UserRole).value<PageData>();
    if (data.index < m_pages.count() && data.index >= 0)
        return m_pages.at(data.index);
    return 0;
}

/** Save the state of the widget */
void PageWidget::saveState()
{
    if (!m_settingKey.isEmpty()) {
        settings()->setValue(m_settingKey+"/LastCategory", m_currentCategory);
        settings()->setValue(m_settingKey+"/LastPage", m_currentPage);
    }
}

/** Return the list of all added widgets from the selected pages */
QList<QWidget *> PageWidget::pageWidgets() const
{
    return m_AddedWidgets;
}

/** Return true if page widget are full sized */
bool PageWidget::isViewExpanded() const
{
    return (m_ui->splitter->sizes().at(0)==0);
}

/** Define the page widget to be full sized according to the \e expand param */
void PageWidget::setViewExpanded(bool expand)
{
    if (expand) {
        QList<int> sizes;
        sizes << 0 << this->width();
        m_ui->splitter->setSizes(sizes);
    } else {
        if (isViewExpanded()) {
            QList<int> sizes;
            sizes << 1 << 3;
            m_ui->splitter->setSizes(sizes);
        }
    }
}

void PageWidget::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type()==QEvent::LanguageChange) {
        for(int i = 0; i < m_Buttons.count(); ++i) {
            m_Buttons[i]->setToolTip(tr("Full view"));
        }
        QHashIterator<Core::IGenericPage*, QLabel *> it(m_Labels);
        while (it.hasNext()) {
            it.next();
            it.value()->setText(it.key()->name());
        }

        QHashIterator<Core::IGenericPage*, QTreeWidgetItem *> it1(m_Items);
        while (it1.hasNext()) {
            it1.next();
            it1.value()->setText(0, it1.key()->name());
        }

        QHashIterator<Core::IGenericPage*, QTreeWidgetItem *> it2(m_Categories);
        while (it2.hasNext()) {
            it2.next();
            it2.value()->setText(0, it2.key()->name());
        }
    }
}

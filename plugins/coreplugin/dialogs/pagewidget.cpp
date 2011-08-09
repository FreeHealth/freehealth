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
#include <QScrollArea>

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
    QWidget(parent), m_applied(false), m_categoryInBold(true)
{
    m_ui = new Ui::PageWidget();
    m_ui->setupUi(this);
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

    if (sortCategoryView)
        m_ui->pageTree->sortItems(0, Qt::AscendingOrder);

    m_ui->stackedPages->layout()->setMargin(0);
    m_ui->layoutWidget->layout()->setContentsMargins(12,0,12,0);
//    m_ui->stackedPages->layout()->setSpacing(0);

    // resize and center window
    Utils::resizeAndCenter(this);
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

void PageWidget::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        /** \todo code here: PageWidget::changeEvent(QEvent *event) */
//        // recreate the widget
//        m_ui->pageTree->clear();
//        for(int i = 0; i < m_ui->stackedPages->count(); ++i) {
//            m_ui->stackedPages->removeWidget(m_ui->stackedPages->widget(i));
//        }
//        setupUi();
    }
}

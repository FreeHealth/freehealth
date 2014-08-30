/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class Core::HelpDialog
  \brief Creates an "autodeletion on close" dialog help viewer.
  It works great with ISettings::DocumentationPath.\n
  You SHOULD NOT delete the dialog.
*/

#include "helpdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constanttranslations.h>
#include <utils/global.h>
#include <utils/widgets/minisplitter.h>

#include <QApplication>
#include <QTextBrowser>
#include <QAction>
#include <QToolBar>
#include <QComboBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QHash>
#include <QString>
#include <QDir>
#include <QHeaderView>
#include <QSplitter>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include <QDebug>

using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Core {
namespace Internal {
const char * const INDEX_FILENAME = "toc.html";

class HelpDialogPrivate
{
public:
    // FIXME: all this is dead code: see Ctor fixme note
    HelpDialogPrivate(QDialog *dlg) :
            m_Browser(0), m_TocBrowser(0), q(dlg)
    {
        Core::ITheme *theme = Core::ICore::instance()->theme();
        m_Browser = new QTextBrowser(dlg);
        m_Browser->setOpenExternalLinks(true);
//        m_TocBrowser = new QTextBrowser(dlg);
//        m_TocBrowser->setOpenExternalLinks(true);
        m_Tree = new QTreeWidget(dlg);
//        m_Combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_ToolBar = new QToolBar(tkTr(Trans::Constants::NAVIGATION), dlg);
#if QT_VERSION < 0x040600
        m_ToolBar->setIconSize(QSize(16,16));
#endif
        aNext = new QAction(dlg);
        aPrevious = new QAction(dlg);
        aHome = new QAction(dlg);
        aFullScreen = new QAction(dlg);
        aZoomIn = new QAction(dlg);
        aZoomOut = new QAction(dlg);
        aClose = new QAction(dlg);
        aNext->setIcon(theme->icon(Constants::ICONNEXT));
        aPrevious->setIcon(theme->icon(Constants::ICONPREVIOUS));
        aHome->setIcon(theme->icon(Constants::ICONHOME));
        aFullScreen->setIcon(theme->icon(Constants::ICONFULLSCREEN));
        aZoomIn->setIcon( Core::ICore::instance()->theme()->icon(Core::Constants::ICONFONTBIGGER) );
        aZoomOut->setIcon( Core::ICore::instance()->theme()->icon(Core::Constants::ICONFONTSMALLER) );
        aClose->setIcon(theme->icon(Constants::ICONEXIT));
        m_ToolBar->addAction(aHome);
        m_ToolBar->addSeparator();
        m_ToolBar->addAction(aPrevious);
        m_ToolBar->addAction(aNext);
        m_ToolBar->addSeparator();
        m_ToolBar->addAction(aFullScreen);
        m_ToolBar->addAction(aZoomIn);
        m_ToolBar->addAction(aZoomOut);
//        m_ToolBar->addSeparator();
//        m_ToolBar->addWidget(m_Combo);
        m_ToolBar->addSeparator();
        m_ToolBar->addAction(aClose);
    }
    ~HelpDialogPrivate() {}

    void retranslate()
    {
        aNext->setToolTip(tkTr(Trans::Constants::NEXT_TEXT));
        aPrevious->setToolTip(tkTr(Trans::Constants::PREVIOUS_TEXT));
        aHome->setToolTip(tkTr(Trans::Constants::HOME_TEXT));
        aClose->setToolTip(tkTr(Trans::Constants::CLOSE_TEXT));
        aFullScreen->setToolTip(tkTr(Trans::Constants::FULLSCREEN_TEXT));
        aZoomIn->setToolTip(tkTr(Trans::Constants::ZOOMIN_TEXT));
        aZoomOut->setToolTip(tkTr(Trans::Constants::ZOOMOUT_TEXT));
    }

    void createConnections()
    {
        Q_ASSERT(q);
        q->connect(aHome, SIGNAL(triggered()), m_Browser, SLOT(home()));
        q->connect(aNext, SIGNAL(triggered()), m_Browser, SLOT(forward()));
        q->connect(aPrevious, SIGNAL(triggered()), m_Browser, SLOT(backward()));
        q->connect(aHome, SIGNAL(triggered()), m_Browser, SLOT(home()));
        q->connect(aClose, SIGNAL(triggered()), q, SLOT(close()));
        q->connect(m_Browser, SIGNAL(sourceChanged(QUrl)), q, SLOT(updateWindowTitle()));
//        q->connect(m_TocBrowser, SIGNAL(anchorClicked(QUrl)), q, SLOT(changePage(QUrl)));
        q->connect(aFullScreen, SIGNAL(triggered()), q, SLOT(fullScreen()));
        q->connect(aZoomIn, SIGNAL(triggered()), m_Browser, SLOT(zoomIn()));
        q->connect(aZoomOut, SIGNAL(triggered()), m_Browser, SLOT(zoomOut()));
        q->connect(aFullScreen, SIGNAL(triggered()), q, SLOT(fullScreen()));
        q->connect(m_Tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),q, SLOT(treeActivated(QTreeWidgetItem*)));
    }

    void populateTree()
    {
        QDir dir(settings()->path(ISettings::DocumentationPath));
        QStringList files = dir.entryList(QStringList() << "*.htm" << "*.html", QDir::Files | QDir::Readable);
        m_Tree->clear();
        foreach(const QString &f, files) {
            QString tmp = Utils::readTextFile(dir.absolutePath() + QDir::separator() + f);
            if (tmp.contains("<title>")) {
                int begin = tmp.indexOf("<title>") + 7;
                int end = tmp.indexOf("</title");
                QString title = tmp.mid(begin, end-begin);
                title.replace("&#39;","'");
                m_Title_Page.insert(title, f);
                new QTreeWidgetItem(m_Tree, QStringList() << title);
//                if (f=="index.html")
//                    m_Tree->setCurrentIndex(m_Tree->topLevelItemCount()-1);
            }
        }
    }

    QTextBrowser *m_Browser;
    QTextBrowser *m_TocBrowser;
//    QComboBox *m_Combo;
    QTreeWidget *m_Tree;
    QDialog *q;
    QHash<QString,QString> m_Title_Page;
    QToolBar *m_ToolBar;
    QAction *aNext, *aPrevious, *aHome, *aClose, *aFullScreen, *aZoomIn, *aZoomOut;
};

}  // End Internal
}  // End Core


/** \brief Construct a help dialog pointing to the \e page */
HelpDialog::HelpDialog(const QString &page, QWidget *parent) :
    QDialog(parent),
    d(0)
{
    setObjectName("HelpDialog");
    setAttribute(Qt::WA_DeleteOnClose);

    // FIXME: as dokuwiki offline exporter is off, we can not provide local documentation
    // We need to fix the dokuwiki exporter (including JS sources / DFSG) and the current code
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    QLabel *lbl = new QLabel(tr("<center>Documentation is available on line only.</center>"), this);
    QLabel *lblLink = new QLabel(QString("<center><a href='%1'>%1</a></center>").arg(settings()->path(Core::ISettings::WebSiteUrl)));
    QDialogButtonBox *but = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    connect(but, SIGNAL(accepted()), this, SLOT(accept()));
    layout->addWidget(lbl);
    layout->addWidget(lblLink);
    layout->addWidget(but);
    setWindowTitle(tkTr(Trans::Constants::HELP_TEXT));

//    // Create dialog
//    d = new HelpDialogPrivate(this);
////    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
//    Q_ASSERT(d);
//    d->retranslate();
//    d->createConnections();

//    // Load pages to browsers
//    d->m_Browser->setSearchPaths(QStringList() << settings()->path(ISettings::DocumentationPath));
////    d->m_TocBrowser->setSearchPaths(QStringList() << settings()->path(ISettings::DocumentationPath));

//    // Set default index
//    d->m_Browser->setSource(QString(INDEX_FILENAME));
////    d->m_TocBrowser->setSource(QString(TOC_FILENAME));

//    // Find local page
//    if (!page.isEmpty()) {
//        QString fullPagePath = QString("%1/%2").arg(settings()->path(ISettings::DocumentationPath)).arg(page);
//        if (QFileInfo(fullPagePath).exists()) {
//            if (page!="index.html" && page!="index.htm" && (!page.isEmpty()))
//                d->m_Browser->setSource(page);
//        }
//    }

//    // create the layout
//    QGridLayout *layout = new QGridLayout(this);
//    layout->setMargin(0);
//    layout->setSpacing(0);
//    layout->addWidget(d->m_ToolBar);//,0,0,1,2);
//    Utils::MiniSplitter *split = new Utils::MiniSplitter(this);
////    split->addWidget(d->m_Tree);
////    split->addWidget(d->m_TocBrowser);
//    split->addWidget(d->m_Browser);
//    layout->addWidget(split);
////    layout->addWidget(d->m_Tree,1,0);

//    // set icons
//    Core::ITheme *theme = Core::ICore::instance()->theme();
//    setWindowIcon(theme->icon(Constants::ICONHELP));
////    d->populateTree();
//    updateWindowTitle();

    // resize and center windows
    //Utils::resizeAndCenter(this);
}

/** \brief Creates a new help browser */
void HelpDialog::showPage(const QString &page)
{
    HelpDialog *hb = new HelpDialog(page, qApp->activeWindow());
    hb->show();
}

/** \brief Creates a new help browser starting on the documentation index file */
void HelpDialog::showIndex()
{
    showPage(QString(::INDEX_FILENAME));
}

/** \brief Slot called when text browser source changes --> update the window title. */
void HelpDialog::updateWindowTitle()
{
    setWindowTitle( tkTr(Trans::Constants::HELP_TEXT) + ": " + d->m_Browser->documentTitle());
//    d->m_Combo->setCurrentIndex( d->m_Combo->findText(d->m_Browser->documentTitle()) );
    QList<QTreeWidgetItem *> list = d->m_Tree->findItems(d->m_Browser->documentTitle(),Qt::MatchExactly,0);
    if (list.count())
        d->m_Tree->setCurrentItem(list.at(0));
}

/** \brief Slot called for full screen toogler. */
void HelpDialog::fullScreen()
{
    // FIXME: see Ctor fixme
    return;
    Utils::setFullScreen(this, !this->isFullScreen());
}

void HelpDialog::treeActivated(QTreeWidgetItem *item)
{
    d->m_Browser->setSource(d->m_Title_Page.value(item->text(0)));
}

//void HelpDialog::changePage(const QUrl &url)
//{
//    d->m_Browser->setSource(url);
//}

/** \brief Protected slot that assumes translations according to the QEvent::LangageChange. */
void HelpDialog::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        d->retranslate();
    }
}

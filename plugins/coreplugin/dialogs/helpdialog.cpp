/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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

/**
  \class HelpDialog
  \brief Creates an "autodeletion on close" dialog help viewer.
  It works great with ISettings::DocumentationPath.\n
  You SHOULD NOT delete the dialog.
*/

#include "helpdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>

#include <translationutils/constanttranslations.h>
#include <utils/global.h>

#include <QApplication>
#include <QTextBrowser>
#include <QAction>
#include <QToolBar>
#include <QComboBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QSplitter>
#include <QHash>
#include <QString>
#include <QDir>
#include <QHeaderView>
#include <QSplitter>

#include <QDebug>


using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

namespace Core {
namespace Internal {


/** \todo Manage multilingual documentation by changing search path ? */


class HelpDialogPrivate
{
public:
    HelpDialogPrivate(QDialog *dlg) :
            m_Browser(0), m_Parent(dlg)
    {
        Core::ITheme *theme = Core::ICore::instance()->theme();
        m_Browser = new QTextBrowser(dlg);
        m_Browser->setOpenExternalLinks(true);
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
        Q_ASSERT(m_Parent);
        m_Parent->connect(aNext, SIGNAL(triggered()), m_Browser, SLOT(forward()));
        m_Parent->connect(aPrevious, SIGNAL(triggered()), m_Browser, SLOT(backward()));
        m_Parent->connect(aHome, SIGNAL(triggered()), m_Browser, SLOT(home()));
        m_Parent->connect(aClose, SIGNAL(triggered()), m_Parent, SLOT(close()));
        m_Parent->connect(m_Browser, SIGNAL(sourceChanged(QUrl)), m_Parent, SLOT(updateWindowTitle()));
        m_Parent->connect(aFullScreen, SIGNAL(triggered()), m_Parent, SLOT(fullScreen()));
        m_Parent->connect(aZoomIn, SIGNAL(triggered()), m_Browser, SLOT(zoomIn()));
        m_Parent->connect(aZoomOut, SIGNAL(triggered()), m_Browser, SLOT(zoomOut()));
        m_Parent->connect(aFullScreen, SIGNAL(triggered()), m_Parent, SLOT(fullScreen()));
        m_Parent->connect(m_Tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),m_Parent, SLOT(treeActivated(QTreeWidgetItem*)));
    }

    void populateTree()
    {
        QDir dir(Core::ICore::instance()->settings()->path(ISettings::DocumentationPath));
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
//    QComboBox *m_Combo;
    QTreeWidget *m_Tree;
    QDialog *m_Parent;
    QHash<QString,QString> m_Title_Page;
    QToolBar *m_ToolBar;
    QAction *aNext, *aPrevious, *aHome, *aClose, *aFullScreen, *aZoomIn, *aZoomOut;
};

}  // End Internal
}  // End Core


/** \brief Construct a help dialog pointing to the \e page */
HelpDialog::HelpDialog(const QString &page, QWidget *parent) :
        QDialog(parent), d(0)
{
    setObjectName("HelpDialog");
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_GroupLeader);
    d = new HelpDialogPrivate(this);
    Q_ASSERT(d);
    d->retranslate();
    d->createConnections();
    d->m_Browser->setSearchPaths(QStringList() << Core::ICore::instance()->settings()->path(ISettings::DocumentationPath));
    if (page.isEmpty())
        d->m_Browser->setSource(QString("index.html"));
    else
        d->m_Browser->setSource(page);
    // create the layout
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(d->m_ToolBar);//,0,0,1,2);
    QSplitter *split = new QSplitter(this);
    split->addWidget(d->m_Tree);
    split->addWidget(d->m_Browser);
    layout->addWidget(split);
//    layout->addWidget(d->m_Tree,1,0);

    Core::ITheme *theme = Core::ICore::instance()->theme();
    setWindowIcon(theme->icon(Constants::ICONHELP));
    d->populateTree();
    updateWindowTitle();

    // resize windows
    QWidget *ref = 0;
    if (Core::ICore::instance()->mainWindow())
        ref = Core::ICore::instance()->mainWindow();
    else
        ref = qApp->topLevelWidgets().first();
    QSize size = ref->size();
    size = QSize(size.width()*0.9, size.height()*0.9);
    this->resize(size);
    // recenter window
    Utils::centerWidget(this, ref);
}

/** \brief Creates a new help browser */
void HelpDialog::showPage(const QString &page)
{
    HelpDialog *hb = new HelpDialog(page, qApp->activeWindow());
    hb->show();
}

/** \brief Slot called when text browser source changes --> update the window title. */
void HelpDialog::updateWindowTitle()
{
    setWindowTitle( tkTr(Trans::Constants::HELP_TEXT) + " : " + d->m_Browser->documentTitle());
//    d->m_Combo->setCurrentIndex( d->m_Combo->findText(d->m_Browser->documentTitle()) );
    QList<QTreeWidgetItem *> list = d->m_Tree->findItems(d->m_Browser->documentTitle(),Qt::MatchExactly,0);
    if (list.count())
        d->m_Tree->setCurrentItem(list.at(0));
}

/** \brief Slot called for full screen toogler. */
void HelpDialog::fullScreen()
{
    Utils::setFullScreen(this, !this->isFullScreen());
}

void HelpDialog::treeActivated(QTreeWidgetItem *item)
{
    d->m_Browser->setSource(d->m_Title_Page.value(item->text(0)));
}

/** \brief Protected slot that assumes translations according to the QEvent::LangageChange. */
void HelpDialog::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        d->retranslate();
    }
}

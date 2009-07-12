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
  \class tkHelpDialog
  \brief Creates an "autodeletion on close" dialog help viewer.
  It works great with tkSettings::DocumentationPath.\n
  You SHOULD NOT delete the dialog.
*/

#include "tkHelpDialog.h"

// including toolkit headers
#include <tkSettings.h>
#include <tkConstantTranslations.h>
#include <tkTheme.h>

#include <QTextBrowser>
#include <QPushButton>
#include <QComboBox>
#include <QApplication>
#include <QGridLayout>
#include <QSplitter>
#include <QHash>
#include <QString>

#include <QDebug>

Q_TK_USING_TRANSLATIONS;

class tkHelpDialogPrivate
{
public:
    tkHelpDialogPrivate(QDialog *dlg) :
            m_Browser(0), m_Next(0), m_Previous(0), m_Home(0), m_Close(0), m_Combo(0), m_Parent(dlg) {}
    ~tkHelpDialogPrivate() {}

    void retranslate()
    {
        m_Next->setToolTip(tkTr(NEXT_TEXT));
        m_Previous->setToolTip(tkTr(PREVIOUS_TEXT));
        m_Home->setToolTip(tkTr(HOME_TEXT));
        m_Close->setToolTip(tkTr(CLOSE_TEXT));
        m_FullScreen->setToolTip(tkTr(FULLSCREEN_TEXT));
    }

    void createConnections()
    {
        Q_ASSERT(m_Parent);
        m_Parent->connect(m_Next, SIGNAL(clicked()), m_Browser, SLOT(forward()));
        m_Parent->connect(m_Previous, SIGNAL(clicked()), m_Browser, SLOT(backward()));
        m_Parent->connect(m_Home, SIGNAL(clicked()), m_Browser, SLOT(home()));
        m_Parent->connect(m_Close, SIGNAL(clicked()), m_Parent, SLOT(close()));
        m_Parent->connect(m_Browser, SIGNAL(sourceChanged(QUrl)), m_Parent, SLOT(updateWindowTitle()));
        m_Parent->connect(m_FullScreen, SIGNAL(clicked()), m_Parent, SLOT(fullScreen()));
        m_Parent->connect(m_Combo, SIGNAL(activated(const QString&)), m_Parent, SLOT(comboActivated(const QString&)));
    }

    void populateCombo()
    {
        QDir dir(tkSettings::instance()->path(tkSettings::DocumentationPath));
        QStringList files = dir.entryList(QStringList() << "*.htm" << "*.html", QDir::Files | QDir::Readable);
        foreach(const QString &f, files) {
            QString tmp = tkGlobal::readTextFile(dir.absolutePath() + QDir::separator() + f);
            if (tmp.contains("<title>")) {
                int begin = tmp.indexOf("<title>") + 7;
                int end = tmp.indexOf("</title");
                QString title = tmp.mid(begin, end-begin);
                title.replace("&#39;","'");
                m_Title_Page.insert(title, f);
                m_Combo->addItem(title);
                if (f=="index.html")
                    m_Combo->setCurrentIndex(m_Combo->count()-1);
            }
        }
    }

    QTextBrowser *m_Browser;
    QPushButton  *m_Next, *m_Previous, *m_Home, *m_Close, *m_FullScreen;
    QComboBox *m_Combo;
    QDialog *m_Parent;
    QHash<QString,QString> m_Title_Page;
};

/** \brief Construct a help dialog pointing to the \e page */
tkHelpDialog::tkHelpDialog(const QString &page, QWidget *parent) :
        QDialog(parent), d(0)
{
    setObjectName("tkHelpDialog");
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_GroupLeader);
    d = new tkHelpDialogPrivate(this);
    Q_ASSERT(d);
    // create objects
    d->m_Browser = new QTextBrowser(this);
    d->m_Next = new QPushButton(this);
    d->m_Previous = new QPushButton(this);
    d->m_Home = new QPushButton(this);
    d->m_Close = new QPushButton(this);
    d->m_FullScreen = new QPushButton(this);
    d->m_Combo = new QComboBox(this);
    d->retranslate();
    d->createConnections();
    d->m_Browser->setSearchPaths(QStringList() << tkSettings::instance()->path(tkSettings::DocumentationPath));
    if (page.isEmpty())
        d->m_Browser->setSource(QString("index.html"));
    else
        d->m_Browser->setSource(page);
    // create the layout
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setSpacing(10);
    layout->addWidget(d->m_Home,             0, 0 );
    layout->addWidget(d->m_Previous,         0, 1 );
    layout->addWidget(d->m_Next,             0, 2 );
    layout->addWidget(d->m_FullScreen,       0, 3 );
    layout->addWidget(d->m_Combo,            0, 4 );
    layout->addWidget(new QSplitter(this),   0, 5 );
    layout->addWidget(d->m_Close,            0, 6 );
    layout->addWidget(d->m_Browser,          1, 0, 1, 7 );

    setWindowIcon(tkTheme::icon(ICONHELP));
    d->m_Home->setMaximumSize(22,22);
    d->m_Next->setMaximumSize(22,22);
    d->m_Previous->setMaximumSize(22,22);
    d->m_FullScreen->setMaximumSize(22,22);
    d->m_Close->setMaximumSize(22,22);
    d->m_Home->setIcon(tkTheme::icon(ICONHOME));
    d->m_Next->setIcon(tkTheme::icon(ICONNEXT));
    d->m_Previous->setIcon(tkTheme::icon(ICONPREVIOUS));
    d->m_FullScreen->setIcon(tkTheme::icon(ICONFULLSCREEN));
    d->m_Close->setIcon(tkTheme::icon(ICONEXIT));
    d->populateCombo();
    updateWindowTitle();
    resize(500,500);
}

/** \brief Creates a new help browser */
void tkHelpDialog::showPage(const QString &page)
{
    tkHelpDialog *hb = new tkHelpDialog(page,qApp->activeWindow());
    hb->show();
}

/** \brief Slot called when text browser source changes --> update the window title. */
void tkHelpDialog::updateWindowTitle()
{
    setWindowTitle( tkTr(HELP_TEXT) + " : " + d->m_Browser->documentTitle());
    d->m_Combo->setCurrentIndex( d->m_Combo->findText(d->m_Browser->documentTitle()) );
}

/** \brief Slot called for full screen toogler. */
void tkHelpDialog::fullScreen()
{
    tkGlobal::setFullScreen(this, !this->isFullScreen());
}

void tkHelpDialog::comboActivated(const QString &item)
{
    d->m_Browser->setSource(d->m_Title_Page.value(item));
}

/** \brief Protected slot that assumes translations according to the QEvent::LangageChange. */
void tkHelpDialog::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        d->retranslate();
    }
}

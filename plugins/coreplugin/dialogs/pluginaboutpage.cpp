/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "pluginaboutpage.h"

//#include <extensionsystem/pluginspec.h>

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QTreeWidget>
#include <QTextBrowser>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QApplication>
#include <QLabel>

#include <QDebug>

using namespace Core;
using namespace Trans::ConstantTranslations;

PluginAboutPage::PluginAboutPage(ExtensionSystem::PluginSpec *spec, QObject *parent) :
        Core::IAboutPage(parent), m_Spec(spec)
{
    setObjectName("PluginAboutPage");
}

PluginAboutPage::~PluginAboutPage()
{
    if (m_Widget)
        delete m_Widget;
}

QString PluginAboutPage::id() const
{
    return m_Spec->name();
}

QString PluginAboutPage::name() const
{
    return m_Spec->name();
}

QString PluginAboutPage::category() const
{
    return tkTr(Trans::Constants::PLUGINS_CATEGORY);
}

QWidget *PluginAboutPage::widget()
{
    if (!m_Spec)
        return new QWidget();

    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(w);
    tree->header()->hide();
    layout->addWidget(tree);
    QLabel *lbl = new QLabel(w);
    lbl->setText(tkTr(Trans::Constants::DESCRIPTION));
    layout->addWidget(lbl);
    QTextBrowser *tb = new QTextBrowser(w);
    layout->addWidget(tb);

    // popuplate tree
    tree->clear();
    QFont f;
    f.setBold(true);
    QTreeWidgetItem *i = 0;
    i = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::INFORMATIONS));
    i->setFont(0,f);
    new QTreeWidgetItem(i, QStringList() << tkTr(Trans::Constants::VERSION) + " " + m_Spec->version() );
    new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    if (Utils::isDebugCompilation()) {
        new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_DEBUG) );
    } else {
        new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_RELEASE) );
    }
    new QTreeWidgetItem(i, QStringList() << m_Spec->license() );
    tree->expandAll();

    // populate textbrowser
    tb->setPlainText(m_Spec->description());

    return w;
}

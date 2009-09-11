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
#include "appaboutpage.h"

#include <fdcoreplugin/coreimpl.h>
#include <fdcoreplugin/commandlineparser.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>

using namespace Core;
using namespace Core::Internal;

static const char *ABOUT_TEXT = QT_TRANSLATE_NOOP("AboutDialog",
        "<p align=center><b>Welcome to FreeDiams</b><br />"
        "Copyright (C) 2008-2009 by Eric MAEKER, MD</p>"
        "<p align=left>This application is a stable release but can still contains some bugs.<br />"
        "This software is release without any warranty and only for test purposal.<br />"
        "Please refer to web site for more informations.<br />"
        "<a href=\"%1\">Web site</a>"
        "</p>"
        );

AppAboutPage::AppAboutPage(QObject *parent) :
        IAboutPage(parent)
{
    setObjectName("AppAboutPage");
}

AppAboutPage::~AppAboutPage()
{
}

QWidget *AppAboutPage::widget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    QLabel *label = new QLabel(w);
    label->setWordWrap(true);
    label->setOpenExternalLinks(true);
    layout->addWidget(label);
    layout->addSpacerItem(new QSpacerItem(20,20, QSizePolicy::Expanding, QSizePolicy::Expanding));
    label->clear();
    label->setText(tr(ABOUT_TEXT).arg(qApp->organizationDomain()));
    return w;
}


QWidget *CommandLineAboutPage::widget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(w);
    tree->setColumnCount(2);
    layout->addWidget(tree);

    for(int i=0; i<CommandLine::CL_MaxParam;++i) {
        new QTreeWidgetItem(tree, QStringList()
                            << CoreImpl::instance()->commandLine()->paramName(i)
                            << CoreImpl::instance()->commandLine()->value(i, QString("not defined")).toString());
    }
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
    return w;
}


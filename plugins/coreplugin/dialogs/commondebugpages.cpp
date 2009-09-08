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
#include "commondebugpages.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <extensionsystem/pluginmanager.h>

#include <QTreeWidget>
#include <QGridLayout>
#include <QHeaderView>

using namespace Core;
using namespace Core::Internal;


LogErrorDebugPage::LogErrorDebugPage(QObject *parent) :
        IDebugPage(parent)
{
    setObjectName("LogErrorDebugPage");
}

LogErrorDebugPage::~LogErrorDebugPage()
{
}

QString LogErrorDebugPage::name() const     { return Trans::ConstantTranslations::tkTr(Trans::Constants::LOGGED_ERRORS); }
QString LogErrorDebugPage::category() const { return Trans::ConstantTranslations::tkTr(Trans::Constants::ERRORS); }

QWidget *LogErrorDebugPage::widget()
{
    QWidget *m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    Utils::Log::errorsToTreeWidget(tree, true);
    layout->addWidget(tree);
    return m_Widget;
}

LogMessageDebugPage::LogMessageDebugPage(QObject *parent) :
        IDebugPage(parent)
{
    setObjectName("LogMessageDebugPage");
}

LogMessageDebugPage::~LogMessageDebugPage()
{
}

QString LogMessageDebugPage::name() const
{ return Trans::ConstantTranslations::tkTr(Trans::Constants::LOGGED_MESSAGES); }

QString LogMessageDebugPage::category() const
{ return Trans::ConstantTranslations::tkTr(Trans::Constants::MESSAGES); }

QWidget *LogMessageDebugPage::widget()
{
    QWidget *m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(m_Widget);
    layout->addWidget(tree);
    Utils::Log::messagesToTreeWidget(tree, true);
    return m_Widget;
}



///// SETTINGS

SettingDebugPage::SettingDebugPage(QObject *parent) :
        IDebugPage(parent)
{
    setObjectName("SettingDebugPage");
}

SettingDebugPage::~SettingDebugPage()
{
}

QWidget *SettingDebugPage::widget()
{
    QWidget *m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    layout->addWidget(tree);
    Core::ICore::instance()->settings()->getTreeWidget(tree);
    return m_Widget;
}

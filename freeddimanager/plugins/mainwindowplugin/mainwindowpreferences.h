/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MAINWINDOWPREFERENCES_H
#define MAINWINDOWPREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include "ui_mainwindowpreferenceswidget.h"

#include <QPointer>
#include <QObject>

/**
 * \file mainwindowpreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.1
 * \date 19 Oct 2009
*/

namespace Core {
class ISettings;
}

namespace MainWin {
namespace Internal {

class MainWindowPreferencesWidget : public QWidget, private Ui::MainWindowPreferencesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindowPreferencesWidget)

public:
    explicit MainWindowPreferencesWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);
};


class MainWindowPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    MainWindowPreferencesPage(QObject *parent = 0);
    ~MainWindowPreferencesPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const {return displayName();}
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {MainWindowPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<MainWindowPreferencesWidget> m_Widget;
};

}  // End Internal
}  // End MainWin

#endif // MAINWINDOWPREFERENCES_H

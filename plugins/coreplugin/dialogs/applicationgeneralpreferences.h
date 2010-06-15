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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef APPLICATIONGENERALPREFERENCES_H
#define APPLICATIONGENERALPREFERENCES_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/ioptionspage.h>

#include <QWidget>

#include <QPointer>
#include <QObject>

/**
 * \file applicationgeneralpreferences.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 15 June 2010
*/


namespace Core {
class ISettings;

namespace Internal {
namespace Ui {
class ApplicationGeneralPreferencesWidget;
}

class ApplicationGeneralPreferencesWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ApplicationGeneralPreferencesWidget)

public:
    explicit ApplicationGeneralPreferencesWidget(QWidget *parent = 0);

    static void writeDefaultSettings(Core::ISettings *s);
    void setDatasToUi();

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_testButton_clicked();
    void toggleDatabaseConfiguration(bool state);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ApplicationGeneralPreferencesWidget *ui;
};

}  // End namespace Internal



class CORE_EXPORT ApplicationGeneralPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    ApplicationGeneralPreferencesPage(QObject *parent = 0);
    ~ApplicationGeneralPreferencesPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::ApplicationGeneralPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::ApplicationGeneralPreferencesWidget> m_Widget;
};



}  // End namespace Core


#endif // APPLICATIONGENERALPREFERENCES_H

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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MFDRUGSPREFERENCES_H
#define MFDRUGSPREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>

#include "ui_drugsviewoptionspage.h"
#include "ui_drugsprintoptionspage.h"
#include "ui_drugsuseroptionspage.h"
#include "ui_drugsextraoptionspage.h"
#include "ui_drugselectorwidget.h"

/**
 * \file mfDrugsPreferences.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 5 Mar 2010
*/

namespace Core {
class ISettings;
}

namespace Print {
class PrinterPreviewer;
}

namespace DrugsDB {
namespace Internal {
class DrugsData;
}
}

namespace DrugsWidget {
namespace Internal {

class DrugsViewWidget : public QWidget, private Ui::DrugsViewWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugsViewWidget)

public:
    explicit DrugsViewWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);
};

class DrugsSelectorWidget : public QWidget, private Ui::DrugsSelectorWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugsSelectorWidget)

public:
    explicit DrugsSelectorWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);
};


class DrugsPrintWidget : public QWidget, private Ui::DrugsPrintWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugsPrintWidget)

public:
    explicit DrugsPrintWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void resetToDefaultFormatting();

private Q_SLOTS:
    void updateFormatting();

protected:
    virtual void changeEvent(QEvent *e);

private:
    DrugsDB::Internal::DrugsData *drug;
};

class DrugsUserWidget : public QWidget, private Ui::DrugsUserWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugsUserWidget)

public:
    explicit DrugsUserWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Print::PrinterPreviewer *previewer;
};

class DrugsExtraWidget : public QWidget, private Ui::DrugsExtraWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugsExtraWidget)

public:
    explicit DrugsExtraWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);
};


}  // End Internal


class DrugsViewOptionsPage : public Core::IOptionsPage
{
public:
    DrugsViewOptionsPage(QObject *parent = 0);
    ~DrugsViewOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsViewWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsViewWidget> m_Widget;
};


class DrugsSelectorOptionsPage : public Core::IOptionsPage
{
public:
    DrugsSelectorOptionsPage(QObject *parent = 0);
    ~DrugsSelectorOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsViewWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
        private:
    QPointer<Internal::DrugsSelectorWidget> m_Widget;
};




class DrugsPrintOptionsPage : public Core::IOptionsPage
{
public:
    DrugsPrintOptionsPage(QObject *parent = 0);
    ~DrugsPrintOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html#params_poso_mask";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsPrintWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsPrintWidget> m_Widget;
};

class DrugsUserOptionsPage : public Core::IOptionsPage
{
public:
    DrugsUserOptionsPage(QObject *parent = 0);
    ~DrugsUserOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsUserWidget::writeDefaultSettings(s);}
    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsUserWidget> m_Widget;
};

class DrugsExtraOptionsPage : public Core::IOptionsPage
{
public:
    DrugsExtraOptionsPage(QObject *parent = 0);
    ~DrugsExtraOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s)  {Internal::DrugsExtraWidget::writeDefaultSettings(s);}
    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsExtraWidget> m_Widget;
};

}  // End Drugs

#endif // MFDRUGSPREFERENCES_H

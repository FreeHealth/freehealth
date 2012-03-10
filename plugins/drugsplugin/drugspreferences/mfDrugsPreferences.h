/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 07 Sept 2011
*/

namespace Core {
class ISettings;
}

namespace Print {
class PrinterPreviewer;
}

namespace DrugsDB {
class IDrug;
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
    DrugsDB::IDrug *drug;
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

class DrugsViewOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugsViewOptionsPage(QObject *parent = 0);
    ~DrugsViewOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsViewWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsViewWidget> m_Widget;
};


class DrugsSelectorOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugsSelectorOptionsPage(QObject *parent = 0);
    ~DrugsSelectorOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsViewWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
        private:
    QPointer<Internal::DrugsSelectorWidget> m_Widget;
};




class DrugsPrintOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugsPrintOptionsPage(QObject *parent = 0);
    ~DrugsPrintOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsPrintWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsPrintWidget> m_Widget;
};

class DrugsUserOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugsUserOptionsPage(QObject *parent = 0);
    ~DrugsUserOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsUserWidget::writeDefaultSettings(s);}
    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsUserWidget> m_Widget;
};

class DrugsExtraOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugsExtraOptionsPage(QObject *parent = 0);
    ~DrugsExtraOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s)  {Internal::DrugsExtraWidget::writeDefaultSettings(s);}
    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsExtraWidget> m_Widget;
};

}  // End namespace Internal
}  // End namespace Drugs

#endif // MFDRUGSPREFERENCES_H

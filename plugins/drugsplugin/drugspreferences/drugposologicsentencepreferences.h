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
#ifndef DRUGS_POSOLOGICSENTENCE_PREFERENCES
#define DRUGS_POSOLOGICSENTENCE_PREFERENCES

#include <coreplugin/ioptionspage.h>

#include <QPointer>

#include "ui_drugposologicsentencepreferences.h"

/**
 * \file drugposologicsentencepreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
class IPadWriter;
}

namespace DrugsDB {
class IDrug;
}

namespace DrugsWidget {
namespace Internal {

#ifdef WITH_PAD
class DrugPosologicSentenceWithPadPreferencesWidget : public QWidget //, private Ui::DrugPosologicSentencePreferencesWithPadWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugPosologicSentenceWithPadPreferencesWidget)

public:
    explicit DrugPosologicSentenceWithPadPreferencesWidget(QWidget *parent = 0);
    ~DrugPosologicSentenceWithPadPreferencesWidget();

    void setDataToUi();

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void resetToDefaultFormatting();

//private Q_SLOTS:
//    void updateFormatting();

protected:
    virtual void changeEvent(QEvent *e);

private:
    DrugsDB::IDrug *_drug;
    Core::IPadWriter *_writer;
};
#endif


class DrugPosologicSentencePreferencesWidget : public QWidget, private Ui::DrugPosologicSentencePreferencesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugPosologicSentencePreferencesWidget)

public:
    explicit DrugPosologicSentencePreferencesWidget(QWidget *parent = 0);
    void setDataToUi();

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

class DrugPosologicSentencePage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugPosologicSentencePage(QObject *parent = 0);
    ~DrugPosologicSentencePage();

    QString displayName() const;
    QString category() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugPosologicSentencePreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
#ifdef WITH_PAD
    QPointer<Internal::DrugPosologicSentenceWithPadPreferencesWidget> m_Widget;
#else
    QPointer<Internal::DrugPosologicSentencePreferencesWidget> m_Widget;
#endif
};


}  // End namespace Internal
}  // End namespace Drugs

#endif // DRUGS_POSOLOGICSENTENCE_PREFERENCES

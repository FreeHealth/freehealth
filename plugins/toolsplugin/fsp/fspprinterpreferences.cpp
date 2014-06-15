/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:                                                       *
 *       Eric Maeker <eric.maeker@gmail.com>                               *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "fspprinterpreferences.h"
#include "fspconstants.h"
#include "../constants.h"
#include "fsp.h"
#include "fspprinter.h"

#include "ui_fspprinterpreferences.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/widgets/imageviewer.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QPixmap>
#include <QPainter>

#include <math.h>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

/* ----------------------  Preferences Widget ---------------------- */

/*! Creates a new preferences widget with a given parent. */
FspPrinterPreferencesWidget::FspPrinterPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FspPrinterPreferencesWidget)
{
    setObjectName("FspPrinterPreferencesWidget");
    // Based on the png image (mm/pixels)
    _xcoef = 143.76 / 566;
    _ycoef = 36.83 / 145;

    ui->setupUi(this);
    ui->viewCerfa->setIcon(theme()->icon(Core::Constants::ICONEYES));
    connect(ui->viewCerfa, SIGNAL(clicked()), this, SLOT(viewCerfa()));
    setDataToUi();
}

FspPrinterPreferencesWidget::~FspPrinterPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void FspPrinterPreferencesWidget::setDataToUi()
{
    if (!_background.load(settings()->path(Core::ISettings::SplashScreenPixmapPath) + "fsp.png"))
        LOG_ERROR("Unable to loa the background image");

    if (settings()->value(Constants::S_DEFAULTCERFA, Constants::S_CERFA_01).toString() == Constants::S_CERFA_01)
        ui->defaultCerfa->setCurrentIndex(0);
    else if (settings()->value(Constants::S_DEFAULTCERFA, Constants::S_CERFA_01).toString() == Constants::S_CERFA_02)
        ui->defaultCerfa->setCurrentIndex(1);
    else
        ui->defaultCerfa->setCurrentIndex(2);

    connect(ui->printTest, SIGNAL(clicked()), this, SLOT(printTest()));
}

/*! \sa IOptionsPage::matches() */
QString FspPrinterPreferencesWidget::searchKeywords() const
{
    QString rc;
    // uncomment this lines to add some keywords from UI elements
    // QTextStream(&rc)
    //        << ui->someLabel->text()
    //        << ui->otherLabel->text();

    rc.remove(QLatin1Char('&'));
    return rc;
}

/*! Saves the settings in the ui to the settings data model. */
void FspPrinterPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    // if no sets given as param, take default interface
    Core::ISettings *s = sets? sets : settings();
    if (ui->defaultCerfa->currentIndex() == 0)
        s->setValue(Constants::S_DEFAULTCERFA, Constants::S_CERFA_01);
    else if (ui->defaultCerfa->currentIndex() == 1)
        s->setValue(Constants::S_DEFAULTCERFA, Constants::S_CERFA_02);
    else
        s->setValue(Constants::S_DEFAULTCERFA, Constants::S_CERFA_02_V2);
}

/*! Writes the default settings to the data model. */
void FspPrinterPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    LOG_FOR("FspPrinterPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FspPrinterPreferencesWidget"));
    s->setValue(Constants::S_DEFAULTCERFA, Constants::S_CERFA_01);
}

/*! Retranslates the ui widgets to the changed language. */
void FspPrinterPreferencesWidget::retranslateUi()
{
}

void FspPrinterPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}

void FspPrinterPreferencesWidget::printTest()
{
    Fsp test;
    test.setData(Fsp::Bill_Number, "123456789012345");
    test.setData(Fsp::Bill_Date, QDate::currentDate());

    test.setData(Fsp::Patient_FullName, "NOM PATIENT ET PRENOM");
    test.setData(Fsp::Patient_DateOfBirth, QDate(1974, 11, 7));
    test.setData(Fsp::Patient_Personal_NSS, "1234567890123");
    test.setData(Fsp::Patient_Personal_NSSKey, "45");
    test.setData(Fsp::Patient_Assurance_Number, "ASSURNBSSDF");
    test.setData(Fsp::Patient_Assure_FullName, "NOM DE L'ASSURÉ");
    test.setData(Fsp::Patient_Assure_NSS, "ASSURE7890123");
    test.setData(Fsp::Patient_Assure_NSSKey, "89");
    test.setData(Fsp::Patient_FullAddress, "ADRESSE DU PATIENT SDFQSD FQSDF QSD FQSD FQSD FQSDFQSDFQSDF QSD F24352345 2345 21345 SQDFQSDF");

    test.setData(Fsp::Condition_Maladie, true);
    test.setData(Fsp::Condition_Maladie_ETM, true);
    test.setData(Fsp::Condition_Maladie_ETM_Ald, true);
    test.setData(Fsp::Condition_Maladie_ETM_Autre, true);
    test.setData(Fsp::Condition_Maladie_ETM_L115, true);
    test.setData(Fsp::Condition_Maladie_ETM_Prevention, true);
    test.setData(Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui, true);
    test.setData(Fsp::Condition_Maladie_ETM_AccidentParTiers_Date, QDate::currentDate());
    test.setData(Fsp::Condition_Maternite, true);
    test.setData(Fsp::Condition_Maternite_Date, QDate::currentDate());
    test.setData(Fsp::Condition_ATMP, true);
    test.setData(Fsp::Condition_ATMP_Number, "12345678901");
    test.setData(Fsp::Condition_ATMP_Date, QDate::currentDate());
    test.setData(Fsp::Condition_NouveauMedTraitant, true);
    test.setData(Fsp::Condition_MedecinEnvoyeur, "MÉDECIN ENVOYEUR - QSFQSFAE QSFQSFAE 23423");
    test.setData(Fsp::Condition_AccesSpecifique, true);
    test.setData(Fsp::Condition_Urgence, true);
    test.setData(Fsp::Condition_HorsResidence, true);
    test.setData(Fsp::Condition_Remplace, true);
    test.setData(Fsp::Condition_HorsCoordination, true);
    test.setData(Fsp::Condition_AccordPrealableDate, QDate::currentDate().addDays(-18));
    test.setData(Fsp::Unpaid_PartObligatoire, true);
    test.setData(Fsp::Unpaid_PartComplementaire, true);

    for(int i=0; i < 4; ++i) {
        test.addAmountData(i, Fsp::Amount_Date, QDate::currentDate());
        test.addAmountData(i, Fsp::Amount_ActCode, "CODE123456");
        test.addAmountData(i, Fsp::Amount_Activity, i);
        test.addAmountData(i, Fsp::Amount_CV, "CV");
        test.addAmountData(i, Fsp::Amount_OtherAct1, "ACT1");
        test.addAmountData(i, Fsp::Amount_OtherAct2, "ACT2");
        test.addAmountData(i, Fsp::Amount_Amount, 234.00);
        test.addAmountData(i, Fsp::Amount_Depassement, 1);
        test.addAmountData(i, Fsp::Amount_Deplacement_IKMD, "IK");
        test.addAmountData(i, Fsp::Amount_Deplacement_Nb, 1);
        test.addAmountData(i, Fsp::Amount_Deplacement_IKValue, 0.97);
    }

    FspPrinter printer;
    printer.setDrawRects(true);
    printer.print(test, FspPrinter::S12541_01, true);
}

void FspPrinterPreferencesWidget::viewCerfa()
{
    Utils::ImageViewer view(this);
    QPixmap background;
    if (ui->defaultCerfa->currentIndex() == 0) {
        if (!background.load(settings()->path(Core::ISettings::ThemeRootPath) + "/pixmap/others/S3110.png", "PNG"))
            qWarning() << "ERROR: unable to load background pixmap";
        view.setPixmap(background);
    } else if (ui->defaultCerfa->currentIndex() == 1) {
        if (!background.load(settings()->path(Core::ISettings::ThemeRootPath) + "/pixmap/others/S3110_02.png", "PNG"))
            qWarning() << "ERROR: unable to load background pixmap";
        view.setPixmap(background);
    } else {
        if (!background.load(settings()->path(Core::ISettings::ThemeRootPath) + "/pixmap/others/S3110_02_v2.png", "PNG"))
            qWarning() << "ERROR: unable to load background pixmap";
        view.setPixmap(background);
    }
    view.exec();
}

/* ----------------------  Preferences Page ---------------------- */

/*!
 * \class Tools::Internal::FspPrinterPreferencesPage
 * \brief Generic FreeMedForms preferences page for Tools plugin.
 *
 *  The page is listed in the settings dialog under the category returned by \sa category().
 */

/*! Creates a new preferences page with a given parent. */
FspPrinterPreferencesPage::FspPrinterPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("FspPrinterPreferencesPage");
}

FspPrinterPreferencesPage::~FspPrinterPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString FspPrinterPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString FspPrinterPreferencesPage::displayName() const
{
    return "Impression de FSP";
}

/*! Returns the (translated) category of the preferences page. */
QString FspPrinterPreferencesPage::category() const
{
    return tr("Tools");
}

/*! Returns the (translated) title of the preferences page. */
QString FspPrinterPreferencesPage::title() const
{
    return "Impression de FSP";
}

/*! Returns the sorting order (pages are sorted starting from 0). */
int FspPrinterPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_TOOLS;
}

/*! Resets the whole preferences page to the default settings of the settings data model. */
void FspPrinterPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void FspPrinterPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void FspPrinterPreferencesPage::finish()
{
    delete m_Widget;
}

/*! \brief Checks if the entered settings are valid.
 *
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void FspPrinterPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_DEFAULTCERFA, Constants::S_CERFA_01);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
}

bool FspPrinterPreferencesPage::matches(const QString &searchKeyWord) const
{
    return m_searchKeywords.contains(searchKeyWord, Qt::CaseInsensitive);
}

/*! Creates the settings page */
QWidget *FspPrinterPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new FspPrinterPreferencesWidget(parent);
    if (m_searchKeywords.isEmpty())
        m_searchKeywords = m_Widget->searchKeywords();
    return m_Widget;
}




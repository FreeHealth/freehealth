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
#include "drugposologicsentencepreferences.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/drugselector.h>
#include <drugsbaseplugin/idrug.h>
#include <drugsbaseplugin/prescriptiontoken.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipadtools.h>

#include <QPixmap>

using namespace DrugsWidget;
using namespace Internal;
using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::IPadTools *padTools() { return Core::ICore::instance()->padTools(); }
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static QString getPrescriptionTokenHtmlFileContent()
{
    QString content;
#ifdef WITH_PAD
    content = Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + QString(DrugsDB::Constants::S_DEF_PRESCRIPTION_TOKENFILE_1_LANG).arg(QLocale().name().left(2).toLower()));
    if (content.isEmpty()) {
        content = Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + QString(DrugsDB::Constants::S_DEF_PRESCRIPTION_TOKENFILE_1_LANG).arg(Trans::Constants::ALL_LANGUAGE));
        if (content.isEmpty()) {
            LOG_ERROR_FOR("DrugPosologicSentencePreferencesWidget", "No token'd prescription file found");
        }
    }
    if (content.contains("<body"))
        content = content.remove("\n");
#else
    // Old style
    content = QCoreApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
#endif
    return content;
}

DrugPosologicSentencePreferencesWidget::DrugPosologicSentencePreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setObjectName("DrugPosologicSentencePreferencesWidget");
    setupUi(this);

    // Create a virtual drug and prescription
    using namespace DrugsDB::Constants;
    drug = drugsBase().getDrugByUID("-1");
    if (!drug) {
        LOG_ERROR("Unable to retreive a drug from the database");
    } else {
        drug->setPrescriptionValue(Prescription::IntakesFrom, 1);
        drug->setPrescriptionValue(Prescription::IntakesTo, 3);
        drug->setPrescriptionValue(Prescription::IntakesScheme, tkTr(Trans::Constants::INTAKES));
        drug->setPrescriptionValue(Prescription::IntakesUsesFromTo, true);
        drug->setPrescriptionValue(Prescription::Period, 2);
        drug->setPrescriptionValue(Prescription::PeriodScheme, tkTr(Trans::Constants::DAY_S));
        drug->setPrescriptionValue(Prescription::IntakesIntervalOfTime, 2);
        drug->setPrescriptionValue(Prescription::IntakesIntervalSchemeIndex, tkTr(Trans::Constants::DAY_S));
        drug->setPrescriptionValue(Prescription::DurationFrom, 1);
        drug->setPrescriptionValue(Prescription::DurationTo, 3);
        drug->setPrescriptionValue(Prescription::DurationScheme, tkTr(Trans::Constants::WEEK_S));
        drug->setPrescriptionValue(Prescription::DurationUsesFromTo, true);
        drug->setPrescriptionValue(Prescription::MealTimeSchemeIndex, 1);
        drug->setPrescriptionValue(Prescription::Note, tr("This a note to take into account<br />written in two lines..."));
        QString daily = "<" + Trans::ConstantTranslations::dailySchemeXmlTagList().at(1) + "=1>";
        daily += "<" + Trans::ConstantTranslations::dailySchemeXmlTagList().at(3) + "=1>";
        daily += "<" + Trans::ConstantTranslations::dailySchemeXmlTagList().at(6) + "=1>";
        drug->setPrescriptionValue(Prescription::SerializedDailyScheme, daily);

        setDatasToUi();

        connect(defaultFormattingButton, SIGNAL(clicked()), this, SLOT(resetToDefaultFormatting()));
        connect(prescriptionFormatting->textEdit(), SIGNAL(textChanged()), this, SLOT(updateFormatting()));
    }
    // formatingSample
}

void DrugPosologicSentencePreferencesWidget::setDatasToUi()
{
    prescriptionFormatting->textEdit()->setHtml(settings()->value(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML).toString());
    updateFormatting();
}

void DrugPosologicSentencePreferencesWidget::resetToDefaultFormatting()
{
    QString content = getPrescriptionTokenHtmlFileContent();
    prescriptionFormatting->setHtml(content);
}

static inline QString getFullPrescription(DrugsDB::IDrug *drug, bool toHtml, const QString &tmp)
{
#ifdef WITH_PAD
    // ~drugsmodel deletes all registered drugs ==> use a copy *drug
    DrugsDB::IDrug *cp = new DrugsDB::IDrug(*drug);
    DrugsDB::DrugsModel model;
    model.addDrug(cp);
    DrugsDB::PrescriptionToken::setPrescriptionModel(&model);
    DrugsDB::PrescriptionToken::setPrescriptionModelRow(0);
    if (toHtml)
        return padTools()->processHtml(tmp);
    return padTools()->processPlainText(tmp);
#else
    return DrugsDB::DrugsModel().getFullPrescription(drug, toHtml, tmp);
#endif
}

void DrugPosologicSentencePreferencesWidget::updateFormatting()
{
    QString tmp = prescriptionFormatting->textEdit()->toHtml();
    formatingSample->setHtml(getFullPrescription(drug, true, tmp));
}

void DrugPosologicSentencePreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    QString tmp = prescriptionFormatting->textEdit()->toHtml();
    tmp = Utils::toHtmlAccent(tmp);
    int cutBegin = tmp.indexOf("<p ");
    int cutEnd = tmp.indexOf("</body>");
    s->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML, tmp.mid(cutBegin, cutEnd-cutBegin));
    s->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN, prescriptionFormatting->textEdit()->toPlainText());
}

void DrugPosologicSentencePreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    LOG_FOR("DrugPosologicSentencePreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DrugPosologicSentencePreferencesWidget"));
    s->setValue(S_CONFIGURED, true);

    QString content = getPrescriptionTokenHtmlFileContent();
    QTextDocument doc;
    doc.setHtml(content);
    s->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML, content);
    s->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN, doc.toPlainText());
}

void DrugPosologicSentencePreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

DrugPosologicSentencePage::DrugPosologicSentencePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("DrugPosologicSentencePage");
}

DrugPosologicSentencePage::~DrugPosologicSentencePage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

QString DrugPosologicSentencePage::id() const { return objectName(); }
QString DrugPosologicSentencePage::name() const { return tr("Printing"); }
QString DrugPosologicSentencePage::category() const { return tkTr(Trans::Constants::DRUGS); }
QString DrugPosologicSentencePage::title() const {return tr("Drug's printing preferences");}
int DrugPosologicSentencePage::sortIndex() const {return 30;}

void DrugPosologicSentencePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DrugPosologicSentencePage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DrugPosologicSentencePage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    QString content = getPrescriptionTokenHtmlFileContent();

    QTextDocument doc;
    doc.setHtml(content);
    defaultvalues.insert(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML, content);
    defaultvalues.insert(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN, doc.toPlainText());

    if (Core::ICore::instance()->updatePreferences()) {
        LOG(tr("Updating posologic sentence."));
        foreach(const QString &k, defaultvalues.keys()) {
            settings()->setValue(k, defaultvalues.value(k));
        }
    }

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }

    defaultvalues.clear();

//    QTextDocument t;
//    t.setHtml(settings()->value(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML).toString());
//    if (settings()->value(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN).isNull() ||
//        settings()->value(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML).isNull() ||
//        t.toPlainText().simplified().isEmpty()) {
//        LOG("Restoring prescription sentence to its defaults");
//        settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN,
//                             qApp->translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
//        settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML,
//                             qApp->translate(Constants::DRUGCONSTANTS_TR_CONTEXT, DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING_PLAIN));
//    }
    settings()->sync();
}

void DrugPosologicSentencePage::finish() { delete m_Widget; }
QString DrugPosologicSentencePage::helpPage()
{
    QString l = QLocale().name().left(2);
    if (l=="fr")
        return Constants::H_PREFERENCES_PRINT_FR;
    return Constants::H_PREFERENCES_PRINT_EN;
}

QWidget *DrugPosologicSentencePage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DrugPosologicSentencePreferencesWidget(parent);
    return m_Widget;
}

/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers:                                                     *
 *       Eric Maeker <eric.maeker@gmail.com>                               *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "printcorrectionpreferences.h"
#include "ui_printcorrectionpreferences.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/printaxishelper.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>

#include <QDebug>

using namespace Print;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

namespace {
enum Horiz {
    ToRight = 0,
    ToLeft
};
enum Vertic {
    ToTop = 0,
    ToBottom
};
}

/* ----------------------  Preferences Widget ---------------------- */

/*! Creates a new preferences widget with a given parent. */
PrintCorrectionPreferencesWidget::PrintCorrectionPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrintCorrectionPreferencesWidget)
{
    ui->setupUi(this);
    connect(ui->printTestWithoutCorrection, SIGNAL(clicked()), this, SLOT(printTest()));
    connect(ui->printTestWithCorrection, SIGNAL(clicked()), this, SLOT(printTest()));
    connect(ui->horiz, SIGNAL(valueChanged(double)), this, SLOT(shiftPreview()));
    connect(ui->horizCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(shiftPreview()));
    connect(ui->vertic, SIGNAL(valueChanged(double)), this, SLOT(shiftPreview()));
    connect(ui->verticCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(shiftPreview()));

    connect(ui->horizDist, SIGNAL(valueChanged(double)), this, SLOT(horizLineDistanceChanged()));
    connect(ui->verticDist, SIGNAL(valueChanged(double)), this, SLOT(verticLineDistanceChanged()));
    setDataToUi();
}

PrintCorrectionPreferencesWidget::~PrintCorrectionPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void PrintCorrectionPreferencesWidget::setDataToUi()
{
    double x = settings()->value(Core::Constants::S_PRINTERCORRECTION_HORIZ_MM, 0.).toDouble();
    double y = settings()->value(Core::Constants::S_PRINTERCORRECTION_VERTIC_MM, 0.).toDouble();
    if (x < 0.) {
        ui->horizCombo->setCurrentIndex(ToLeft);
    } else {
        ui->horizCombo->setCurrentIndex(ToRight);
    }
    if (y < 0.) {
        ui->verticCombo->setCurrentIndex(ToTop);
    } else {
        ui->verticCombo->setCurrentIndex(ToBottom);
    }
    ui->horizDist->setValue(50. + x);
    ui->verticDist->setValue(50. + y);

    if (settings()->value(Core::Constants::S_PRINT_DIRECTION) == Core::Constants::S_TOPTOBOTTOM)
        ui->direction->setCurrentIndex(0);
    else
        ui->direction->setCurrentIndex(1);
    shiftPreview();
}

/*! \sa IOptionsPage::matches() */
QString PrintCorrectionPreferencesWidget::searchKeywords() const
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
void PrintCorrectionPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    // if no sets given as param, take default interface
    Core::ISettings *s = sets? sets : settings();
    double x = 0.;
    double y = 0.;
    if (ui->horizCombo->currentIndex() == ToRight) {
        // ->
        x += ui->horiz->value();
    } else {
        // <-
        x -= ui->horiz->value();
    }
    if (ui->verticCombo->currentIndex() == ToTop) {
        // to top
        y -= ui->vertic->value();
    } else {
        // to bottom
        y += ui->vertic->value();
    }
    s->setValue(Core::Constants::S_PRINTERCORRECTION_HORIZ_MM, x);
    s->setValue(Core::Constants::S_PRINTERCORRECTION_VERTIC_MM, y);
    if (ui->direction->currentIndex() == 0)
        s->setValue(Core::Constants::S_PRINT_DIRECTION, Core::Constants::S_TOPTOBOTTOM);
    else
        s->setValue(Core::Constants::S_PRINT_DIRECTION, Core::Constants::S_BOTTOMTOTOP);
}

/*! Writes the default settings to the data model. */
void PrintCorrectionPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    //    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("PrintCorrectionPreferencesWidget"));
    s->setValue(Core::Constants::S_PRINT_DIRECTION, Core::Constants::S_TOPTOBOTTOM);
}

/*! Retranslates the ui widgets to the changed language. */
void PrintCorrectionPreferencesWidget::retranslateUi()
{
}

bool PrintCorrectionPreferencesWidget::horizLineDistanceChanged()
{
    double y = 50. - ui->horizDist->value();
    if (y < 0.) {
        y = -y;
        ui->horizCombo->setCurrentIndex(0);
    } else {
        ui->horizCombo->setCurrentIndex(1);
    }
    ui->horiz->setValue(y);
    return true;
}

bool PrintCorrectionPreferencesWidget::verticLineDistanceChanged()
{
    double x = ui->verticDist->value() - 50.;
    if (x < 0.) {
        x = -x;
        ui->verticCombo->setCurrentIndex(0);
    } else {
        ui->verticCombo->setCurrentIndex(1);
    }
    ui->vertic->setValue(x);
    return true;
}

bool PrintCorrectionPreferencesWidget::printTest()
{
    QPushButton *sdr = qobject_cast<QPushButton*>(sender());
    if (!sdr)
        return false;

    // try to use the default printer
    QPrintDialog dlg;
    if (dlg.exec()==QDialog::Rejected)
        return false;
    QPrinter *printer = dlg.printer();
    printer->setFullPage(true);
    printer->setPaperSize(QPrinter::A4);
    printer->setResolution(150);

    Utils::PrintAxisHelper axisHelper;
    axisHelper.setPageSize(printer->paperRect(), printer->paperSize(QPrinter::Millimeter));

    QPainter painter;
    if (!painter.begin(printer)) { // failed to open file
        qWarning("failed to open file, is it writable?");
        return false;
    }

    QFont font;
    font.setFamily("Arial Black");
    font.setPointSize(9);
    painter.setFont(font);

    QPen pen(Qt::black, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    painter.setPen(pen);
    painter.save();

    // Print correction: horiz and vertic shift
    if (sdr == ui->printTestWithCorrection) {
        double x = 0.;
        double y = 0.;
        if (ui->horizCombo->currentIndex() == ToRight) {
            // ->
            x += ui->horiz->value();
        } else {
            // <-
            x -= ui->horiz->value();
        }
        if (ui->verticCombo->currentIndex() == ToTop) {
            // to top
            y -= ui->vertic->value();
        } else {
            // to bottom
            y += ui->vertic->value();
        }
        axisHelper.translateMillimeters(-x, -y);
        painter.drawText(axisHelper.pointToPixels(80.0,  200.0), tr("Corrected printing. x shift=%1mm; y shift=%2mm").arg(x).arg(y));
    }

    QPointF top1 = axisHelper.pointToPixels(50.0,  50.0);
    QPointF top2 = axisHelper.pointToPixels(150.0, 50.0);
    QPointF topText = axisHelper.pointToPixels(60.0,  60.0);
    painter.drawLine(top1, top2);
    painter.drawText(topText, "^^^  " + tr("The above line should be at 50mm to the top of the page"));
    topText = axisHelper.pointToPixels(60.0,  70.0);
    painter.drawText(topText, "^^^  " + tr("The above line is printed at _____mm to the top of the page"));

    QPointF left1 = axisHelper.pointToPixels(50.0,  100.0);
    QPointF left2 = axisHelper.pointToPixels(50.0, 200.0);
    QPointF leftText = axisHelper.pointToPixels(60.0,  150.0);
    painter.drawLine(left1, left2);
    painter.drawText(leftText, "<<< " + tr("The line should be at 50mm to the left of the page"));
    leftText = axisHelper.pointToPixels(60.0, 160.0);
    painter.drawText(leftText, "<<< " + tr("The line is printed at _____mm to the left of the page"));

    QPointF squareTopLeft = axisHelper.pointToPixels(100.0,  270.0);
    QSizeF  squareSize = axisHelper.sizeToPixels(10., 10.0);
    painter.drawRect(QRectF(squareTopLeft, squareSize));
    leftText = axisHelper.pointToPixels(70.0,  265.0);
    painter.drawText(leftText, tr("The square should be at 100mm, 270mm to the top left of the page"));

    painter.restore();
    painter.end();
    return true;
}

void PrintCorrectionPreferencesWidget::shiftPreview()
{
    QPixmap image(120*10, 100*10);
    image.fill(Qt::white);

    Utils::PrintAxisHelper axisHelper;
    axisHelper.setPageSize(image.rect(), QSizeF(150, 100));

    QPainter painter;
    if (!painter.begin(&image)) { // failed to open file
        qWarning("failed to open file, is it writable?");
        return;
    }

    QFont font;
    font.setPixelSize(30);
    painter.setFont(font);

    QPen pen(Qt::black, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    painter.setPen(pen);

    painter.save();
    QPointF top1 = axisHelper.pointToPixels(20.0,  20.0);
    QPointF top2 = axisHelper.pointToPixels(80.0, 20.0);
    painter.drawLine(top1, top2);
    QPointF topText = axisHelper.pointToPixels(25.0,  25.0);
    painter.drawText(topText, "^^^  " + tr("The above line should be at 50mm to the top of the page"));

    QPointF left1 = axisHelper.pointToPixels(20.0,  20.0);
    QPointF left2 = axisHelper.pointToPixels(20.0, 80.0);
    painter.drawLine(left1, left2);
    QPointF leftText = axisHelper.pointToPixels(25.0,  75.0);
    painter.drawText(leftText, "<<< " + tr("The line should be at 50mm to the left of the page"));
    painter.restore();

    pen = QPen(Qt::red, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    painter.setPen(pen);
    double x = 0.;
    double y = 0.;
    if (ui->horizCombo->currentIndex() == ToRight) {
        // ->
        x += ui->horiz->value();
    } else {
        // <-
        x -= ui->horiz->value();
    }
    if (ui->verticCombo->currentIndex() == ToTop) {
        // to top
        y -= ui->vertic->value();
    } else {
        // to bottom
        y += ui->vertic->value();
    }
    ui->horizDist->blockSignals(true);
    ui->horizDist->setValue(50. + x);
    ui->horizDist->blockSignals(false);

    ui->verticDist->blockSignals(true);
    ui->verticDist->setValue(50. + y);
    ui->verticDist->blockSignals(false);

    top1 = axisHelper.pointToPixels(20.0 + x,  20.0 + y);
    top2 = axisHelper.pointToPixels(80.0 + x, 20.0 + y);
    painter.drawLine(top1, top2);

    left1 = axisHelper.pointToPixels(20.0 + x,  20.0 + y);
    left2 = axisHelper.pointToPixels(20.0 + x, 80.0 + y);
    painter.drawLine(left1, left2);

    topText = axisHelper.pointToPixels(25.0,  30.0);
    painter.drawText(topText, "^^^  " + tr("The above line is printed at %1mm to the top of the page").arg(50. + y));
    leftText = axisHelper.pointToPixels(25.0,  80.0);
    painter.drawText(leftText, "<<< " + tr("The line is printed at %1mm to the left of the page").arg(50 + x));

    painter.end();

    image = image.scaledToHeight(400, Qt::SmoothTransformation);
    ui->currentPrintLabel->setPixmap(image);
}

void PrintCorrectionPreferencesWidget::changeEvent(QEvent *e)
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

/* ----------------------  Preferences Page ---------------------- */

/*!
 * \class Print::PrintCorrectionPreferencesPage
 * \brief Generic FreeMedForms preferences page for Print plugin.
 *
 *  The page is listed in the settings dialog under the category returned by \sa category().
 */

/*
 * All you have to do is to load this preferences page in the constructor in the Print plugin.
 * Do this like this:
 * \code
 * PrintPlugin::PrintPlugin():
 *     ExtensionSystem::IPlugin()
 * {
 *     _prefPage = new PrintCorrectionPreferencesPage(this);
 *     addObject(_prefPage);
 * }
 * \endcode
 */

/*! Creates a new preferences page with a given parent. */
PrintCorrectionPreferencesPage::PrintCorrectionPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("PrintCorrectionPreferencesPage");
}

PrintCorrectionPreferencesPage::~PrintCorrectionPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString PrintCorrectionPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString PrintCorrectionPreferencesPage::displayName() const
{
    return tr("Print correction");
}

/*! Returns the (translated) category of the preferences page. */
QString PrintCorrectionPreferencesPage::category() const
{
    return tkTr(Trans::Constants::PRINTING);
}

/*! Returns the (translated) title of the preferences page. */
QString PrintCorrectionPreferencesPage::title() const
{
    return tr("Print correction");
}

/*! Returns the sorting order (pages are sorted starting from 0). */
int PrintCorrectionPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_PRINT;
}

/*! Resets the whole preferences page to the default settings of the settings data model. */
void PrintCorrectionPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void PrintCorrectionPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PrintCorrectionPreferencesPage::finish()
{
    delete m_Widget;
}

/*! \brief Checks if the entered settings are valid.
 *
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void PrintCorrectionPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    //    defaultvalues.insert(%PluginName:c%::Constants::FOO_SETTING_KEY, %PluginName:c%::Constants::FOO_SETTING_VALUE);
    defaultvalues.insert(Core::Constants::S_PRINT_DIRECTION, Core::Constants::S_TOPTOBOTTOM);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

bool PrintCorrectionPreferencesPage::matches(const QString &searchKeyWord) const
{
    return m_searchKeywords.contains(searchKeyWord, Qt::CaseInsensitive);
}

/*! Creates the settings page */
QWidget *PrintCorrectionPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PrintCorrectionPreferencesWidget(parent);
    if (m_searchKeywords.isEmpty())
        m_searchKeywords = m_Widget->searchKeywords();
    return m_Widget;
}




/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Core::FullReleasePage
 * FreeToolBox user page that allow users to start and manage all
 * Core::IFullReleaseStep in one action.
 */

/**
 * \class Core::FullReleasePageWidget
 * The widget shows the steps running and its output.\n
 * This object owns the code mechanism for the full release.
 */

#include "fullreleasepage.h"

#include <coreplugin/constants_icons.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ifullreleasestep.h>

#include <utils/log.h>
#include <utils/global.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_fullreleasepage.h"

#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QProgressDialog>
#include <QtConcurrentRun>

using namespace Core;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

FullReleasePageWidget::FullReleasePageWidget(QWidget *parent) :
    QWidget(parent),
    m_FullReleaseProgress(0),
    _currentProcessingStep(-1)
{
    setObjectName("FullReleasePageWidget");
    QVBoxLayout *lcont = new QVBoxLayout(this);
    lcont->setMargin(0);
    lcont->setSpacing(0);
    this->setLayout(lcont);
}

FullReleasePageWidget::~FullReleasePageWidget()
{
    // TODO: check m_Watcher
//    if (m_Watcher) {
//        delete m_Watcher;
//        m_Watcher=0;
//    }
}

/** Starts all Core::IFullReleaseStep in a correct order */
void FullReleasePageWidget::createFullRelease()
{
    // Clear all cache
    m_Steps.clear();
    if (m_FullReleaseProgress) {
        delete m_FullReleaseProgress;
        m_FullReleaseProgress = 0;
    }

    // Create the progress dialog
    m_FullReleaseProgress = new QProgressDialog(this);
    m_FullReleaseProgress->setModal(true);
    m_FullReleaseProgress->show();

    // get all Core::IFullReleaseSteps
    m_Steps = pluginManager()->getObjects<Core::IFullReleaseStep>();
    if (m_Steps.isEmpty()) {
        LOG_ERROR("No step found");
        return;
    }

    // Sort steps
    qSort(m_Steps.begin(), m_Steps.end(), Core::IFullReleaseStep::lessThan);

    // Connect all steps
    foreach(Core::IFullReleaseStep *step, m_Steps)
        connect(step, SIGNAL(subProcessFinished(Core::IFullReleaseStep::ProcessTiming,Core::IFullReleaseStep::SubProcess)), this, SLOT(onSubProcessFinished(Core::IFullReleaseStep::ProcessTiming,Core::IFullReleaseStep::SubProcess)), Qt::UniqueConnection);

    // Start processings
    _currentProcessingStep = 0;
    _currentTiming = Core::IFullReleaseStep::PreProcess;
    _currentSubProcess = Core::IFullReleaseStep::Initialization;
    startProcess();
}

void FullReleasePageWidget::startProcess()
{
    Core::IFullReleaseStep *step = m_Steps.at(_currentProcessingStep);
    QString timing, subProcess;
    switch (_currentTiming) {
    case Core::IFullReleaseStep::PreProcess: timing="PreProcess"; break;
    case Core::IFullReleaseStep::Process: timing="Process"; break;
    case Core::IFullReleaseStep::PostProcess: timing="PostProcess"; break;
    }
    switch (_currentSubProcess) {
    case Core::IFullReleaseStep::Initialization: subProcess="Initialization"; break;
    case Core::IFullReleaseStep::Main: subProcess="Main"; break;
    case Core::IFullReleaseStep::DataPackSubProcess: subProcess="DataPackSubProcess"; break;
    case Core::IFullReleaseStep::Final: subProcess="Final"; break;
    }
    if (m_FullReleaseProgress) {
        m_FullReleaseProgress->setLabelText(QString("Starting process: %1 (%2;%3)").arg(step->id()).arg(timing).arg(subProcess));
    }
    LOG(QString("----------- Starting process: %1 (%2;%3)").arg(step->id()).arg(timing).arg(subProcess));
    if (!step->startProcessing(_currentTiming, _currentSubProcess))
        LOG_ERROR(QString("Unable to start process: %1 (%2;%3)").arg(step->id()).arg(timing).arg(subProcess));
}

void FullReleasePageWidget::onSubProcessFinished(Core::IFullReleaseStep::ProcessTiming timing, Core::IFullReleaseStep::SubProcess subProcess)
{
    // Compute next step
    switch (timing) {
    case Core::IFullReleaseStep::PreProcess:
        _currentTiming=Core::IFullReleaseStep::Process;
        break;
    case Core::IFullReleaseStep::Process:
        _currentTiming=Core::IFullReleaseStep::PostProcess;
        break;
    case Core::IFullReleaseStep::PostProcess:
    {
        _currentTiming = Core::IFullReleaseStep::PreProcess;

        // Compute the next subprocess
        switch (subProcess) {
        case Core::IFullReleaseStep::Initialization:
            _currentSubProcess = Core::IFullReleaseStep::Main;
            break;
        case Core::IFullReleaseStep::Main:
            _currentSubProcess = Core::IFullReleaseStep::DataPackSubProcess;
            break;
        case Core::IFullReleaseStep::DataPackSubProcess:
            _currentSubProcess = Core::IFullReleaseStep::Final;
            break;
        case Core::IFullReleaseStep::Final:
            // Here we have to switch to the next step or finish the procedure
            _currentSubProcess = Core::IFullReleaseStep::Initialization;
            ++_currentProcessingStep;

            // All processes done?
            if (_currentProcessingStep >= m_Steps.count())
                return;

            break;
        }
        break;
    }
    } // switch

    startProcess();
}

/** Set the range of the progress dialog */
void FullReleasePageWidget::setProgressRange(int min, int max)
{
    if (!m_FullReleaseProgress)
        return;
    m_FullReleaseProgress->setRange(min, max);
}

/** Add a running process to the view */
void FullReleasePageWidget::addRunningProcess(const QString &message)
{
    QWidget *w = new QWidget(this);
    QHBoxLayout *l = new QHBoxLayout(w);
    w->setLayout(l);

    QLabel *lblMovie = new QLabel(w);
    lblMovie->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QMovie *movie = new QMovie(theme()->iconFullPath(Core::Constants::ICONSPINNER));
    lblMovie->setMovie(movie);
    movie->start();
    l->addWidget(lblMovie);
    m_IconLabels.insert("__processRunning__", lblMovie);

    QLabel *lbl = new QLabel(w);
    lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lbl->setText(message + "... " + tr("Processing") + "...");
    l->addWidget(lbl);

    l->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Expanding));

    this->layout()->addWidget(w);
}

/** Stops a running process (to the view)stop the spinner) */
void FullReleasePageWidget::endLastAddedProcess()
{
    // Remove movie
    QLabel *lbl = m_IconLabels.value("__processRunning__", 0);
    if (lbl) {
        QMovie *movie = lbl->movie();
        delete movie;
        movie = 0;
        lbl->setMovie(0);
        lbl->setPixmap(theme()->icon(Core::Constants::ICONOK).pixmap(16,16));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////    FullReleasePage    ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FullReleasePage::FullReleasePage(QObject *parent) :
    IToolPage(parent),
    m_Widget(0),
    m_CentralWidget(0),
    ui(0),
    m_Spacer(0)
{
}

FullReleasePage::~FullReleasePage()
{
    if (ui)
        delete ui; ui=0;
    if (m_Spacer)
        delete m_Spacer; m_Spacer = 0;
}

QString FullReleasePage::name() const
{
    return tr("Full release data set");
}

QString FullReleasePage::category() const
{
    return tr("General");
}

QIcon FullReleasePage::icon() const
{
    return theme()->icon(Constants::ICONPROCESS, ITheme::MediumIcon);
}

/** Create the page (return the widget for the page */
QWidget *FullReleasePage::createPage(QWidget *parent)
{
    QWidget *container = new QWidget(parent);
    m_Widget = new FullReleasePageWidget(container);
    ui = new Ui::FullReleasePage;
    ui->setupUi(container);
    ui->scrollArea->setWidget(m_Widget);
    connect(ui->toolButton, SIGNAL(clicked()), m_Widget, SLOT(createFullRelease()));
    return container;
}

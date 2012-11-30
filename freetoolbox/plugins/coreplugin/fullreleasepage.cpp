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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
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
    m_ActiveStep(0),
    m_Watcher(0),
    m_FullReleaseProgress(0)
{
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
    m_ActiveStep = 0;
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
    qSort(m_Steps.begin(), m_Steps.end(), Core::IFullReleaseStep::lessThan);

    // Create dirs of all steps
    foreach(Core::IFullReleaseStep *s, m_Steps) {
        if (!s->createTemporaryStorage()) {
            Utils::warningMessageBox(tr("%1 can not create its temporary directory.").arg(s->id()),
                                     tr("Please report this problem to the devs at: freemedforms@googlegroups.com"));
            return;
        }
        connect(s, SIGNAL(progressLabelChanged(QString)), m_FullReleaseProgress, SLOT(setLabelText(QString)));
        connect(s, SIGNAL(progress(int)), m_FullReleaseProgress, SLOT(setValue(int)));
        connect(s, SIGNAL(progressRangeChanged(qint64,qint64)), this, SLOT(setProgressRange(qint64,qint64)));
    }

    startNextDownload();
}

/** Set the range of the progress dialog */
void FullReleasePageWidget::setProgressRange(qint64 min, qint64 max)
{
    if (!m_FullReleaseProgress)
        return;
    m_FullReleaseProgress->setRange(min, max);
}

/** Start a new download from the registered Core::IFullReleaseStep objects in plugin manager object pool */
void FullReleasePageWidget::startNextDownload()
{
    // Get the step to start
    if (!m_ActiveStep) {
        m_ActiveStep = m_Steps.first();
    } else {
        // Stop running step process
        endDownloadingProcess(m_ActiveStep->id());
        int id = m_Steps.indexOf(m_ActiveStep);
        if (id == (m_Steps.count()-1)) {
            m_ActiveStep = 0;
            // All downloads are done, start processes
            startNextPostProcessDownload();
            return;
        }
        m_ActiveStep = m_Steps.at(id+1);
    }

    // No steps -> Stop all
    if (!m_ActiveStep) {
        if (m_FullReleaseProgress)
            delete m_FullReleaseProgress;
        m_FullReleaseProgress = 0;
        return;
    }

    // Start the downloading
    addDownloadingProcess(m_ActiveStep->processMessage(), m_ActiveStep->id());
    connect(m_ActiveStep, SIGNAL(downloadFinished()), this, SLOT(startNextDownload()));
    if (!m_ActiveStep->startDownload()) {
        // TODO: manage error
        LOG_ERROR("Download was not started? Step: " + m_ActiveStep->id());
    }
}

/** Run the process of the next step (or start the first step) */
void FullReleasePageWidget::startNextProcess()
{
    // Actual process is m_ActiveStep if == 0 start first step
    if (!m_ActiveStep) {
        m_ActiveStep = m_Steps.first();
    } else {
        // Stop running step process
        endLastAddedProcess();
        int id = m_Steps.indexOf(m_ActiveStep);
        // Finished ?
        if (id == (m_Steps.count()-1)) {
            m_ActiveStep = 0;
            startNextProcess();
            return;
        }
        m_ActiveStep = m_Steps.at(id+1);
    }

    // No step -> finished
    if (!m_ActiveStep) {
        if (m_FullReleaseProgress)
            delete m_FullReleaseProgress;
        m_FullReleaseProgress = 0;
        return;
    }

    // Start the step processing
    addRunningProcess(m_ActiveStep->processMessage());
//    if (!m_Watcher) {
//        m_Watcher = new QFutureWatcher<void>;
//    }
//    QFuture<void> future = QtConcurrent::run(m_ActiveStep, &Core::IFullReleaseStep::process);
    QtConcurrent::run(m_ActiveStep, &Core::IFullReleaseStep::process);
    connect(m_ActiveStep, SIGNAL(processFinished()), this, SLOT(startNextProcess()));
//    m_Watcher->setFuture(future);

}

/** Run the post download process of the next step (or start the first step) */
void FullReleasePageWidget::startNextPostProcessDownload()
{
    // Actual process is m_ActiveStep if == 0 start first step
    if (!m_ActiveStep) {
        m_ActiveStep = m_Steps.first();
    } else {
        // Stop running step process
        endDownloadingProcess(m_ActiveStep->id());
        int id = m_Steps.indexOf(m_ActiveStep);
        if (id==(m_Steps.count()-1)) {
            // jobs terminated
            m_ActiveStep = 0;
            if (m_FullReleaseProgress)
                delete m_FullReleaseProgress;
            m_FullReleaseProgress = 0;
            return;
        }
        m_ActiveStep = m_Steps.at(id+1);
    }

    // No step -> finished
    if (!m_ActiveStep) {
        if (m_FullReleaseProgress)
            delete m_FullReleaseProgress;
        m_FullReleaseProgress = 0;
        return;
    }

    // Start the post download processing
    addDownloadingProcess(m_ActiveStep->processMessage(), m_ActiveStep->id());
    connect(m_ActiveStep, SIGNAL(postProcessDownloadFinished()), this, SLOT(startNextPostProcessDownload()));
    m_ActiveStep->postDownloadProcessing();
}

/** Add a downloading process to the view */
void FullReleasePageWidget::addDownloadingProcess(const QString &message, const QString &id)
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
    m_IconLabels.insert(id, lblMovie);

    QLabel *lbl = new QLabel(w);
    lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lbl->setText(message + "... " + tr("Downloading") + "...");
    l->addWidget(lbl);

    l->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Expanding));

    this->layout()->addWidget(w);
}

/** Stops a downloading process (stop the spinner) */
void FullReleasePageWidget::endDownloadingProcess(const QString &id)
{
    // Remove movie
    QLabel *lbl = m_IconLabels.value(id, 0);
    if (lbl) {
        QMovie *movie = lbl->movie();
        delete movie;
        movie = 0;
        lbl->setMovie(0);
        lbl->setPixmap(theme()->icon(Core::Constants::ICONOK).pixmap(16,16));
    }
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
        IToolPage(parent), m_Widget(0), m_CentralWidget(0), ui(0), m_Spacer(0)
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

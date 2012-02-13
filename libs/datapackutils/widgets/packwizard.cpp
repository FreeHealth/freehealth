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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "packwizard.h"
#include <datapackutils/datapackcore.h>
#include <datapackutils/packmanager.h>
#include <datapackutils/serverenginestatus.h>

#include <utils/log.h>

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QGridLayout>
#include <QScrollArea>
#include <QProgressBar>
#include <QMovie>
#include <QAbstractButton>
#include <QTimer>

#include <QDebug>

using namespace DataPack;
using namespace Internal;

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }
static inline Internal::PackManager *packManager() { return qobject_cast<Internal::PackManager*>(core().packManager()); }
static inline QIcon icon(const QString &name, DataPack::DataPackCore::ThemePath path = DataPack::DataPackCore::MediumPixmaps) { return QIcon(core().icon(name, path)); }
static inline QString iconFullPath(const QString &name, DataPack::DataPackCore::ThemePath path = DataPack::DataPackCore::MediumPixmaps) { return core().icon(name, path); }

namespace DataPack {
namespace Internal {
class PackWizardPrivate
{
public:
    QList<Pack> m_InstallPacks;
    QList<Pack> m_UpdatePacks;
    QList<Pack> m_RemovePacks;
};
} // End Internal
} // End DataPack

PackWizard::PackWizard(QWidget *parent) :
    QWizard(parent),
    d(new Internal::PackWizardPrivate)
{
    setPage(IntroPage, new PackIntroPage(this));
    setPage(RemovePacks, new PackRemovePage(this));
    setPage(DownloadPacks, new PackDownloadPage(this));
    setPage(InstallPacks, new PackInstallPage(this));
    setPage(EndPage, new PackEndPage(this));
}

PackWizard::~PackWizard()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void PackWizard::setPackToInstall(const Pack &pack)
{
    setPackToInstall(QList<Pack>() << pack);
}

void PackWizard::setPackToInstall(const QList<Pack> &packs)
{
    d->m_InstallPacks = packs;
}

void PackWizard::setPackToUpdate(const Pack &pack)
{
    setPackToRemove(QList<Pack>() << pack);
}

void PackWizard::setPackToUpdate(const QList<Pack> &packs)
{
    d->m_UpdatePacks = packs;
}

void PackWizard::setPackToRemove(const Pack &pack)
{
    setPackToRemove(QList<Pack>() << pack);
}

void PackWizard::setPackToRemove(const QList<Pack> &packs)
{
    d->m_RemovePacks = packs;
}

void PackWizard::setPackToProcess(const QList<Pack> &installPacks, const QList<Pack> &updatePacks, const QList<Pack> &removePacks)
{
    d->m_InstallPacks = installPacks;
    d->m_UpdatePacks = updatePacks;
    d->m_RemovePacks = removePacks;
}

const QList<Pack> &PackWizard::installPacks() const
{
    return d->m_InstallPacks;
}

const QList<Pack> &PackWizard::removePacks() const
{
    return d->m_RemovePacks;
}

const QList<Pack> &PackWizard::updatePacks() const
{
    return d->m_UpdatePacks;
}

DataPack::PackWizard *PackPage::packWizard() const {return qobject_cast<DataPack::PackWizard*>(wizard());}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  PackIntroPage  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackIntroPage::PackIntroPage(QWidget *parent)
{
    setTitle(tr("Pack processing"));
    setSubTitle(tr("The following packs will be processed. Please verify all informations."));
    m_Browser = new QTextBrowser(this);
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);
    lay->addWidget(m_Browser);
}

static QString toHtml(const QList<Pack> &packs, const QString &title)
{
    QString html;
    if (!packs.isEmpty()) {
        html += QString("<span style\"font-weight:bold;\">%1</span><ul>").arg(title);
        for(int i = 0; i < packs.count(); ++i) {
            html += QString("<li>%1 (%2)</li>").arg(packs.at(i).name()).arg(packs.at(i).version());
        }
        html += "</ul>";
    }
    return html;
}

void PackIntroPage::initializePage()
{
    m_Browser->clear();
    // Create the HTML output of processing packs
    QString html = "<p>";
    // install
    html += toHtml(packWizard()->installPacks(), tr("Pack(s) to install"));
    // update
    html += toHtml(packWizard()->updatePacks(), tr("Pack(s) to update"));
    // remove
    html += toHtml(packWizard()->removePacks(), tr("Pack(s) to remove"));
    html += "</p>";
    m_Browser->setHtml(html);
}

//void PackIntroPage::cleanupPage()
//{
//    initializePage();
//}

bool PackIntroPage::validatePage()
{
    return true;
}

bool PackIntroPage::isComplete() const
{
    return true;
}

int PackIntroPage::nextId() const
{
    if (!packWizard()->removePacks().isEmpty())
        return PackWizard::RemovePacks;
    return PackWizard::DownloadPacks;
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  PackDownloadPage  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackDownloadPage::PackDownloadPage(QWidget *parent) :
    PackPage(parent)
{
    setObjectName("PackDownloadPage");
    setTitle(tr("Downloading packs"));
    setSubTitle(tr("You just have to wait until all packs are downloaded."));
    m_Area = new QScrollArea(this);
    m_Grid = new QGridLayout(m_Area);
    m_Area->setLayout(m_Grid);
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);
    layout()->addWidget(m_Area);

    connect(packManager(), SIGNAL(packDownloaded(DataPack::Pack,DataPack::ServerEngineStatus)),
            this, SLOT(packDownloaded(DataPack::Pack,DataPack::ServerEngineStatus)));
}

static inline QString packKey(const Pack &pack) {return pack.uuid() + pack.vendor() + pack.version();}

static inline QMovie *movie()
{
    QMovie *m = new QMovie(iconFullPath("spinner.png", DataPackCore::SmallPixmaps));
    m->start();
    return m;
}

void PackDownloadPage::initializePage()
{
    m_PackDownloaded.clear();
    m_DownloadPacks.clear();
    m_DownloadPacks << packWizard()->installPacks();
    m_DownloadPacks << packWizard()->updatePacks();

    // Manage UI
    for(int i = 0; i < m_DownloadPacks.count(); ++i) {
        const Pack &pack = m_DownloadPacks.at(i);
        const QString &key = packKey(pack);
        if (!m_PackLabel.keys().contains(key)) {
            // Create a label
            QLabel *processLabel = new QLabel(this);
            processLabel->setPixmap(icon("package.png", DataPackCore::SmallPixmaps).pixmap(16,16));
            processLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            QLabel *label = new QLabel(pack.name() + " " + pack.version(), this);
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

            // Create a progBar
            QProgressBar *bar = new QProgressBar(this);

            // Register both
            m_PackProcessing.insert(key, processLabel);
            m_PackLabel.insert(key, label);
            m_PackBar.insert(key, bar);

            // Put into ui
            int row = m_Grid->rowCount() + 1;
            m_Grid->addWidget(processLabel, row, 0);
            m_Grid->addWidget(label, row, 1);
            QHBoxLayout *hb = new QHBoxLayout;
            QSpacerItem *sp = new QSpacerItem(20,20, QSizePolicy::Fixed, QSizePolicy::Fixed);
            hb->addSpacerItem(sp);
            hb->addWidget(bar);
            m_Grid->addLayout(hb, row + 1, 0, 1, 2);
        }
    }
    QSpacerItem *sp = new QSpacerItem(20,20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout *lay = qobject_cast<QVBoxLayout*>(layout());
    lay->addSpacerItem(sp);

    /** \todo IMPROVE THIS */
    // Start downloads (one pack at time)
    if (!m_DownloadPacks.isEmpty()) {
        // Manage wizard buttons
        packWizard()->button(QWizard::NextButton)->setEnabled(false);
        QTimer::singleShot(2, this, SLOT(startDownloads()));
    }
}

void PackDownloadPage::startDownloads()
{
    // Start downloads
    const QString &k = packKey(m_DownloadPacks.at(0));
    packManager()->downloadPack(m_DownloadPacks.at(0), m_PackBar.value(k));
    //        m_PackProcessing.value(k)->setMovie(movie());
}

//void PackDownloadPage::cleanupPage()
//{
//    // TODO
//    m_PackDownloaded.clear();
//    m_DownloadPacks.clear();
//    m_DownloadPacks << packWizard()->installPacks();
//    m_DownloadPacks << packWizard()->updatePacks();
//    packWizard()->button(QWizard::NextButton)->setEnabled(true);
//}

bool PackDownloadPage::validatePage()
{
    return isComplete();
}

bool PackDownloadPage::isComplete() const
{
//    WARN_FUNC << m_PackDownloaded.count()<<m_DownloadPacks.count() << m_DownloadPacks.isEmpty();
////    qWarning() << m_PackDownloaded;
    return (m_PackDownloaded.count()==m_DownloadPacks.count() && !m_DownloadPacks.isEmpty());
}

int PackDownloadPage::nextId() const
{
    return PackWizard::InstallPacks;
}

void PackDownloadPage::packDownloaded(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status)
{
    const QString &k = packKey(pack);
    QLabel *processLabel = m_PackProcessing.value(k);
    m_PackDownloaded.append(k);

//    WARN_FUNC << k;
    // Correctly manage errors
    if (status.hasError || !status.isSuccessful) {
        processLabel->setPixmap(QPixmap(iconFullPath("warning.png", DataPackCore::SmallPixmaps)));
        processLabel->setToolTip(status.errorMessages.join("\n"));
        LOG_ERROR(status.errorMessages.join("\n"));
    } else {
        processLabel->setPixmap(icon("ok.png", DataPackCore::SmallPixmaps).pixmap(16,16));
        processLabel->setToolTip(status.engineMessages.join("\n"));
    }

    // Start next download
    int id = m_DownloadPacks.indexOf(pack) + 1;
    if (id!=m_DownloadPacks.count()) {
        packManager()->downloadPack(m_DownloadPacks.at(id), m_PackBar.value(k));
    } else {
        allDownloadFinished();
    }
}

void PackDownloadPage::allDownloadFinished()
{
    // Manage wizard buttons
    packWizard()->button(QWizard::NextButton)->setEnabled(true);
    Q_EMIT completeChanged();
    packWizard()->next();
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  PackInstallPage  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackInstallPage::PackInstallPage(QWidget *parent) :
    PackPage(parent)
{
    setTitle(tr("Installing pack(s)"));
    setSubTitle(tr("Wait until all installations are done."));
    m_Area = new QScrollArea(this);
    m_Grid = new QGridLayout(m_Area);
    m_Area->setLayout(m_Grid);
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);
    lay->addWidget(m_Area);

    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)),
            this, SLOT(packInstalled(DataPack::Pack)));
}

void PackInstallPage::initializePage()
{
    m_PackInstalled.clear();
    m_InstallPacks.clear();
    m_InstallPacks << packWizard()->installPacks();
    m_InstallPacks << packWizard()->updatePacks();

    for(int i = 0; i < m_InstallPacks.count(); ++i) {
        const Pack &pack = m_InstallPacks.at(i);
        const QString &key = packKey(pack);
        if (!m_PackLabel.keys().contains(key)) {
            // Create a label
            QLabel *processLabel = new QLabel(this);
            processLabel->setPixmap(icon("package.png", DataPackCore::SmallPixmaps).pixmap(16,16));
            processLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            QLabel *label = new QLabel(pack.name() + " " + pack.version(), this);
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

            // Create a progBar
            QProgressBar *bar = new QProgressBar(this);

            // Register both
            m_PackProcessing.insert(key, processLabel);
            m_PackLabel.insert(key, label);
            m_PackBar.insert(key, bar);

            // Put into ui
            int row = m_Grid->rowCount() + 1;
            m_Grid->addWidget(processLabel, row, 0);
            m_Grid->addWidget(label, row, 1);
            QHBoxLayout *hb = new QHBoxLayout;
            QSpacerItem *sp = new QSpacerItem(20,20, QSizePolicy::Fixed, QSizePolicy::Fixed);
            hb->addSpacerItem(sp);
            hb->addWidget(bar);
            m_Grid->addLayout(hb, row + 1, 0, 1, 2);
        }
    }
    QSpacerItem *sp = new QSpacerItem(20,20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout *lay = qobject_cast<QVBoxLayout*>(layout());
    lay->addSpacerItem(sp);

    /** \todo IMPROVE THIS */
    if (!m_InstallPacks.isEmpty()) {
        packWizard()->button(QWizard::NextButton)->setEnabled(false);
        QTimer::singleShot(2, this, SLOT(startInstalls()));
    }
}

void PackInstallPage::startInstalls()
{
    const QString &k = packKey(m_InstallPacks.at(0));
    packManager()->installDownloadedPack(m_InstallPacks.at(0), m_PackBar.value(k));
}

void PackInstallPage::packInstalled(const DataPack::Pack &pack)
{
    const QString &k = packKey(pack);
    QLabel *processLabel = m_PackProcessing.value(k);
    m_PackInstalled.append(k);

//    // Correctly manage errors
//    if (status.hasError || !status.isSuccessful) {
//        processLabel->setPixmap(QPixmap(iconFullPath("warning.png", DataPackCore::SmallPixmaps)));
//        processLabel->setToolTip(status.errorMessages.join("\n"));
//        LOG_ERROR(status.errorMessages.join("\n"));
//    } else {
        processLabel->setPixmap(icon("ok.png", DataPackCore::SmallPixmaps).pixmap(16,16));
//        processLabel->setToolTip(status.engineMessages.join("\n"));
//    }

    // Start next download
    int id = m_InstallPacks.indexOf(pack) + 1;
    if (id!=m_InstallPacks.count()) {
        packManager()->installDownloadedPack(m_InstallPacks.at(id), m_PackBar.value(k));
    } else {
        allInstallsFinished();
    }
}

void PackInstallPage::allInstallsFinished()
{
    // Manage wizard buttons
    packWizard()->button(QWizard::NextButton)->setEnabled(true);
    Q_EMIT completeChanged();
    packWizard()->next();
}


void PackInstallPage::cleanupPage()
{
    WARN_FUNC;
}

bool PackInstallPage::validatePage()
{
    return isComplete();
}

bool PackInstallPage::isComplete() const
{
    return (m_PackInstalled.count()==m_InstallPacks.count() && !m_InstallPacks.isEmpty());
}

int PackInstallPage::nextId() const
{
    return PackWizard::EndPage;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  PackRemovePage  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackRemovePage::PackRemovePage(QWidget *parent) :
    PackPage(parent)
{
    setTitle(tr("Removing pack(s)"));
    setSubTitle(tr("Wait until all removal are done."));
    m_Area = new QScrollArea(this);
    m_Grid = new QGridLayout(m_Area);
    m_Area->setLayout(m_Grid);
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);
    lay->addWidget(m_Area);
}

void PackRemovePage::initializePage()
{
    for(int i = 0; i < packWizard()->removePacks().count(); ++i) {
        const Pack &pack = packWizard()->removePacks().at(i);
        // Create a label
        QLabel *processLabel = new QLabel(this);
        processLabel->setPixmap(icon("edittrash.png", DataPackCore::SmallPixmaps).pixmap(16,16));
        processLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QLabel *label = new QLabel(pack.name() + " " + pack.version(), this);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // Put into ui
        int row = m_Grid->rowCount() + 1;
        m_Grid->addWidget(processLabel, row, 0);
        m_Grid->addWidget(label, row, 1);
    }
    QSpacerItem *sp = new QSpacerItem(20,20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout *lay = qobject_cast<QVBoxLayout*>(layout());
    lay->addSpacerItem(sp);

    /** \todo IMPROVE THIS */
    if (!packWizard()->removePacks().isEmpty()) {
        packWizard()->button(QWizard::NextButton)->setEnabled(false);
        QTimer::singleShot(2, this, SLOT(startRemoval()));
    }
}

void PackRemovePage::startRemoval()
{
    for(int i = 0; i < packWizard()->removePacks().count(); ++i) {
        const Pack &pack = packWizard()->removePacks().at(i);
        packManager()->removePack(pack);
    }
    packWizard()->next();
}

void PackRemovePage::cleanupPage()
{
    WARN_FUNC;
}

bool PackRemovePage::validatePage()
{
    return true;
}

bool PackRemovePage::isComplete() const
{
    return true;
}

int PackRemovePage::nextId() const
{
    if (!packWizard()->installPacks().isEmpty() || !packWizard()->updatePacks().isEmpty())
        return PackWizard::DownloadPacks;
    return PackWizard::EndPage;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  PackRemovePage  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackEndPage::PackEndPage(QWidget *parent) :
    PackPage(parent)
{
    setTitle(tr("Pack(s) processed."));
    setFinalPage(true);
}

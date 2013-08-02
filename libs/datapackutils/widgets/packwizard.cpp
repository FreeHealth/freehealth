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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class DataPack::PackWizard
  Wizard used by the DataPack::ServerPackEditor for the pack processing:
  install, remove, update.

*/

#include "packwizard.h"
#include <datapackutils/datapackcore.h>
#include <datapackutils/packmanager.h>
#include <datapackutils/serverenginestatus.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/licenseterms.h>

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QGridLayout>
#include <QScrollArea>
#include <QProgressBar>
#include <QMovie>
#include <QAbstractButton>
#include <QCheckBox>
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
    setPage(LicensePacks, new PackLicensePage(this));
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

void PackWizard::done(int result)
{
    core().stopJobsAndClearQueues();
    QWizard::done(result);
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

DataPack::PackWizard *PackPage::packWizard() const
{
    return qobject_cast<DataPack::PackWizard*>(wizard());
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  PackIntroPage  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackIntroPage::PackIntroPage(QWidget *parent) :
    PackPage(parent)
{
    setObjectName("PackIntroPage");
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
    setTitle(tr("Pack processing"));
    setSubTitle(tr("The following packs will be processed. Please verify all information."));
    m_Browser->clear();
    // Create the HTML output of processing packs
    QString html = "<p>";
    // install
    // TODO: better would be tr(" %1 Pack(s) to install/update/remove").arg(packWizard()->installPacks()) to reflect other languages.
    html += toHtml(packWizard()->installPacks(), tr("%1 Pack(s) to install").arg(packWizard()->installPacks().count()));
    // update
    html += toHtml(packWizard()->updatePacks(), tr("%1 Pack(s) to update").arg(packWizard()->updatePacks().count()));
    // remove
    html += toHtml(packWizard()->removePacks(), tr("%1 Pack(s) to remove").arg(packWizard()->removePacks().count()));
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
    return PackWizard::LicensePacks;
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  PackLicensePage  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackLicensePage::PackLicensePage(QWidget *parent) :
    PackPage(parent)
{
    setObjectName("PackLicensePage");
    m_Browser = new QTextBrowser(this);
    m_AgreeBox = new QCheckBox(this);
    m_AgreeBox->setText(tr("&Accept all license terms"));
    m_AgreeBox->setToolTip(m_AgreeBox->text());
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);
    lay->addWidget(m_Browser);
    lay->addWidget(m_AgreeBox);
    connect(m_AgreeBox, SIGNAL(clicked()), this, SIGNAL(completeChanged()));
}

static QString toHtml(const QList<Pack> &packs)
{
    if (packs.isEmpty())
        return QString();
    QString html;
    for(int i = 0; i < packs.count(); ++i) {
        const Pack &pack = packs.at(i);
        html += QString("<p><span style=\"font-weight:bold;font-size:x-large;text-transform:uppercase;background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))\">%1 (%2)</span></p>").arg(pack.name()).arg(pack.version());

        // Get license agreement
        QString licenseTerms;
        const QString &licenseName = pack.description().data(PackDescription::LicenseName).toString().toLower();
        const QString &extraTerms = pack.description().data(PackDescription::LicenseTerms).toString().toLower();
        if ((licenseName.compare("gpl-3")==0) ||
                (licenseName.compare("gplv3")==0) ||
                (licenseName.compare("gpl3")==0)) {
            licenseTerms = Utils::LicenseTerms::getTranslatedLicenseTerms(Utils::LicenseTerms::GPLv3);
        } else if ((licenseName.compare("bsd")==0)) {
            licenseTerms = Utils::LicenseTerms::getTranslatedLicenseTerms(Utils::LicenseTerms::BSD);
        } else if ((licenseName.compare("bsdnew")==0) ||
                   (licenseName.compare("bsd-new")==0) ||
                   (licenseName.compare("bsd new")==0) ||
                   (licenseName.compare("bsdrevised")==0) ||
                   (licenseName.compare("bsd-revised")==0) ||
                   (licenseName.compare("bsd revised")==0)) {
            licenseTerms = Utils::LicenseTerms::getTranslatedLicenseTerms(Utils::LicenseTerms::BSDModified);
        } else if ((licenseName.compare("lgpl")==0) ||
                   (licenseName.compare("lgpl2.1")==0) ||
                   (licenseName.compare("lgpl-2.1")==0) ||
                   (licenseName.compare("lgplv2.1")==0) ||
                   (licenseName.compare("lgpl 2.1")==0)) {
            licenseTerms = Utils::LicenseTerms::getTranslatedLicenseTerms(Utils::LicenseTerms::LGPL);
        }

        if (!licenseTerms.isEmpty())
            html += QString("<p style=\"margin-left:10px;\">%1</p>").arg(licenseTerms);

        if (!extraTerms.isEmpty()) {
            html += QString("<p style=\"margin-left:10px;\">%1</p>").arg(extraTerms);
        }
    }
    return html;
}

void PackLicensePage::initializePage()
{
    setTitle(tr("License agreement"));
    setSubTitle(tr("The following packs need a license agreement."));
    m_Browser->clear();
    // Create the HTML output of processing packs
    QString html = "<p>";
    // install
    html += toHtml(packWizard()->installPacks());
    html += "</p>";
    m_Browser->setHtml(html);
}

//void PackLicensePage::cleanupPage()
//{
//    initializePage();
//}

bool PackLicensePage::validatePage()
{
    return m_AgreeBox->isChecked();
}

bool PackLicensePage::isComplete() const
{
    return m_AgreeBox->isChecked();
}

int PackLicensePage::nextId() const
{
    return PackWizard::DownloadPacks;
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  PackDownloadPage  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackDownloadPage::PackDownloadPage(QWidget *parent) :
    PackPage(parent)
{
    setObjectName("PackDownloadPage");
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
    setTitle(tr("Downloading packs"));
    setSubTitle(tr("Please wait until all packs are downloaded."));
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

    // TODO: IMPROVE THIS
    // Start downloads (one pack at time)
    if (!m_DownloadPacks.isEmpty()) {
        // Manage wizard buttons
        packWizard()->button(QWizard::NextButton)->setEnabled(false);
        QTimer::singleShot(2, this, SLOT(startDownloads()));
    }
    connect(packWizard()->button(QWizard::CancelButton), SIGNAL(clicked()), this, SLOT(cancelDownloads()));

}

void PackDownloadPage::startDownloads()
{
    // Start downloads
    const QString &k = packKey(m_DownloadPacks.at(0));
    packManager()->downloadPack(m_DownloadPacks.at(0), m_PackBar.value(k));
    //        m_PackProcessing.value(k)->setMovie(movie());
}

void PackDownloadPage::cancelDownloads()
{
    core().stopJobsAndClearQueues();
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

    // Correctly manage errors
    if (status.hasError || !status.isSuccessful) {
        processLabel->setPixmap(QPixmap(iconFullPath("warning.png", DataPackCore::SmallPixmaps)));
        processLabel->setToolTip(status.errorMessages.join("\n"));
        Utils::warningMessageBox(tr("Download failed"), status.errorMessages.join("\n"));
        LOG_ERROR(status.errorMessages.join("\n"));
    } else {
        processLabel->setPixmap(icon("ok.png", DataPackCore::SmallPixmaps).pixmap(16,16));
        processLabel->setToolTip(status.engineMessages.join("\n"));
    }

    // Start next download
    int id = m_DownloadPacks.indexOf(pack) + 1;
    if (id != m_DownloadPacks.count()) {
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
    setObjectName("PackInstallPage");
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
    setTitle(tr("%1 pack(s) to install").arg(packWizard()->installPacks().count()));
    setSubTitle(tr("Please wait until all packs are installed."));
    m_PackInstalled.clear();
    m_InstallPacks.clear();
    m_InstallPacks << packWizard()->installPacks();
    m_InstallPacks << packWizard()->updatePacks();

    for(int i = 0; i < m_InstallPacks.count(); ++i) {
        const Pack &pack = m_InstallPacks.at(i);
        const QString &key = packKey(pack);
        if (!m_PackLabel.keys().contains(key)) {
            // Create a label
            QLabel *processIconLabel = new QLabel(this);
            processIconLabel->setPixmap(icon("package.png", DataPackCore::SmallPixmaps).pixmap(16,16));
            processIconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            QLabel *descriptionLabel = new QLabel(QString("%1 %2").arg(pack.name(),pack.version()), this);
            descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            // Create a progBar
            QProgressBar *bar = new QProgressBar(this);

            // Register both
            m_PackProcessing.insert(key, processIconLabel);
            m_PackLabel.insert(key, descriptionLabel);
            m_PackBar.insert(key, bar);

            // Put into ui
            int row = m_Grid->rowCount() + 1;
            m_Grid->addWidget(processIconLabel, row, 0);
            m_Grid->addWidget(descriptionLabel, row, 1);
            QHBoxLayout *hb = new QHBoxLayout();
            QSpacerItem *sp = new QSpacerItem(20,20, QSizePolicy::Fixed, QSizePolicy::Fixed);
            hb->addSpacerItem(sp);
            hb->addWidget(bar);
            m_Grid->addLayout(hb, row + 1, 0, 1, 2);
        }
    }
    QSpacerItem *sp = new QSpacerItem(20,20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout *lay = qobject_cast<QVBoxLayout*>(layout());
    lay->addSpacerItem(sp);

    // TODO: IMPROVE THIS
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
    setObjectName("PackRemovePage");
    m_Area = new QScrollArea(this);
    m_Grid = new QGridLayout(m_Area);
    m_Area->setLayout(m_Grid);
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);
    lay->addWidget(m_Area);
}

void PackRemovePage::initializePage()
{
    setTitle(tr("%1 pack(s) to remove").arg(packWizard()->removePacks().count()));
    setSubTitle(tr("Please wait until all packs are removed."));
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

    // TODO: IMPROVE THIS
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
        return PackWizard::LicensePacks;
    return PackWizard::EndPage;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  PackRemovePage  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
PackEndPage::PackEndPage(QWidget *parent) :
    PackPage(parent)
{
    setObjectName("PackEndPage");
    setTitle(tr("All Pack(s) processed."));
    setFinalPage(true);
}

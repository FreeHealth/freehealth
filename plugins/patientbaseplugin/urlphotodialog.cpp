/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Christian A Reiter                                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "urlphotodialog.h"
#include "ui_urlphotodialog.h"

#include <QUrl>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include <QFile>

#include <utils/httpdownloader.h>
#include <utils/log.h>
#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>

namespace Patients {
namespace Internal {

UrlPhotoDialog::UrlPhotoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UrlPhotoDialog),
    m_alreadyDownloading(false),
    m_alreadyUrlChecking(false),
    m_progressTotal(0)
{
    ui->setupUi(this);
    connect(ui->urlEdit, SIGNAL(textChanged(QString)), this, SLOT(on_urlChanged(QString)));
    m_OkButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    Q_ASSERT(m_OkButton);
    m_OkButton->setDisabled(true);

    ui->errorLabel->hide();
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
    ui->progressBar->setRange(0, 4096);
}

UrlPhotoDialog::~UrlPhotoDialog()
{
    delete ui;
}

QPixmap UrlPhotoDialog::photo() const
{
    const QPixmap *pix = ui->photoLabel->pixmap();
    return pix? pix->copy() : QPixmap();
}

void UrlPhotoDialog::on_urlChanged(const QString &userUrlText)
{
    if (m_alreadyUrlChecking)
        return;

    // try to get a valid URL out of the user input
    QUrl url = QUrl::fromUserInput(userUrlText);
    if (!url.isValid()) {
        ui->photoLabel->setPixmap(QPixmap());
        return;
    }
    QTimer::singleShot(500, this, SLOT(downloadRequested()));
    m_alreadyUrlChecking = true;
}

void UrlPhotoDialog::downloadRequested()
{
    ui->errorLabel->hide();

    if (m_alreadyUrlChecking)
        m_alreadyUrlChecking = false;

    if (m_alreadyDownloading)
        return;

    // try to get a valid URL out of the user input
    QUrl url = QUrl::fromUserInput(ui->urlEdit->text());
    if (!url.isValid()) {
        ui->photoLabel->setPixmap(QPixmap());
        return;
    }
    QString s = url.toString(QUrl::StripTrailingSlash);
    if (! (s.endsWith(".jpg", Qt::CaseInsensitive) ||
           s.endsWith(".jpeg", Qt::CaseInsensitive) ||
           s.endsWith(".png", Qt::CaseInsensitive) ||
           s.endsWith(".gif", Qt::CaseInsensitive))) {
        ui->photoLabel->setPixmap(QPixmap());
        return;
    }

    ui->urlEdit->setDisabled(true);
    m_alreadyDownloading = true;

    // FIXME: manage dialog cancellation. Issue 239
    m_httpDld = new Utils::HttpDownloader(this);
    m_httpDld->setUrl(url);
    m_httpDld->setOutputPath(Core::ICore::instance()->settings()->path(Core::ISettings::ApplicationTempPath));
    connect(m_httpDld, SIGNAL(downloadFinished()), this, SLOT(onDownloadFinished()));
    connect(m_httpDld, SIGNAL(downloadFinished()), m_httpDld, SLOT(deleteLater()));
    connect(m_httpDld, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDownloadProgress(qint64,qint64)));
    m_httpDld->startDownload();
    ui->progressBar->show();
}

void UrlPhotoDialog::onDownloadFinished()
{
    ui->progressBar->hide();
    if( m_httpDld->networkError() != QNetworkReply::NoError) {
        ui->errorLabel->setToolTip(m_httpDld->lastErrorString());
        ui->errorLabel->show();
        ui->photoLabel->setPixmap(QPixmap());
        m_OkButton->setDisabled(true);
        return;
    }

    QPixmap pixmap;
    QString filename = m_httpDld->outputAbsoluteFileName();
    if (!QFile::exists(filename)) {
        LOG_ERROR(QString("Could not save %1.").arg(filename));
        return;
    }

    pixmap.load(m_httpDld->outputAbsoluteFileName());

    ui->photoLabel->setPixmap(pixmap);
    m_alreadyDownloading = false;
    m_OkButton->setEnabled(true);
    ui->urlEdit->setEnabled(true);
    ui->urlEdit->setFocus();
}

void UrlPhotoDialog::updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setRange(0, bytesTotal);
    ui->progressBar->setValue(bytesReceived);
}

} // namespace Internal
} // namespace Patients

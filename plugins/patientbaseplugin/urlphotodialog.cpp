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
 *   Main developers : Christian A Reiter                                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "urlphotodialog.h"
#include "ui_urlphotodialog.h"

#include <QUrl>
#include <QPushButton>
#include <QDebug>
#include <QTimer>

namespace Patients {
namespace Internal {

UrlPhotoDialog::UrlPhotoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UrlPhotoDialog),
    m_alreadyDownloading(false),
    m_alreadyUrlChecking(false)
{
    ui->setupUi(this);
    connect(ui->urlEdit, SIGNAL(textChanged(QString)), this, SLOT(on_urlChanged(QString)));
    m_OkButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    Q_ASSERT(m_OkButton);
    m_OkButton->setDisabled(true);

    ui->errorLabel->hide();
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
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

void UrlPhotoDialog::on_urlChanged(const QString &url)
{
    Q_UNUSED(url)
    if (m_alreadyUrlChecking)
        return;
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

    // FIXME: Use the Utils::HttpDownloader instead and manage dialog cancellation. Issue 239

    m_netAccMan = new QNetworkAccessManager(this);
    //    connect(m_netAccMan, SIGNAL((int,int)), this, SLOT(progress(int,int)));
    m_reply = m_netAccMan->get(QNetworkRequest(url));
    connect(m_netAccMan, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
    ui->progressBar->show();
}

void UrlPhotoDialog::requestFinished(QNetworkReply *reply)
{
    ui->progressBar->hide();
    if( reply->error()) {
        ui->errorLabel->setToolTip(reply->errorString());
        ui->errorLabel->show();
        ui->photoLabel->setPixmap(QPixmap());
        m_OkButton->setDisabled(true);
    }

    QPixmap pixmap;
    pixmap.loadFromData(reply->readAll());
    reply->deleteLater();
    ui->photoLabel->setPixmap(pixmap);
    m_alreadyDownloading = false;
    m_OkButton->setEnabled(true);
    ui->urlEdit->setEnabled(true);
    ui->urlEdit->setFocus();
}

void UrlPhotoDialog::progress(qint64 done, qint64 total)
{
    ui->progressBar->setRange(0, total);
    ui->progressBar->setValue(done);
}

} // namespace Internal
} // namespace Patients

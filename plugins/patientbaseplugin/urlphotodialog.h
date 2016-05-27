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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Christian A Reiter                                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_URLPHOTODIALOG_H
#define PATIENTS_URLPHOTODIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

namespace Utils {
class HttpDownloader;
}

/**
 * \file urlphotodialog.h
 * \author Christian A Reiter
 * \version 0.8.0
 * \date 05 Dec 2012
*/

namespace Patients {
namespace Internal {
namespace Ui {
class UrlPhotoDialog;
}

class UrlPhotoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UrlPhotoDialog(QWidget *parent = 0);
    ~UrlPhotoDialog();

    QPixmap photo() const;

private Q_SLOTS:
    void on_urlChanged(const QString &userUrlText);
    void downloadRequested();
    void onDownloadFinished();

    void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    Ui::UrlPhotoDialog *ui;
    QPushButton *m_OkButton;
    int m_httpGetId;
    Utils::HttpDownloader *m_httpDld;
    QNetworkReply *m_reply;
    QByteArray m_picture;
    bool m_alreadyDownloading, m_alreadyUrlChecking;
    qint64 m_progressTotal;
};

} // namespace Internal
} // namespace Patients

#endif // PATIENTS_URLPHOTODIALOG_H

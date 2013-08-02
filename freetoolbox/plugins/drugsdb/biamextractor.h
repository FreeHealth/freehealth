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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BIAMEXTRACTOR_H
#define BIAMEXTRACTOR_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;

namespace DrugInfos {
namespace Internal {
class BiamExtractorPrivate;
}

namespace Ui {
    class BiamExtractor;
}

class BiamExtractorPage : public Core::IToolPage
{
public:
    BiamExtractorPage(QObject *parent) : IToolPage(parent) {}

    virtual QString id() const {return "BiamExtractorPage";}
    virtual QString name() const {return "BIAM extractor";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};

class BiamExtractor : public QWidget
{
    Q_OBJECT

public:
    explicit BiamExtractor(QWidget *parent = 0);
    ~BiamExtractor();

protected Q_SLOTS:
    bool on_download_clicked();
    void indexPageDownloaded(QNetworkReply *reply);
    void substancePageDownloaded(QNetworkReply *reply);
    void on_process_clicked();

protected:
    void changeEvent(QEvent *e);

Q_SIGNALS:
    void downloadFinished();

private:
    Internal::BiamExtractorPrivate *d;
};

}  // End namespace DrugInfos

#endif // BIAMEXTRACTOR_H

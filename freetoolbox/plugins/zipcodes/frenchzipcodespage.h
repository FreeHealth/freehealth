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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FRENCHZIPCODESPAGE_H
#define FRENCHZIPCODESPAGE_H

#include <coreplugin/ifullreleasestep.h>
#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

QT_BEGIN_NAMESPACE
class QDate;
QT_END_NAMESPACE

/**
 * \file frenchzipcodespage.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 30 Nov 2012
*/

namespace ZipCodes {

class FrenchZipCodesPage : public Core::IToolPage
{
    Q_OBJECT
public:
    FrenchZipCodesPage(QObject *parent = 0);

    virtual QString id() const {return "FrenchZipCodesPage";}
    virtual QString name() const {return tr("ZipCodes: Fr");}
    virtual QString category() const {return Core::Constants::CATEGORY_ZIPCODESDATABASE;}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


class FrenchZipCodesStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    FrenchZipCodesStep(QObject *parent = 0);
    ~FrenchZipCodesStep();

    QString id() const {return "FrenchZipCodesStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::ZipCodes;}
    bool createTemporaryStorage();
    bool cleanTemporaryStorage();
    bool startProcessing(ProcessTiming timing, SubProcess subProcess);

    bool startDownload();
    bool process();
    QString processMessage() const {return tr("French zip codes database creation");}

    bool unzipFiles();
    bool prepareData();
    bool createDatabase();
    bool populateDatabase();
    bool setDatabaseVersion(const QString &connection, const QString &version, const QDate &date);

    bool registerDataPack();

private Q_SLOTS:
    void onSubProcessFinished();

private:
    ProcessTiming _currentTiming;
    SubProcess _currentSubProcess;
};

namespace Ui {
class FrenchZipCodesWidget;
}

class FrenchZipCodesWidget : public QWidget
{
    Q_OBJECT
public:
    FrenchZipCodesWidget(QWidget *parent = 0);
    ~FrenchZipCodesWidget();

private Q_SLOTS:
    void on_startJob_clicked();
    void on_download_clicked();
    void downloadFinished();

private:
    Ui::FrenchZipCodesWidget *ui;
    FrenchZipCodesStep *m_Step;
};


}

#endif // FRENCHZIPCODESPAGE_H

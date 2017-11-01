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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PORTUGUESEDRUGSDATABASE_H
#define PORTUGUESEDRUGSDATABASE_H

#include <coreplugin/ifullreleasestep.h>
#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>

namespace DrugsDbCreator {

class PtDrugsDatabasePage : public Core::IToolPage
{
public:
    PtDrugsDatabasePage(QObject *parent = 0) : Core::IToolPage(parent) {}

    virtual QString id() const {return "PtDrugsDatabasePage";}
    virtual QString name() const {return "Portuguese Drugs Database Creator";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


class PtDrugDatatabaseStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    PtDrugDatatabaseStep(QObject *parent = 0);
    ~PtDrugDatatabaseStep();

    QString id() const {return "PtDrugDatatabaseStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::DrugsDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles(QProgressBar *bar = 0);
    bool process();
    QString processMessage() const {return tr("Portuguese drugs database creation");}

    bool unzipFiles();
    bool prepareDatas();
    bool createDatabase();
    bool populateDatabase();
    bool linkMolecules();

    QStringList errors() const {return m_Errors;}

private:
    QStringList m_Errors;
    bool m_WithProgress;
};


namespace Ui {
    class PortugueseDrugsDatabase;
}

class PortugueseDrugsDatabase : public QWidget
{
    Q_OBJECT

public:
    explicit PortugueseDrugsDatabase(QWidget *parent = 0);
    ~PortugueseDrugsDatabase();

private Q_SLOTS:
    void on_startJobs_clicked();
    bool on_download_clicked();
    void downloadFinished();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PortugueseDrugsDatabase *ui;
    PtDrugDatatabaseStep *m_Step;
};


}  //  End namespace DrugsDbCreator


#endif // PORTUGUESEDRUGSDATABASE_H

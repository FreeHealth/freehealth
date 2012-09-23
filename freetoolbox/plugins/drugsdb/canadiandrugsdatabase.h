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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CANADIANDRUGSDATABASE_H
#define CANADIANDRUGSDATABASE_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/ifullreleasestep.h>

#include <QIcon>
#include <QString>
#include <QWidget>
#include <QMultiHash>
#include <QHash>

namespace DrugsDbCreator {

class CanadianDrugsDatabasePage : public Core::IToolPage
{
public:
    CanadianDrugsDatabasePage(QObject *parent = 0);

    virtual QString id() const {return "CanadianDrugsDatabase";}
    virtual QString name() const {return "Canadian Drugs Database Creator";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


class CaDrugDatatabaseStep : public Core::IFullReleaseStep
{
    Q_OBJECT

public:
    CaDrugDatatabaseStep(QObject *parent = 0);
    ~CaDrugDatatabaseStep();

    QString id() const {return "CaDrugDatatabaseStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::DrugsDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles(QProgressBar *bar = 0);
    bool process();
    QString processMessage() const {return tr("Canadian drugs database creation");}

    bool unzipFiles();
    bool prepareData();
    bool createDatabase();
    bool populateDatabase();
    bool linkDrugsRoutes();
    bool linkMolecules();

    QStringList errors() const {return m_Errors;}

private:
    QMultiHash<int, QString> extractUidRelatedDatas(
            const QString &absFileName,
            const int uidCol, const int dataCol,
            const QHash<int, QString> &equalityCondition);

private:
    QStringList m_Errors;
    bool m_WithProgress;
};


namespace Ui {
    class CanadianDrugsDatabaseWidget;
}

class CanadianDrugsDatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanadianDrugsDatabaseWidget(QWidget *parent = 0);
    ~CanadianDrugsDatabaseWidget();

private Q_SLOTS:
    void on_startJobs_clicked();
    bool on_download_clicked();
    void downloadFinished();

protected:
    void changeEvent(QEvent *e);
    bool unzipFiles();
    bool prepareDatas();
    bool createDatabase();
    bool populateDatabase();
    bool linkMolecules();

private:
    Ui::CanadianDrugsDatabaseWidget *ui;
    CaDrugDatatabaseStep *m_Step;
};

}  //  End namespace DrugsDbCreator


#endif // CANADIANDRUGSDATABASE_H

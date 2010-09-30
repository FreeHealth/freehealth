/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef SOUTHAFRICANDRUGSDATABASE_H
#define SOUTHAFRICANDRUGSDATABASE_H

#include "itoolpage.h"
#include "constants.h"

#include <QWidget>
#include <QMap>
#include <QString>
class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;

namespace Ui {
class SouthAfricanDrugsDatabase;
}


class SouthAfricanDrugsDatabasePage : public IToolPage
{
public:
    SouthAfricanDrugsDatabasePage(QObject *parent) : IToolPage(parent) {}

    virtual QString id() const {return "ZADrugsDatabase";}
    virtual QString name() const {return "ZA Drugs Database Creator";}
    virtual QString category() const {return Constants::CATEGORY_DRUGSDATABASE;}
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

};

class SouthAfricanDrugsDatabase : public QWidget
{
    Q_OBJECT
public:
    SouthAfricanDrugsDatabase(QWidget *parent);
    ~SouthAfricanDrugsDatabase();

protected Q_SLOTS:
    void on_startJobs_clicked();
    void replyFinished(QNetworkReply *reply);

private:
    void changeEvent(QEvent *e);
    bool downloadFiles();
    bool prepareDatas();
    bool createDatabase();
    bool populateDatabase();
    bool linkMolecules();

Q_SIGNALS:
    void downloadFinished();

private:
    Ui::SouthAfricanDrugsDatabase *ui;
    QString m_WorkingPath;
    QNetworkAccessManager *manager;
    QMap<QString, QString> m_Drug_Link;
    int m_nbOfDowloads;
    QProgressDialog *m_Progress;
};

#endif // SOUTHAFRICANDRUGSDATABASE_H

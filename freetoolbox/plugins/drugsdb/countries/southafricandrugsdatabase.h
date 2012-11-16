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
#ifndef SOUTHAFRICANDRUGSDATABASE_H
#define SOUTHAFRICANDRUGSDATABASE_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>
#include <drugsdb/idrugdatabasestep.h>

#include <QWidget>
#include <QMap>
#include <QString>
class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;

namespace DrugsDB {
namespace Internal {
class ZaDrugDatatabaseStep;

class FreeSouthAfricanDrugsDatabasePage : public Core::IToolPage
{
    Q_OBJECT
public:
    explicit FreeSouthAfricanDrugsDatabasePage(QObject *parent = 0);
    ~FreeSouthAfricanDrugsDatabasePage();

    virtual QString id() const {return "FreeSouthAfricanDrugsDatabasePage";}
    virtual QString name() const;
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

private:
    ZaDrugDatatabaseStep *_step;
};

class NonFreeSouthAfricanDrugsDatabasePage : public Core::IToolPage
{
    Q_OBJECT
public:
    explicit NonFreeSouthAfricanDrugsDatabasePage(QObject *parent = 0);
    ~NonFreeSouthAfricanDrugsDatabasePage();

    virtual QString id() const {return "NonFreeSouthAfricanDrugsDatabasePage";}
    virtual QString name() const;
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

private:
    ZaDrugDatatabaseStep *_step;
};

class ZaDrugDatatabaseStep : public DrugsDB::Internal::IDrugDatabaseStep
{
    Q_OBJECT

public:
    ZaDrugDatatabaseStep(QObject *parent = 0);
    ~ZaDrugDatatabaseStep();

    QString id() const {return "ZaDrugDatatabaseStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::DrugsDatabase;}
    void setLicenseType(LicenseType type);

    bool downloadFiles(QProgressBar *bar = 0);
    bool process();
    QString processMessage() const;

    bool prepareData();
    bool populateDatabase();
    bool linkMolecules();

    QStringList errors() const {return m_Errors;}

private Q_SLOTS:
    void replyFinished(QNetworkReply *reply);

private:
    QStringList m_Errors;
    QNetworkAccessManager *manager;
    QMap<QString, QString> m_Drug_Link;
    int m_nbOfDowloads;
    QProgressDialog *m_Progress;
    bool m_WithProgress;
};

}  //  namespace Internal
}  //  End namespace DrugsDB

#endif // SOUTHAFRICANDRUGSDATABASE_H

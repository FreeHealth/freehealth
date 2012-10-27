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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BELGISHDRUGSDATABASE_H
#define BELGISHDRUGSDATABASE_H

#include <drugsdb/idrugdatabasestep.h>
#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>

namespace DrugsDB {
namespace Internal {
class BeDrugDatatabaseStep;

class FreeBeDrugsDatabasePage : public Core::IToolPage
{
    Q_OBJECT
public:
    explicit FreeBeDrugsDatabasePage(QObject *parent = 0);
    ~FreeBeDrugsDatabasePage();

    virtual QString id() const {return "FreeBeDrugsDatabasePage";}
    virtual QString name() const;
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

private:
    BeDrugDatatabaseStep *_step;
};

class NonFreeBeDrugsDatabasePage : public Core::IToolPage
{
    Q_OBJECT
public:
    explicit NonFreeBeDrugsDatabasePage(QObject *parent = 0);
    ~NonFreeBeDrugsDatabasePage();

    virtual QString id() const {return "NonFreeBeDrugsDatabasePage";}
    virtual QString name() const;
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

private:
    BeDrugDatatabaseStep *_step;
};

class BeDrugDatatabaseStep : public DrugsDB::Internal::IDrugDatabaseStep
{
    Q_OBJECT

public:
    BeDrugDatatabaseStep(QObject *parent = 0);
    ~BeDrugDatatabaseStep();

    QString id() const {return "BeDrugDatatabaseStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::DrugsDatabase;}
    void setLicenseType(LicenseType type);
    QString tmpDatabaseAbsPath();

    bool cleanFiles();
    bool downloadFiles(QProgressBar *bar = 0);
    bool process();
    QString processMessage() const;

    bool unzipFiles();
    bool prepareData();
    bool populateDatabase();
    bool linkMolecules();

    QStringList errors() const {return m_Errors;}

private:
    QStringList m_Errors;
    bool m_WithProgress;
};

}  //  namespace Internal
}  //  namespace DrugsDB

#endif // BELGISHDRUGSDATABASE_H

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
 ***************************************************************************/
#ifndef PIMDATABASECREATOR_H
#define PIMDATABASECREATOR_H

#include <coreplugin/ifullreleasestep.h>
#include <coreplugin/itoolpage.h>

#include <QStringList>
#include <QVector>
#include <QWidget>
#include <QDomDocument>
#include <QMultiHash>
#include <QString>


namespace Utils {
    class PubMedDownloader;
}

namespace DrugsDB {
namespace Internal {

class PimStep;

namespace Ui {
    class PimDatabaseCreator;
}

class PimDatabaseCreator : public QWidget
{
    Q_OBJECT

public:
    explicit PimDatabaseCreator(QWidget *parent = 0);
    ~PimDatabaseCreator();

private Q_SLOTS:
    void on_createAndSave_clicked();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PimDatabaseCreator *ui;
    PimStep *m_Step;
};

class PimStep : public Core::IFullReleaseStep
{
    Q_OBJECT
public:
    PimStep(QObject *parent = 0);

    QString id() const {return "PimStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::PimDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles(QProgressBar *bar = 0);
    bool process();

    QString processMessage() const {return tr("Potentially inappropriate medications database creation");}
//    bool postProcessDownload() {m_ActiveDownloadId = -1; downloadNextSource(); return true;}
    bool postProcessDownload() {Q_EMIT postProcessDownloadFinished(); return true;}

    QStringList errors() const {return m_Errors;}

private:
    void savePim(const QDomElement &element, const int sourceId, const int typeId, const QMultiHash<QString, QString> &molNameToAtcCode);

//public Q_SLOTS:
//    void downloadNextSource();

private:
    QStringList m_Errors;
    bool m_UseProgressDialog;
    int m_ActiveDownloadId;
    QVector<int> m_SourceToDownload;
    Utils::PubMedDownloader *m_Downloader;
};

class PimDatabasePage : public Core::IToolPage
{
    Q_OBJECT

public:
    PimDatabasePage(QObject *parent = 0) : IToolPage(parent) {}
    ~PimDatabasePage() {}

    virtual QString id() const {return "PimDatabasePage";}
    virtual QString name() const {return "PIM Database Builder";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0) {return new PimDatabaseCreator(parent);}
};

}  //  namespace Internal
}  //  namespace DrugsDB

#endif // PIMDATABASECREATOR_H

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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_PACKWIZARD_H
#define DATAPACK_PACKWIZARD_H

#include <datapackutils/datapack_exporter.h>
#include <datapackutils/pack.h>
#include <QWizard>
#include <QWizardPage>
#include <QHash>
#include <QString>

QT_BEGIN_NAMESPACE
class QTextBrowser;
class QScrollArea;
class QLabel;
class QProgressBar;
class QGridLayout;
class QCheckBox;
QT_END_NAMESPACE

/**
 * \file packwizard.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.5
 * \date 25 Jun 2012
*/

namespace DataPack {
struct ServerEngineStatus;

namespace Internal {
class PackWizardPrivate;
} // End Internal

class DATAPACK_EXPORT PackWizard : public QWizard
{
    Q_OBJECT
//    friend class DataPack::IServerManager;

public:
    enum {
        IntroPage = 0,
        RemovePacks,
        LicensePacks,
        DownloadPacks,
        InstallPacks,
        UpdatePacks,
        EndPage
    };

    explicit PackWizard(QWidget *parent = 0);
    ~PackWizard();

    void setPackToInstall(const Pack &pack);
    void setPackToInstall(const QList<Pack> &packs);

    void setPackToUpdate(const Pack &pack);
    void setPackToUpdate(const QList<Pack> &packs);

    void setPackToRemove(const Pack &pack);
    void setPackToRemove(const QList<Pack> &packs);

    void setPackToProcess(const QList<Pack> &installPacks, const QList<Pack> &updatePacks, const QList<Pack> &removePacks);

    const QList<Pack> &installPacks() const;
    const QList<Pack> &removePacks() const;
    const QList<Pack> &updatePacks() const;

private Q_SLOTS:
    void done(int result);

private:
    Internal::PackWizardPrivate *d;
};

namespace Internal {

class PackPage : public QWizardPage
{
    Q_OBJECT
public:
    PackPage(QWidget *parent = 0): QWizardPage(parent) {}

    DataPack::PackWizard *packWizard() const;
};

class PackIntroPage : public PackPage
{
    Q_OBJECT
public:
    PackIntroPage(QWidget *parent = 0);

    void initializePage();
//    void cleanupPage();
    bool validatePage();
    bool isComplete() const;
    int nextId() const;

private:
    QTextBrowser *m_Browser;
};

class PackLicensePage : public PackPage
{
    Q_OBJECT
public:
    PackLicensePage(QWidget *parent = 0);

    void initializePage();
    bool validatePage();
    bool isComplete() const;
    int nextId() const;

private:
    QTextBrowser *m_Browser;
    QCheckBox *m_AgreeBox;
    QGridLayout *m_Grid;
};

class PackDownloadPage : public PackPage
{
    Q_OBJECT
public:
    PackDownloadPage(QWidget *parent = 0);

    void initializePage();
//    void cleanupPage();
    bool validatePage();
    bool isComplete() const;
    int nextId() const;

private Q_SLOTS:
    void startDownloads();
    void packDownloaded(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status);

private:
    void allDownloadFinished();

private:
    QScrollArea *m_Area;
    QGridLayout *m_Grid;
    QHash<QString, QLabel*> m_PackLabel;
    QHash<QString, QLabel*> m_PackProcessing;
    QHash<QString, QProgressBar*> m_PackBar;
    QStringList m_PackDownloaded;
    QList<Pack> m_DownloadPacks;
};

class PackInstallPage : public PackPage
{
    Q_OBJECT
public:
    PackInstallPage(QWidget *parent = 0);

    void initializePage();
    void cleanupPage();
    bool validatePage();
    bool isComplete() const;
    int nextId() const;

private Q_SLOTS:
    void startInstalls();
    void packInstalled(const DataPack::Pack &pack);

private:
    void allInstallsFinished();

private:
    QScrollArea *m_Area;
    QGridLayout *m_Grid;
    QHash<QString, QLabel*> m_PackLabel;
    QHash<QString, QLabel*> m_PackProcessing;
    QHash<QString, QProgressBar*> m_PackBar;
    QStringList m_PackInstalled;
    QList<Pack> m_InstallPacks;
};

class PackRemovePage : public PackPage
{
    Q_OBJECT
public:
    PackRemovePage(QWidget *parent = 0);

    void initializePage();
    void cleanupPage();
    bool validatePage();
    bool isComplete() const;
    int nextId() const;

private Q_SLOTS:
    void startRemoval();

private:
    QScrollArea *m_Area;
    QGridLayout *m_Grid;
};

class PackEndPage : public PackPage
{
    Q_OBJECT
public:
    PackEndPage(QWidget *parent = 0);
};

}
} // namespace DataPack

#endif // DATAPACK_PACKWIZARD_H

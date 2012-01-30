/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DATAPACK_PACKPROCESSDIALOG_H
#define DATAPACK_PACKPROCESSDIALOG_H

#include <QDialog>
class QStandardItemModel;
class QGridLayout;

/**
 * \file packprocessdialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 30 Jan 2012
 * \warning Needs Qt >= 4.7
*/

namespace DataPack {
class Pack;
class IServerManager;

namespace Internal {
class IServerEngine;
}

namespace Ui {
    class PackProcessDialog;
}

class PackProcessDialog : public QDialog
{
    Q_OBJECT
    friend class DataPack::IServerManager;

public:
    explicit PackProcessDialog(QWidget *parent = 0);
    ~PackProcessDialog();

    void setPackToInstall(const Pack &pack);
    void setPackToInstall(const QList<Pack> &packs);

    void setPackToUpdate(const Pack &pack);
    void setPackToUpdate(const QList<Pack> &packs);

    void setPackToRemove(const Pack &pack);
    void setPackToRemove(const QList<Pack> &packs);

    void setPackToProcess(const QList<Pack> &installPacks, const QList<Pack> &updatePacks, const QList<Pack> &removePacks);

//    void checkDependencies() const;
//    bool isDependenciesAssumed() const;

protected Q_SLOTS:
    void done(int result);

protected:
    void setServerEngines(const QVector<Internal::IServerEngine*> &engines);

private Q_SLOTS:
    void processPacks();
    void packDownloadDone();

private:
    void startPackDownloads();
    void installPacks();
    void removePacks();
    void clearTemporaries();

private:
    Ui::PackProcessDialog *ui;
    QWidget *m_ScrollWidget;
    QGridLayout *m_ScrollLayout;
    QList<Pack> m_InstallPacks;
    QList<Pack> m_UpdatePacks;
    QList<Pack> m_RemovePacks;
    QVector<Internal::IServerEngine*> m_Engines;
};

}  // End namespace DataPack

#endif // DATAPACK_PackProcessDialog_H

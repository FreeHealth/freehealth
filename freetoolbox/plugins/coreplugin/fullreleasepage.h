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
 ***************************************************************************/
#ifndef FREETOOLBOX_FULLRELEASEPAGE_H
#define FREETOOLBOX_FULLRELEASEPAGE_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ifullreleasestep.h>

#include <QLabel>
#include <QHash>
QT_BEGIN_NAMESPACE
class QProgressDialog;
class QSpacerItem;
QT_END_NAMESPACE

/**
 * \file fullreleasepage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 30 Nov 2012
*/

namespace Core {
class IFullReleaseStep;

namespace Ui {
class FullReleasePage;
}

class FullReleasePageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FullReleasePageWidget(QWidget *parent);
    ~FullReleasePageWidget();

public Q_SLOTS:
    void createFullRelease();

private Q_SLOTS:
    void startProcess();
    void onSubProcessFinished(Core::IFullReleaseStep::ProcessTiming timing, Core::IFullReleaseStep::SubProcess subProcess);
    void setProgressRange(int min, int max);

private:
    void addRunningProcess(const QString &message);
    void endLastAddedProcess();

private:
    QHash<QString, QLabel *> m_IconLabels;
    QProgressDialog *m_FullReleaseProgress;
    QList<Core::IFullReleaseStep*> m_Steps;
    Core::IFullReleaseStep::ProcessTiming _currentTiming;
    Core::IFullReleaseStep::SubProcess _currentSubProcess;
    int _currentProcessingStep;
};

class FullReleasePage : public Core::IToolPage
{
    Q_OBJECT

public:
    FullReleasePage(QObject *parent = 0);
    ~FullReleasePage();

    QString id() const {return "FullReleasePage";}
    QString name() const;
    QString category() const;
    QIcon icon() const;

    // widget will be deleted after the show
    QWidget *createPage(QWidget *parent = 0);

    void addDownloadingProcess(const QString &message, const QString &id);
    void endDownloadingProcess(const QString &id);

    void addRunningProcess(const QString &message);
    void endLastAddedProcess();

private:
    QWidget *m_Widget, *m_CentralWidget;
    Ui::FullReleasePage *ui;
    QSpacerItem *m_Spacer;
};

}  // namespace Core

#endif // FREETOOLBOX_FULLRELEASEPAGE_H

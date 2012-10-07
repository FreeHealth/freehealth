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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_APPCONFIGWIZARD_H
#define CORE_APPCONFIGWIZARD_H

#include <QObject>
#include <QWidget>
#include <QWizardPage>
#include <QWizard>

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QProgressBar;
QT_END_NAMESPACE

/**
 * \file appconfigwizard.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.6
 * \date 28 Jun 2012
*/

namespace Utils {
    class LanguageComboBox;
}

namespace Core {
class ServerPreferencesWidget;

class AppConfigWizard : public QWizard
{
    Q_OBJECT
public:
    AppConfigWizard(QWidget *parent = 0);

protected Q_SLOTS:
    void done(int r);

protected:
    void resizeEvent(QResizeEvent *event);
};


class CoreConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    CoreConfigPage(QWidget *parent = 0);

    bool validatePage();
    int nextId() const;

private:
    void changeEvent(QEvent *e);
    void retranslate();

private:
    QLabel *langLabel, *typeLabel;
    Utils::LanguageComboBox *combo;
    QComboBox *installCombo;
};

class ServerConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    ServerConfigPage(QWidget *parent = 0);

    void initializePage();
    bool isComplete() const;
    bool validatePage();
    int nextId() const;

private:
    void changeEvent(QEvent *e);
    void retranslate();

private:
    Core::ServerPreferencesWidget *serverWidget;
};

class ClientConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    ClientConfigPage(QWidget *parent = 0);

    void initializePage();
    bool isComplete() const;
    bool validatePage();
    int nextId() const;

private:
    void changeEvent(QEvent *e);
    void retranslate();

private:
    Core::ServerPreferencesWidget *serverWidget;
};

class CoreDatabaseCreationPage: public QWizardPage
{
    Q_OBJECT
public:
    CoreDatabaseCreationPage(QWidget *parent = 0);

public:
    void initializePage();
    bool isComplete() const;
    bool validatePage();
    int nextId() const;

private Q_SLOTS:
    void startDbCreation();

private:
    void retranslate();
    void changeEvent(QEvent *e);

private:
    QProgressBar *_progressBar;
    bool _completed;
};

class EndConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    EndConfigPage(QWidget *parent = 0);

public:
    void initializePage();

private Q_SLOTS:
    void comboDbActivated(int);
    void comboVirtualActivated(int);

private:
    void retranslate();
    void changeEvent(QEvent *e);

private:
    QLabel *lblDb;
    QComboBox *comboDb;
    QLabel *lblVirtual;
    QComboBox *comboVirtual;
    QLabel *lbl1, *lbl1_1, *lbl2, *lbl2_1;

};


}  // End namespace Core


#endif // CORE_APPCONFIGWIZARD_H

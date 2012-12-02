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
#ifndef FREETOOLBOX_ATCPAGE_H
#define FREETOOLBOX_ATCPAGE_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>
QT_BEGIN_NAMESPACE
class QLineEdit;
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file atcpage.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 02 Dec 2012
*/

namespace Utils {
class DetailsWidget;
}

namespace DrugsDB {

class AtcPage : public Core::IToolPage
{
    Q_OBJECT
public:
    explicit AtcPage(QObject *parent = 0);

    virtual QString id() const {return "AtcPage";}
    virtual QString name() const {return "ATC";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};

namespace Ui {
class AtcPage;
}

class AtcWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AtcWidget(QWidget *parent = 0);
    ~AtcWidget();

private Q_SLOTS:
    void onAtcCodeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void changeEvent(QEvent *e);

private:
    Ui::AtcPage *ui;
    Utils::DetailsWidget *_details;
    QLineEdit *_code, *_english, *_french, *_deutsch, *_spanish;
};

}  //  End namespace DrugsDB

#endif // FREETOOLBOX_ATCPAGE_H

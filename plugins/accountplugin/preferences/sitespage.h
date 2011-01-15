/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre, GP               *
 *   pm.desombre@medsyn.fr                                                 *
 *   original from Eric Maeker, MD.                                        *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef SITESPAGE_H
#define SITESPAGE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QString>
#include <QDataWidgetMapper>

#include "ui_sitespage.h"

/**
 * \file Sitespage.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 31 Mar 2010
*/

namespace Core {
class ISettings;
}

namespace AccountDB {
class SitesModel;
}

namespace Account {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  SitesWidget  ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SitesWidget : public QWidget, private Ui::SitesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SitesWidget)

public:
    explicit SitesWidget(QWidget *parent = 0);
    ~SitesWidget();
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_mpComboBox_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_save_clicked();
    void on_removeButton_clicked();
   // void on_name_textChanged(const QString & text);

private:
    void changeEvent(QEvent *e);
    void saveModel();

private:
    AccountDB::WorkingPlacesModel *m_Model;
    QDataWidgetMapper *m_Mapper;
    QString m_user_uid;
    QString m_user_fullName;
};

}  // End Internal


class SitesPage : public Core::IOptionsPage
{
public:
    SitesPage(QObject *parent = 0);
    ~SitesPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::SitesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::SitesWidget> m_Widget;
};

}  // End namespace Account

#endif // SitesSPAGE_H

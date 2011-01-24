/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre, GP               *
 *   pm.desombre@medsyn.fr                                                 *
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
#ifndef PERCENTAGESPAGE_H
#define PERCENTAGESPAGE_H
#include "ui_percentagespage.h"
#include <coreplugin/ioptionspage.h>
#include <accountbaseplugin/percentmodel.h>

#include <QPointer>
#include <QString>
#include <QDataWidgetMapper>
#include <QHash>
#include <QSpinBox>


namespace Core {
class ISettings;
}

namespace AccountDB {
class PercentagesModel;
}

namespace Account {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  PercentagesWidget  ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PercentagesWidget : public QWidget, private Ui::PercentagesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(PercentagesWidget)

public:
    explicit PercentagesWidget(QWidget *parent = 0);
    ~PercentagesWidget();
    void setDatasToUi();
    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_percentagesComboBox_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_deleteButton_clicked();

private:
    void changeEvent(QEvent *e);
    void saveModel();
    int calcPercentagesUid();
private:
    AccountDB::PercentModel *m_Model;
    QSpinBox *m_percentageUidLabel;
    QDataWidgetMapper *m_Mapper;
    QString m_user_uid;
    QString m_user_fullName;

};

}  // End Internal


class PercentagesPage : public Core::IOptionsPage
{
public:
    PercentagesPage(QObject *parent = 0);
    ~PercentagesPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::PercentagesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::PercentagesWidget> m_Widget;
};

}  // End namespace Account

#endif // PERCENTAGESSPAGE_H

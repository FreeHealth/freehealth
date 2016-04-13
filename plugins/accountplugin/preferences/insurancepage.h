/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef INSURANCEPAGE_H
#define INSURANCEPAGE_H

#include <coreplugin/ioptionspage.h>
#include <accountbaseplugin/insurancemodel.h>

#include <QPointer>
#include <QString>
#include <QDataWidgetMapper>
#include <QHash>
#include <QSpinBox>
#include <QStandardItemModel>

/**
 * \file insurancepage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace AccountDB {
class InsuranceModel;
}

namespace Account {
namespace Internal {
namespace Ui {
class InsuranceWidget;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  InsuranceWidget  ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class InsuranceWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(InsuranceWidget)

public:
    explicit InsuranceWidget(QWidget *parent = 0);
    ~InsuranceWidget();
    void setDataToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_insuranceComboBox_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_deleteButton_clicked();

private:
    void changeEvent(QEvent *e);
    void saveModel();
    int calcInsuranceUid();

private:
    Ui::InsuranceWidget *ui;
    AccountDB::InsuranceModel *m_Model;
    QSpinBox *m_insuranceUidLabel;
    QDataWidgetMapper *m_Mapper;
    QString m_user_uid;
    QString m_user_fullName;
    QHash<QString,QString> m_hashTownZip;
};

}  // End Internal


class InsurancePage : public Core::IOptionsPage
{
    Q_OBJECT

public:
    InsurancePage(QObject *parent = 0);
    ~InsurancePage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const {return displayName();}
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::InsuranceWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::InsuranceWidget> m_Widget;
};

}  // End namespace Account

#endif // InsuranceSPAGE_H

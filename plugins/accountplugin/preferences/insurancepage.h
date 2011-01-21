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
#ifndef INSURANCEPAGE_H
#define INSURANCEPAGE_H
#include "ui_insurancepage.h"
#include <coreplugin/ioptionspage.h>
#include <accountbaseplugin/insurancemodel.h>

#include <QPointer>
#include <QString>
#include <QDataWidgetMapper>
#include <QHash>
#include <QSpinBox>


namespace Core {
class ISettings;
}

namespace AccountDB {
class InsuranceModel;
}

namespace Account {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  InsuranceWidget  ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class InsuranceWidget : public QWidget, private Ui::InsuranceWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(InsuranceWidget)

public:
    explicit InsuranceWidget(QWidget *parent = 0);
    ~InsuranceWidget();
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_insuranceComboBox_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void findCityFromZipCode(const QString & zipCodeText);
   // void on_name_textChanged(const QString & text);

private:
    void changeEvent(QEvent *e);
    void saveModel();
    QHash<QString,QString> parseZipcodeCsv();
    QStringList listOfCountries();
    QString findCityFromZipCode();
    int calcInsuranceUid();

private:
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
public:
    InsurancePage(QObject *parent = 0);
    ~InsurancePage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::InsuranceWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::InsuranceWidget> m_Widget;
};

}  // End namespace Account

#endif // InsuranceSPAGE_H

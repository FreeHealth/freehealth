/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MEDICALPROCEDURESPAGE_H
#define MEDICALPROCEDURESPAGE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QString>
#include <QDataWidgetMapper>
#include <QStandardItemModel>

#include "ui_medicalprocedurepage.h"

/**
 * \file medicalprocedurepage.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 31 Mar 2010
*/

namespace Core {
class ISettings;
}

namespace AccountDB {
class MedicalProcedureModel;
}

namespace Account {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  MedicalProcedureWidget  ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MedicalProcedureWidget : public QWidget, private Ui::MedicalProcedureWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MedicalProcedureWidget)

public:
    explicit MedicalProcedureWidget(QWidget *parent = 0);
    ~MedicalProcedureWidget();
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_mpComboBox_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_save_clicked();
    void on_removeButton_clicked();
    void on_type_textChanged(const QString & text);
    void on_abstractEdit_textChanged(const QString & text);
   // void on_name_textChanged(const QString & text);

private:
    void changeEvent(QEvent *e);
    void saveModel();
    void setCompletionList(const QString & text);
    void setCompletionAbstractList(const QString & text);
    QStandardItemModel * MedicalProcedureModelByLocale();
    bool fillEmptyMPModel();

private:
    AccountDB::MedicalProcedureModel *m_Model;
    QDataWidgetMapper *m_Mapper;
    QString m_user_uid;
    QString m_user_fullName;
    QStringList m_completionList;
    QStringList m_completionAbstractList;
};

}  // End Internal


class MedicalProcedurePage : public Core::IOptionsPage
{
public:
    MedicalProcedurePage(QObject *parent = 0);
    ~MedicalProcedurePage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::MedicalProcedureWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::MedicalProcedureWidget> m_Widget;
};

}  // End namespace Account

#endif // MEDICALPROCEDURESPAGE_H

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
#ifndef DRUGSCENTRALWIDGET_H
#define DRUGSCENTRALWIDGET_H

#include <drugsplugin/drugs_exporter.h>

#include <QWidget>
#include <QObject>
#include <QListView>

/**
 * \file drugscentralwidget.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 13 Feb 2013
*/

namespace DrugsDB {
class DrugsModel;
}

namespace DrugsWidget {
namespace Internal {
class DrugsContext;
class DrugsActionHandler;
class DrugSelector;

namespace Ui {
class DrugsCentralWidget;
}  // End Ui
}  // End Internal

class PrescriptionViewer;
class DrugsModel;

class DRUGS_EXPORT DrugsCentralWidget : public QWidget
{
    Q_OBJECT
    friend class DrugsWidget::Internal::DrugsActionHandler;

public:
    DrugsCentralWidget(QWidget *parent = 0);
    ~DrugsCentralWidget();

    bool initialize(bool hideSelector = false);
    void clear();

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    void changeFontTo(const QFont &font);
    DrugsDB::DrugsModel *currentDrugsModel() const;

    QListView *prescriptionListView();
    PrescriptionViewer *prescriptionView();
    Internal::DrugSelector *drugSelector();

    void setCurrentSearchMethod(int method);
    bool printPrescription();
    void printPreview();
    bool createTemplate();

    void showDrugsDatabaseInformation();
    void showDosagesDatabaseInformation();

protected:
    void createConnections();
    void disconnect();

private Q_SLOTS:
    // drugs slots
    void onSelectorDrugSelected(const QVariant &drugId);

private:
    void focusInEvent(QFocusEvent *event);

private:
    DrugsDB::DrugsModel *m_CurrentDrugModel;
    Internal::Ui::DrugsCentralWidget *m_ui;
    Internal::DrugsContext *m_Context;
};

}  // End DrugsWidget

#endif // DRUGSCENTRALWIDGET_H

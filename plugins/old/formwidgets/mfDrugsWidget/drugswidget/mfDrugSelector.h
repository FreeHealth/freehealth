/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFDRUGSELECTOR_H
#define MFDRUGSELECTOR_H

// include form headers
#include "ui_mfDrugSelector.h"

// include toolkit headers
#include <QButtonLineEdit.h>

// include Qt headers
#include <QSqlTableModel>
#include <QToolButton>


/**
 * \file mfDrugsSelector.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.5
 * \date 08 July 2009
 */

/**
 \brief Creates a QWidget that contains a search line, and views to drugs and inn. Emit signal drugSelected( int ) when a drug is selected. int is the CIS of the selected drug.
 \ingroup freediams drugswidget
*/
class mfDrugSelector : public QWidget, private Ui::mfDrugSelector
{
    Q_OBJECT
    Q_DISABLE_COPY( mfDrugSelector );

public:
    explicit mfDrugSelector( QWidget *parent = 0 );
    void initialize();

    void setFont( const QFont & font );
    void setFocus(Qt::FocusReason r) { searchLine->setFocus(r); }
    void setFocus() { searchLine->setFocus(); }

    void setSearchMethod(int method);

public Q_SLOTS:
    void retranslateUi( const QString & );

Q_SIGNALS:
    void drugSelected( int CIS );
    void drugSelected( const QModelIndex & index );

private:
    void createDrugsHistoryActions();
    void createToolButtons();
    void createDrugModelView();
    void createINNModelView();
    void createConnections();

private Q_SLOTS:
    // models slots
    void updateModel();                                     // slot called by lineedit to change searched name
    void on_drugsView_doubleClicked( const QModelIndex & ); // index of m_DrugsView
    void on_InnView_clicked( const QModelIndex & index );

    // history slots
    void historyAct_triggered( QAction * action );

private:
    // models
    QSqlTableModel *m_DrugsModel;
    QSqlTableModel *m_InnModel;

    // widgets and actions
    QToolButton  *m_SearchToolButton;      // toolbutton that goes to the searchline (left)
    QToolButton  *m_DrugsHistoricButton;   // toolbutton that goes to the searchline (right)
    QActionGroup *m_HistoryAct;

    // filter for drugs model
    QString          m_filterModel;           // '__replaceit__' must be replaced by search text.

    // search method and history
    int    m_SearchMethod;              /*!< \sa mfDrugsConstants::SearchMethod */
};

#endif // MFDRUGSELECTOR_H

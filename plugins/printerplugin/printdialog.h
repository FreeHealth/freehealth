/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <QList>
#if QT_VERSION < 0x050000
#include <QPrinterInfo>
#else
#include <QtPrintSupport/QPrinterInfo>
#endif

/**
 * \file printdialog.h
 * \author Eric Maeker
 * \version 0.6.2
 * \date 01 Dec 2011
*/

namespace Print {
class Printer;

namespace Internal {
namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT
public:
    PrintDialog(QWidget *parent = 0);
    ~PrintDialog();

    void setPrinter(Print::Printer *printer);
    Print::Printer *printer() const;

    void setTwoNUp(bool state);
    bool isTwoNUp() const;

    void setPdfCache(bool state);
    bool isPdfCacheEnabled() const;

    void previewPage(int n);

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void accept();
    void toFile(QAction *action);
    void on_duplicates_toggled(bool);
    void on_nup_toggled(bool);
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_firstButton_clicked();
    void on_lastButton_clicked();
    void on_pageFrom_valueChanged(int);
    void on_pageTo_valueChanged(int);
    void on_printerCombo_currentIndexChanged(int index);


private:
    Ui::PrintDialog *ui;
    Print::Printer *m_Printer;
    int m_PreviewingPage;
    QList<QPrinterInfo> m_AvailPrinterAtDialogOpens;
    QAction *aSavePdf, *aMailPdf, *aSaveHtml, *aMailHtml;
};

}  // End namespace Internal
}  // End namespace Print

#endif // PRINTDIALOG_H

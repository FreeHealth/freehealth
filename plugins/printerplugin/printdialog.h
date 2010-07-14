/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
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
#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>

/**
 * \file printdialog.h
 * \author Eric Maeker
 * \version 0.4.2
 * \date 13 July 2010
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
    void toPdf();
    void on_duplicatas_toggled(bool);
    void on_nup_toggled(bool);
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_firstButton_clicked();
    void on_lastButton_clicked();
    void on_pageFrom_valueChanged(int);
    void on_pageTo_valueChanged(int);

private:
    Ui::PrintDialog *ui;
    Print::Printer *m_Printer;
    int m_PreviewingPage;
};

}  // End namespace Internal
}  // End namespace Print

#endif // PRINTDIALOG_H

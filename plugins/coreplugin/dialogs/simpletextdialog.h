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
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SIMPLETEXTDIALOG_H
#define SIMPLETEXTDIALOG_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/idocumentprinter.h>

#include <QDialog>
#include <QString>

/**
 * \file simpletextdialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.4
 * \date 10 July 2010
*/


namespace Core {
namespace Internal {
namespace Ui {
class SimpleTextDialog;
}
}  // End Internal

class CORE_EXPORT SimpleTextDialog : public QDialog
{
    Q_OBJECT

public:
    SimpleTextDialog(const QString &title, const QString &zoomSettingKey = QString::null, QWidget *parent = 0);
    ~SimpleTextDialog();

    void setHtml(const QString &html);
    void setPlainText(const QString &text);
    void setHelpPageUrl(const QString &url) {m_HelpUrl = url;}
    void setUserPaper(const int iDocumentPaper) {m_Papers = iDocumentPaper;}
    void setPrintDuplicata(const bool state) {m_Duplicata = state;}

protected:
    void changeEvent(QEvent *e);

protected Q_SLOTS:
    void print();
    void showHelp();
    void zoomIn();
    void zoomOut();

private:
    Internal::Ui::SimpleTextDialog *ui;
    int m_Zoom;
    QString m_Key, m_HelpUrl;
    int m_Papers;
    bool m_Duplicata;
};

}  // End namespace Core

#endif // SIMPLETEXTDIALOG_H

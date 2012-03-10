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
#ifndef TEMPLATESCREATIONDIALOG_H
#define TEMPLATESCREATIONDIALOG_H

#include <templatesplugin/templates_exporter.h>

#include <QDialog>

namespace Templates {
namespace Internal {
namespace Ui {
    class TemplatesCreationDialog;
}  // End namespace Ui
}  // End namespace Internal

class TEMPLATES_EXPORT TemplatesCreationDialog : public QDialog
{
    Q_OBJECT
public:
    TemplatesCreationDialog(QWidget *parent = 0);
    ~TemplatesCreationDialog();

    void setTemplateContent(const QString &content) {m_Content = content;}
    void setTemplateSummary(const QString &summary);
    void setTemplateMimeTypes(const QStringList &list);
    void setUserUuid(const QString &uuid);

protected:
    void done(int r);
    void changeEvent(QEvent *e);

private:
    Internal::Ui::TemplatesCreationDialog *ui;
    QString m_Content;
    QStringList m_Mimes;
};

}  // End namespace Templates

#endif // TEMPLATESCREATIONDIALOG_H

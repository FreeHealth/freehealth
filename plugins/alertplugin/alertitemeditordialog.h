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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERTITEMEDITORDIALOG_H
#define ALERTITEMEDITORDIALOG_H

#include <alertplugin/alertplugin_exporter.h>
#include <QDialog>

namespace Alert {
class AlertItem;

namespace Ui {
class AlertItemEditorDialog;
}

class ALERT_EXPORT AlertItemEditorDialog : public QDialog
{
    Q_OBJECT
public:
    enum EditableParam {
        NoEditableParams     = 0x00000000,
        Label                = 0x00000001,
        Category             = 0x00000002,
        Description          = 0x00000004,
        Icon                 = 0x00000008,
        Relation             = 0x00000010,
        ViewType             = 0x00000100,
        ContentType          = 0x00000200,
        Priority             = 0x00000400,
        Timing               = 0x00000800,
        OverrideNeedsComment = 0x00001000,
        CSS                  = 0x00002000,
        ExtraXml             = 0x00004000,
        Scripts              = 0x00010000,
        Types                = Relation | ViewType | ContentType | Priority | OverrideNeedsComment,
        FullDescription      = Label | Category | Description | Icon,
        AllParams            = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(EditableParams, EditableParam)

    explicit AlertItemEditorDialog(QWidget *parent = 0);
    ~AlertItemEditorDialog();

    void setEditableParams(EditableParams params);

    void setAlertItem(const AlertItem &item);

public Q_SLOTS:
    bool submit(AlertItem &item);
    void reset();

private:
    Ui::AlertItemEditorDialog *ui;
};

}  // namespace Alert

Q_DECLARE_OPERATORS_FOR_FLAGS(Alert::AlertItemEditorDialog::EditableParams)

#endif // ALERTITEMEDITORDIALOG_H

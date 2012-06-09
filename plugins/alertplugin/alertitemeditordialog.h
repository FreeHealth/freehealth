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
class AlertItemEditorWidget;

class ALERT_EXPORT AlertItemEditorDialog : public QDialog
{
    Q_OBJECT
public:
    enum EditableParam {
        NoEditableParams     = 0x00000000,
        Label                = 0x00000001,
        Category             = 0x00000002,
        Description          = 0x00000004,
        Relation             = 0x00000008,
        ViewType             = 0x00000010,
        ContentType          = 0x00000020,
        Priority             = 0x00000040,
        StartDate            = 0x00000080,
        ExpirationDate       = 0x00000100,
        OverrideNeedsComment = 0x00000200,
        CSS                  = 0x00000400,
        ExtraXml             = 0x00000800,
        Script               = 0x00001000,
        AllParams            = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(EditableParams, EditableParam)

    explicit AlertItemEditorDialog(QWidget *parent = 0);
    void setEditableParams(EditableParams params);

    void setAlertItem(const AlertItem &item);

public Q_SLOTS:
    bool submit(AlertItem &item);
    void reset();

private:
    AlertItemEditorWidget *_editor;
};

}  // namespace Alert

Q_DECLARE_OPERATORS_FOR_FLAGS(Alert::AlertItemEditorDialog::EditableParams)

#endif // ALERTITEMEDITORDIALOG_H

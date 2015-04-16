/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 ***************************************************************************/
#ifndef ICDCENTRALWIDGET_H
#define ICDCENTRALWIDGET_H

#include <icdplugin/icd_exporter.h>
#include <icdplugin/icdcontextualwidget.h>
#include <QWidget>

namespace ICD {
class IcdWidgetManager;
class IcdCollectionModel;

namespace Internal {
class IcdCentralWidgetPrivate;
class IcdActionHandler;
}  // End namespace Internal

namespace Ui {
    class IcdCentralWidget;
}

class ICD_EXPORT IcdCentralWidget : public IcdContextualWidget
{
    Q_OBJECT
    friend class ICD::IcdWidgetManager;
    friend class ICD::Internal::IcdActionHandler;

public:
    enum SelectorModes {
        SelectorSimpleMode = 0,
        SelectorFullMode
    };
    enum CollectionModes {
        CollectionSimpleMode = 10,
        CollectionFullMode
    };

    explicit IcdCentralWidget(QWidget *parent = 0);
    ~IcdCentralWidget();

    void setSelectorMode(const SelectorModes mode);
    SelectorModes selectorMode() const;

    void setCollectionMode(const CollectionModes mode);
    CollectionModes collectionMode() const;

    void setIcdCollectionModel(IcdCollectionModel *model);
    IcdCollectionModel *icdCollectionModel() const;

    void openFile(const QString &file);
    void readXmlCollection(const QString &xml);
    QString collectionToXml() const;

    void clear();

protected:
    void toggleSelector();
    void removeItem();
    void print();

    void changeEvent(QEvent *e);

private Q_SLOTS:
    void onSelectorActivated(const QVariant &SID);

private:
    Ui::IcdCentralWidget *ui;
    Internal::IcdCentralWidgetPrivate *d;
};

}  // End namespace ICD

#endif // ICDCENTRALWIDGET_H

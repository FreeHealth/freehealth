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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PIMINTEGRATOR_H
#define PIMINTEGRATOR_H

#include <QWidget>
#include <utils/qabstractxmltreemodel.h>
#include <coreplugin/itoolpage.h>

namespace DrugsDB {
namespace Internal {
class PimTreeModelPrivate;

class PimsTreePage : public Core::IToolPage
{
public:
    PimsTreePage(QObject *parent = 0) : Core::IToolPage(parent) { setObjectName("PimsTreePage"); }

    virtual QString id() const {return "PimsTreePage";}
    virtual QString name() const {return "PimsTreePage";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


class PimTreeModel : public Utils::QAbstractXmlTreeModel
{
public:
    enum DataRepresentation {
        NameOrTranslatedText = 0,
        TypeOrLanguage,
        Level,
        Review,
        ColumnCount
    };

    PimTreeModel(QObject *parent = 0);
    ~PimTreeModel();

    int columnCount(const QModelIndex &) const;

    QVariant data(const QModelIndex &index, int role) const;

};


namespace Ui {
    class PimIntegrator;
}

class PimIntegrator : public QWidget
{
    Q_OBJECT

public:
    explicit PimIntegrator(QWidget *parent = 0);
    ~PimIntegrator();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PimIntegrator *ui;
};

}  //  namespace Internal
}  // End namespace DrugsDB

#endif // PIMINTEGRATOR_H

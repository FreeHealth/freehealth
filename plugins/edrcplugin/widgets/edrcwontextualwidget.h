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
 *   Main Developers:                                                      *
 *       Eric Maeker <eric.maeker@gmail.com>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_INTERNAL_EDRCCONTEXTUALUALWIDGET_H
#define EDRC_INTERNAL_EDRCCONTEXTUALUALWIDGET_H

#include <coreplugin/contextmanager/icontext.h>
#include <QWidget>
#include <QList>

/**
 * \file edrccontextualwidget.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 28 Sept 2013
*/

#include <QDebug>

namespace eDRC {
namespace Internal {
class EdrcContext;

class EdrcContextualWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit EdrcContextualWidget(QWidget *parent = 0);
    ~EdrcContextualWidget();

    EdrcContext *context() const {return m_Context;}

public Q_SLOTS:
    virtual void fileOpen() = 0;
    virtual void fileSave() = 0;
    virtual void fileSaveAs() = 0;
    virtual void fileSavePDF() = 0;
    virtual void filePrint() = 0;
    virtual void filePrintPreview() = 0;
    virtual void editItem() = 0;
    virtual void addItem() {qWarning() << "addItem";}
    virtual void removeItem() = 0;
    virtual void clearItems() = 0;
    virtual void renewItem() = 0;

private:
    EdrcContext *m_Context;
};

class EdrcContext : public Core::IContext
{
    Q_OBJECT
public:
    EdrcContext(EdrcContextualWidget *w) :
        Core::IContext(w)
    {
        setObjectName("EdrcContext");
        setWidget(w);
    }
};

} // namespace Internal
} // namespace eDRC

#endif // EDRC_INTERNAL_EDRCCONTEXTUALUALWIDGET_H


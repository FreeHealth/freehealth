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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef INTERACTIONEDITORPAGE_H
#define INTERACTIONEDITORPAGE_H

#include <coreplugin/itoolpage.h>

#include <QObject>
#include <QIcon>
QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace DrugsDB {
namespace Internal {
class InteractionEditorPage : public Core::IToolPage
{
    Q_OBJECT

public:
    InteractionEditorPage(QObject *parent = 0) : IToolPage(parent) {}
    ~InteractionEditorPage() {}

    virtual QString id() const {return "InteractionEditorPage";}
    virtual QString name() const {return "Interactions editor";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};

class InteractionEditorWidgetPrivate;

class InteractionEditorWidget : public QWidget
{
    Q_OBJECT
public:
    InteractionEditorWidget(QWidget *parent = 0);
    ~InteractionEditorWidget();

protected Q_SLOTS:
    void createNewDDI();
    void filterDrugDrugInteractionModel(const QString &filter);
    void edit();
    void interactionActivated(const QModelIndex &index);
    void save();
    void removeCurrent();
    void translateCurrent();
    void translateAll();
    void reformatOldXmlSource();
    void splitCurrent();

protected Q_SLOTS:
    void translationDone(const QString &trans);

private:
    void setEditorsEnabled(bool state);

protected:
    void changeEvent(QEvent *e);

private:
    InteractionEditorWidgetPrivate *d;
};

}  // namespace Internal
}  // namespace DrugsDB

#endif // INTERACTIONEDITORPAGE_H

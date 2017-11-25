/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACKPLUGIN_INTERNAL_SERVERTOOLWIDGET_H
#define DATAPACKPLUGIN_INTERNAL_SERVERTOOLWIDGET_H

#include <utils/detailsbutton.h>

QT_BEGIN_NAMESPACE
class QToolButton;
class QGraphicsOpacityEffect;
QT_END_NAMESPACE

namespace DataPackPlugin {
namespace Internal {

class FadingWidget : public QWidget
{
    Q_OBJECT
public:
    FadingWidget(QWidget *parent = 0);
    void fadeTo(qreal value);
    qreal opacity();
    void setOpacity(qreal value);
protected:
    QGraphicsOpacityEffect *m_opacityEffect;
};

class ServerToolWidget : public Utils::FadingPanel
{
    Q_OBJECT
public:
    ServerToolWidget(QWidget *parent = 0);

    void fadeTo(qreal value);
    void setOpacity(qreal value);

    void setBuildStepEnabled(bool b);
    void setUpEnabled(bool b);
    void setDownEnabled(bool b);
    void setRemoveEnabled(bool b);
    void setUpVisible(bool b);
    void setDownVisible(bool b);
signals:
    void disabledClicked();
    void upClicked();
    void downClicked();
    void removeClicked();

private:
    QToolButton *m_disableButton;
    QToolButton *m_upButton;
    QToolButton *m_downButton;
    QToolButton *m_removeButton;

    bool m_buildStepEnabled;
    FadingWidget *m_firstWidget;
    FadingWidget *m_secondWidget;
    qreal m_targetOpacity;
};

} // namespace Internal
} // namespace DataPackPlugin

#endif // DATAPACKPLUGIN_INTERNAL_SERVERTOOLWIDGET_H

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
#include "servertoolwidget.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QPropertyAnimation>

using namespace DataPackPlugin;
using namespace Internal;

ServerToolWidget::ServerToolWidget(QWidget *parent)
    : Utils::FadingPanel(parent), m_buildStepEnabled(true)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(4);
    layout->setSpacing(4);
    setLayout(layout);
    m_firstWidget = new FadingWidget(this);
    m_firstWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->setSpacing(0);
    m_firstWidget->setLayout(hbox);
#ifdef Q_OS_MAC
    QSize buttonSize(20, 20);
#else
    QSize buttonSize(20, 26);
#endif

    m_disableButton = new QToolButton(m_firstWidget);
    m_disableButton->setAutoRaise(true);
    m_disableButton->setToolTip(BuildStepListWidget::tr("Disable"));
    m_disableButton->setFixedSize(buttonSize);
    m_disableButton->setIcon(QIcon(QLatin1String(":/projectexplorer/images/disabledbuildstep.png")));
    m_disableButton->setCheckable(true);
    hbox->addWidget(m_disableButton);
    layout->addWidget(m_firstWidget);

    m_secondWidget = new FadingWidget(this);
    m_secondWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    hbox = new QHBoxLayout();
    hbox->setMargin(0);
    hbox->setSpacing(4);
    m_secondWidget->setLayout(hbox);

    m_upButton = new QToolButton(m_secondWidget);
    m_upButton->setAutoRaise(true);
    m_upButton->setToolTip(BuildStepListWidget::tr("Move Up"));
    m_upButton->setFixedSize(buttonSize);
    m_upButton->setIcon(QIcon(QLatin1String(":/core/images/darkarrowup.png")));
    hbox->addWidget(m_upButton);

    m_downButton = new QToolButton(m_secondWidget);
    m_downButton->setAutoRaise(true);
    m_downButton->setToolTip(BuildStepListWidget::tr("Move Down"));
    m_downButton->setFixedSize(buttonSize);
    m_downButton->setIcon(QIcon(QLatin1String(":/core/images/darkarrowdown.png")));
    hbox->addWidget(m_downButton);

    m_removeButton  = new QToolButton(m_secondWidget);
    m_removeButton->setAutoRaise(true);
    m_removeButton->setToolTip(BuildStepListWidget::tr("Remove Item"));
    m_removeButton->setFixedSize(buttonSize);
    m_removeButton->setIcon(QIcon(QLatin1String(":/core/images/darkclose.png")));
    hbox->addWidget(m_removeButton);

    layout->addWidget(m_secondWidget);

    connect(m_disableButton, SIGNAL(clicked()), this, SIGNAL(disabledClicked()));
    connect(m_upButton, SIGNAL(clicked()), this, SIGNAL(upClicked()));
    connect(m_downButton, SIGNAL(clicked()), this, SIGNAL(downClicked()));
    connect(m_removeButton, SIGNAL(clicked()), this, SIGNAL(removeClicked()));
}

void ServerToolWidget::setOpacity(qreal value)
{
    m_targetOpacity = value;
    if (m_buildStepEnabled)
        m_firstWidget->setOpacity(value);
    m_secondWidget->setOpacity(value);
}

void ServerToolWidget::fadeTo(qreal value)
{
    m_targetOpacity = value;
    if (m_buildStepEnabled)
        m_firstWidget->fadeTo(value);
    m_secondWidget->fadeTo(value);
}

void ServerToolWidget::setBuildStepEnabled(bool b)
{
    m_buildStepEnabled = b;
    if (m_buildStepEnabled) {
#ifdef Q_OS_MAC
        m_firstWidget->setOpacity(m_targetOpacity);
#else
        m_firstWidget->fadeTo(m_targetOpacity);
#endif
    } else {
#ifdef Q_OS_MAC
        m_firstWidget->setOpacity(1.0);
#else
        m_firstWidget->fadeTo(1.0);
#endif
    }
    m_disableButton->setChecked(!b);
}

void ServerToolWidget::setUpEnabled(bool b)
{
    m_upButton->setEnabled(b);
}

void ServerToolWidget::setDownEnabled(bool b)
{
    m_downButton->setEnabled(b);
}

void ServerToolWidget::setRemoveEnabled(bool b)
{
    m_removeButton->setEnabled(b);
}

void ServerToolWidget::setUpVisible(bool b)
{
    m_upButton->setVisible(b);
}

void ServerToolWidget::setDownVisible(bool b)
{
    m_downButton->setVisible(b);
}


FadingWidget::FadingWidget(QWidget *parent) :
    QWidget(parent),
    m_opacityEffect(new QGraphicsOpacityEffect)
{
    m_opacityEffect->setOpacity(0);
    setGraphicsEffect(m_opacityEffect);

    // Workaround for issue with QGraphicsEffect. GraphicsEffect
    // currently clears with Window color. Remove if flickering
    // no longer occurs on fade-in
    QPalette pal;
    pal.setBrush(QPalette::All, QPalette::Window, Qt::transparent);
    setPalette(pal);
}

void FadingWidget::setOpacity(qreal value)
{
    m_opacityEffect->setOpacity(value);
}

void FadingWidget::fadeTo(qreal value)
{
    QPropertyAnimation *animation = new QPropertyAnimation(m_opacityEffect, "opacity");
    animation->setDuration(200);
    animation->setEndValue(value);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

qreal FadingWidget::opacity()
{
    return m_opacityEffect->opacity();
}


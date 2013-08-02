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
 *   Main developers : Christian A Reiter                                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "addremovecombobox.h"

#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_icons.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/constants.h>

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStylePainter>
#include <QDebug>

using namespace Views;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

/*!
 * \class Views::AddRemoveComboBox
 * \brief This class provides a QComboBox with a label and a +/- button to add/remove items.
 *
 * The widget can be used instead of a QComboBox. Just set the model via setModel(), and connect to the events
 * for easy interactions with the UI around. Set the Model column via setModelColumn()
 *
*/

/*!
 * \fn void AddRemoveComboBox::setModel(QAbstractItemModel *model)
 * \brief Sets the internal model of the ComboBox.
 */

/*!
 * \fn void AddRemoveComboBox::setModelColumn(int column)
 * \brief Sets the model column that is shown in the ComboBox.
 */

/*!
 * \fn int AddRemoveComboBox::currentIndex() const
 * \brief returns the currentIndex() of the ComboBox model.
 */

/*!
 * \fn void AddRemoveComboBox::setCurrentIndex(int index)
 * \brief sets the currentIndex() of the ComboBox model.
 */


/*! Default constructor */
AddRemoveComboBox::AddRemoveComboBox(QWidget *parent) :
    QWidget(parent),
    minimumRowsAllowed(0)
{
    mLabel = new QLabel("");
    initialize();
}

/*! \brief Constructor with a given \e labelText
 *
 * Per default the label is placed on the left side of the ComboBox.
 */
AddRemoveComboBox::AddRemoveComboBox(const QString &labelText, QWidget *parent) :
    QWidget(parent),
    minimumRowsAllowed(0)
{
    mLabel = new QLabel(labelText);
    initialize();
}

/*! Does all the work for the constructor: button creating, signal/slot connections. */
void AddRemoveComboBox::initialize()
{
    QHBoxLayout *layout = new QHBoxLayout();

    mCombo = new QComboBox(this);
    mCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mLabel->setBuddy(mCombo);

    mAddButton = new QPushButton(this);
    mAddButton->setMinimumSize(22, 22);
    mAddButton->setMaximumSize(22, 22);
    mAddButton->setIcon(theme()->icon(Core::Constants::ICONADD));

    mRemoveButton = new QPushButton(this);
    mRemoveButton->setMinimumSize(22, 22);
    mRemoveButton->setMaximumSize(22, 22);
    mRemoveButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    mRemoveButton->setEnabled(false);

    layout->addWidget(mLabel);
    layout->addWidget(mCombo);
    layout->addWidget(mAddButton);
    layout->addWidget(mRemoveButton);
    setLayout(layout);

    connect(mAddButton, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeItem()));
    connect(mCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(currentIndexChanged(int)));

    // translate int signals to QModelIndex signals
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(translateIntIndexChanged(int)));

    connect(mCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUi()));
}

/*! Default destructor*/
AddRemoveComboBox::~AddRemoveComboBox()
{
}

/*! Sets the label on the left of the QComboBox. */
void AddRemoveComboBox::setLabel(const QString &text)
{
    Q_ASSERT(mLabel);
    mLabel->setText(text);
}

/*! \brief Appends a new item to the ComboBox' model.
 *
 * After successfully inserting the item, the itemAdded(int) signal is emitted.
 * \sa removeItem, itemAdded, itemRemoved
 */
void AddRemoveComboBox::addItem()
{
    QAbstractItemModel *model = mCombo->model();
    if (!model)
        return;
    // insert an empty row
    if(!model->insertRow(model->rowCount()))
        LOG_ERROR("Unable to add row " + QString::number(mCombo->currentIndex()));

    // set the ComboBox to that row
    mCombo->setCurrentIndex(model->rowCount()-1);
    mRemoveButton->setEnabled(true);

    //inform other widgets of successfully added item
    Q_EMIT itemAdded(model->index(model->rowCount()-1, 0));
}

/*! \brief Removes the currentItem from the combobox model
 *
 * After succesful removal, the itemRemoved() signal is emitted.
 * If there are only the minimum allowed rows in the model, the method just returns.
 * \sa addItem(), itemAdded(), itemRemoved(), setMinimumRowsAllowed()
 */
void AddRemoveComboBox::removeItem()
{
    QAbstractItemModel *model = mCombo->model();
    if (model->rowCount() == minimumRowsAllowed)
        return;

    if (!model->removeRow(mCombo->currentIndex())) {
          LOG_ERROR("Unable to remove row " + QString::number(mCombo->currentIndex()));
    }
    // check if there are any items left afterwords
    const bool enabled = (model->rowCount() > 0);
    mRemoveButton->setEnabled(enabled);
//    mCombo->setCurrentIndex(mCombo->count() - 1);

    // inform other widgets of change
    Q_EMIT itemRemoved();

}

/*! Updates the visual state of the UI: enables/disables widgets etc. */
void AddRemoveComboBox::updateUi()
{
    mRemoveButton->setEnabled(mCombo->currentIndex() != -1);
}

/*! This slot takes an int as \e index parameter and emits the corresponding currentIndexChanged(QModelIndex). */
void AddRemoveComboBox::translateIntIndexChanged(int index)
{
    Q_EMIT currentIndexChanged(intIndexToQModelIndex(index));
}

QModelIndex AddRemoveComboBox::intIndexToQModelIndex(int intIndex) const
{
    if(!mCombo || !mCombo->model() || intIndex == -1)
        return QModelIndex();
    return QModelIndex(mCombo->model()->index(intIndex, mCombo->modelColumn()));
}

/*! Reimplemented event handler to handle state changes. Used for language change. */
void AddRemoveComboBox::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        mAddButton->setToolTip(tkTr(Trans::Constants::FILENEW_TEXT));
        mRemoveButton->setToolTip(tkTr(Trans::Constants::REMOVE_TEXT));
        break;
    default:
        break;
    }
}

/*! Sets the visible text of the actual item in the QComboBox. */
void AddRemoveComboBox::setEditText(const QString &text)
{
    mCombo->setEditText(text);
}


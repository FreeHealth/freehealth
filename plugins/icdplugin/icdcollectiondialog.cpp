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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
   \class ICD::IcdCollectionDialog
   \brief Creates a dialog with the ICD code selector and a Collection view.
   You can set and get the XML collection content.
*/

#include "icdcollectiondialog.h"
#include "icdcentralwidget.h"
#include "simpleicdmodel.h"
#include "fullicdcodemodel.h"
#include "icdassociation.h"

#include <utils/global.h>

#include <QGridLayout>
#include <QDialogButtonBox>

#include <QDebug>

using namespace ICD;

IcdCollectionDialog::IcdCollectionDialog(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout *lay = new QGridLayout(this);
    setLayout(lay);
    m_View = new IcdCentralWidget(this);
    lay->addWidget(m_View, 0, 0);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    lay->addWidget(buttonBox, 10, 0);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    Utils::resizeAndCenter(this, parent);
}

IcdCollectionDialog::~IcdCollectionDialog()
{
}

/** \brief Defines the ICD10 Collection Model to use in the dialog. */
void IcdCollectionDialog::setIcdCollectionModel(IcdCollectionModel *model)
{
    Q_ASSERT(m_View);
    if (!m_View)
        return;
    m_View->setIcdCollectionModel(model);
}

/** \brief Defines the XML encoded ICD10 Collection to use in the dialog. */
void IcdCollectionDialog::setXmlIcdCollection(const QString &xml)
{
    Q_ASSERT(m_View);
    if (!m_View)
        return;
    if (xml.isEmpty())
        m_View->clear();
    else
        m_View->readXmlCollection(xml);
}

/** \brief Defines the XML encoded ICD10 Collection to use in the dialog. */
QString IcdCollectionDialog::xmlIcdCollection()
{
    Q_ASSERT(m_View);
    if (!m_View)
        return QString();
    return m_View->collectionToXml();
}

void IcdCollectionDialog::done(int r)
{
    QDialog::done(r);
}

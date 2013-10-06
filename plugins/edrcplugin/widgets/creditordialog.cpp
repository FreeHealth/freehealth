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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class eDRC::Internal::CrEditorDialog
 */

#include "creditordialog.h"
#include <edrcplugin/constants.h>
#include <edrcplugin/consultresult.h>
#include <edrcplugin/widgets/sfmgaboutdialog.h>
#include <edrcplugin/widgets/crargumentsdialog.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <translationutils/constants.h>

#include <QPushButton>

#include <QDebug>

#include "ui_creditordialog.h"

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

using namespace eDRC;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace eDRC {
namespace Internal {
class CrEditorDialogPrivate
{
public:
    CrEditorDialogPrivate(CrEditorDialog *parent) :
        ui(new Ui::CrEditorDialog),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~CrEditorDialogPrivate()
    {
        delete ui;
    }
    
public:
    Ui::CrEditorDialog *ui;
    ConsultResult _cr;
    
private:
    CrEditorDialog *q;
};
} // namespace Internal
} // end namespace eDRC


/*! Constructor of the eDRC::Internal::CrEditorDialog class */
CrEditorDialog::CrEditorDialog(QWidget *parent) :
    QDialog(parent),
    d(new CrEditorDialogPrivate(this))
{
    d->ui->setupUi(this);

    // Create "About SMFG" button
    QPushButton *sfmg = d->ui->buttonBox->addButton(tr("About SFMG"), QDialogButtonBox::HelpRole);
    sfmg->setIcon(theme()->icon(Constants::ICON_SFMG_LOGO, Core::ITheme::SmallIcon));
    connect(sfmg, SIGNAL(clicked()), this, SLOT(onSmfgAboutClicked()));

    // Create "CR Arguments" button
    QPushButton *args = d->ui->buttonBox->addButton(tr("Consult result arguments"), QDialogButtonBox::HelpRole);
    args->setIcon(theme()->icon(Core::Constants::ICONINFORMATION, Core::ITheme::SmallIcon));
    connect(args, SIGNAL(clicked()), this, SLOT(onArgumentsClicked()));

    setWindowTitle(tr("Consult result editor and creator dialog"));
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEDRC));

    Utils::resizeAndCenter(this, parent);
}

/*! Destructor of the eDRC::Internal::CrEditorDialog class */
CrEditorDialog::~CrEditorDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool CrEditorDialog::initialize(const ConsultResult &cr)
{
    d->ui->widget->setConsultResult(cr);
    return true;
}

/** Submit the content of the editor to a eDRC::ConsultResult object and return it. */
ConsultResult CrEditorDialog::submit() const
{
    return d->ui->widget->submit();
}

/** Open the SFMG about dialog */
void CrEditorDialog::onSmfgAboutClicked()
{
    SfmgAboutDialog dlg(this);
    dlg.exec();
}

/** Open the argument dialog for the currently selected RC */
void CrEditorDialog::onArgumentsClicked()
{
    QPushButton *but = qobject_cast<QPushButton*>(sender());
    Q_ASSERT(but);
    if (but)
        but->setEnabled(false);
    int id = d->ui->widget->currentEditingConsultResultId();
    if (id == -1)
        return;
    CrArgumentsDialog dlg(this);
    dlg.setRcId(id);
    dlg.exec();
    if (but)
        but->setEnabled(true);
}


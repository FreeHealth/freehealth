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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "testhtmlcontent.h"
#include "ui_testhtmlcontent.h"

#include <translationutils/constants.h>

#include <QPushButton>

#include <QDebug>

using namespace Tests;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Tests {
namespace Internal {
class HtmlContentTesterPrivate
{
public:
    HtmlContentTesterPrivate(HtmlContentTester *parent) :
        ui(new Ui::HtmlContentTester),
        q(parent)
    {
    }

    ~HtmlContentTesterPrivate()
    {
        delete ui;
    }

public:
    Ui::HtmlContentTester *ui;

private:
    HtmlContentTester *q;
};
} // namespace Internal
} // end namespace Tests


/*! Constructor of the Tests::HtmlContentTester class */
HtmlContentTester::HtmlContentTester(QWidget *parent) :
    QDialog(parent),
    d(new HtmlContentTesterPrivate(this))
{
}

/*! Destructor of the Tests::HtmlContentTester class */
HtmlContentTester::~HtmlContentTester()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool HtmlContentTester::initialize()
{
    d->ui->setupUi(this);
    QPushButton *but = d->ui->buttonBox->addButton(tr("Refresh Browser"), QDialogButtonBox::ActionRole);
    connect(but, SIGNAL(clicked()), this, SLOT(refreshBrowserContent()), Qt::UniqueConnection);
    return true;
}

void HtmlContentTester::refreshBrowserContent()
{
    d->ui->textBrowser->setHtml(d->ui->plainTextEdit->toPlainText());
}

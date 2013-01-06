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
 * \class BaseWidgets::Internal::FrenchSocialNumberWidget
 * French social numbering helper widget.
 */

#include "frenchsocialnumberwidget.h"

#include <QLineEdit>
#include <QRegExpValidator>

#include <QDebug>

#include "ui_frenchsocialnumber.h"

using namespace BaseWidgets;
using namespace Internal;

enum { WarnKeyPressed = false };

namespace {

const QString controlKey1 = "154012A111185";  // clé 82
const QString controlKey2 = "1111111111111";  // clé 20
const QString controlKey3 = "1111111111034";  // clé 97
const QString controlKey4 = "278112b050002";  // clé 86
const QString controlKey5 = "1540250025005";  // clé 22
const QString controlKey6 = "299082B234349";  // clé 29

}

namespace BaseWidgets {
namespace Internal {
class FrenchSocialNumberWidgetPrivate
{
public:
    FrenchSocialNumberWidgetPrivate(FrenchSocialNumberWidget *parent) :
        ui(new Ui::FrenchSocialNumber),
        q(parent)
    {
    }
    
    ~FrenchSocialNumberWidgetPrivate()
    {
        delete ui;
    }
    
    void setupUi()
    {        
        ui->setupUi(q);
        ui->nss->setValidator(new QRegExpValidator(QRegExp("^[1-2]\\d{0,5}[0-9AB]\\d{0,6}$")));
        ui->nss->setInputMask("0 00 00 0N 000 000");
        ui->nss->setMaximumWidth(ui->nss->fontMetrics().width(QString().fill('9', 18)));
        //ui->nss->setMinimumWidth(ui->nss->fontMetrics().width(QString().fill('9', 18)));
        //ui->nss->setMinimumHeight(ui->nss->fontMetrics().height() + 10);

        ui->key->setReadOnly(true);
        ui->key->setMaximumWidth(ui->key->fontMetrics().width(QString().fill('9', 4)));
        //ui->key->setMinimumWidth(ui->key->fontMetrics().width(QString().fill('9', 2)));
        //ui->key->setMinimumHeight(ui->key->fontMetrics().height() + 10);
    }

    QString nss()
    {
        return ui->nss->text().remove(" ");
    }

public:
    Ui::FrenchSocialNumber *ui;

private:
    FrenchSocialNumberWidget *q;
};
} // namespace Internal
} // end namespace BaseWidgets


/*! Constructor of the BaseWidgets::Internal::FrenchSocialNumberWidget class */
FrenchSocialNumberWidget::FrenchSocialNumberWidget(QWidget *parent) :
    QWidget(parent),
    d(new FrenchSocialNumberWidgetPrivate(this))
{
    d->setupUi();
    connect(d->ui->nss, SIGNAL(textChanged(QString)), this, SLOT(checkControlKey()));
}

/*! Destructor of the BaseWidgets::Internal::FrenchSocialNumberWidget class */
FrenchSocialNumberWidget::~FrenchSocialNumberWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FrenchSocialNumberWidget::initialize()
{
    return true;
}

void FrenchSocialNumberWidget::setNumberWithControlKey(const QString &number)
{
    QString s = number;
    s = s.remove(" ");
    d->ui->nss->setText(s.left(13));
    checkControlKey();
}

void FrenchSocialNumberWidget::setNumberWithoutControlKey(const QString &number)
{
    QString s = number;
    s = s.remove(" ");
    d->ui->nss->setText(s.left(13));
    checkControlKey();
}

bool FrenchSocialNumberWidget::isValid() const
{
    return isValid(d->nss(), d->ui->key->text());
}

bool FrenchSocialNumberWidget::isValid(const QString &number, const QString &key) const
{
    // get control key
    int cKey = controlKey(number);
    if (cKey==-1) {
        return false;
    }
    return key==QString::number(cKey);
}

int FrenchSocialNumberWidget::controlKey(const QString &number) const
{
    if (number.size() != 13)
        return -1;

    QString nString = number;
    // manage departement 2A 2B
    nString.replace("A","0", Qt::CaseInsensitive);
    nString.replace("B","0", Qt::CaseInsensitive);
    QRegExp reg("\\d{13}");
    if (!reg.exactMatch(nString))
        return -1;

    qlonglong n = nString.toLongLong();

    // manage departement 2A 2B
    if (number.contains("A", Qt::CaseInsensitive))
        n -= 1000000;
    else if (number.contains("B", Qt::CaseInsensitive))
        n -= 2000000;
    return (97 - (n%97));
}

QString FrenchSocialNumberWidget::numberWithControlKey() const
{
    return QString(d->nss() + d->ui->key->text());
}

QString FrenchSocialNumberWidget::numberWithoutControlKey() const
{
    return d->nss();
}

QString FrenchSocialNumberWidget::emptyHtmlMask() const
{
    return QString("_&nbsp;__&nbsp;__&nbsp;__&nbsp;___&nbsp;___&nbsp;__");
}

QString FrenchSocialNumberWidget::toHtml() const
{
//    QStringList html;
//    for(int i = 0; i < d->m_Edits.count(); ++i) {
//        html << d->m_Edits.at(i)->text();
//    }
//    return html.join("&nbsp;");
    return numberWithControlKey().replace(" ", "&nbsp;");
}

/**
 * Calculate the control key and
 * - populate the control key line edit or
 * - check the control key
*/
void FrenchSocialNumberWidget::checkControlKey()
{
    int cKey = controlKey(d->nss());
    if (cKey==-1) {
        d->ui->key->setText("");
        return;
    }
    if (d->ui->key->text().isEmpty()) {
        d->ui->key->setText(QString::number(cKey));
    } else if (d->ui->key->text() != QString::number(cKey)) {
        d->ui->key->setText(QString::number(cKey));
    }
}

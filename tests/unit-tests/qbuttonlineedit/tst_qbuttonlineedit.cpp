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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <utils/global.h>
#include <utils/widgets/qbuttonlineedit.h>

#include <QVBoxLayout>
#include <QTest>
#include <QtGui>

const char * const ICONOK = ":/icons/ok.png";
const char * const ICONWARNING = ":/icons/warning.png";

class tst_QButtonLineEdit : public QObject
{
    Q_OBJECT
private slots:

    void testQButtonLineEdit()
    {
        QWidget *w = new QWidget;
        QVBoxLayout *l = new QVBoxLayout(w);
        w->setLayout(l);

        Utils::QButtonLineEdit *line1 = new Utils::QButtonLineEdit;
        line1->setRightIcon(QIcon(ICONOK));
        line1->setPlaceholderText("Line 1 (Right icon test: one icon set)");

        Utils::QButtonLineEdit *line2 = new Utils::QButtonLineEdit;
        line2->setRightIcon(QIcon(ICONOK));
        line2->setRightIcon(QIcon(ICONWARNING));
        line2->setPlaceholderText("Line 2 (Right icon test: replacing ok icon with warning icon)");

        Utils::QButtonLineEdit *line3 = new Utils::QButtonLineEdit;
        line3->setLeftIcon(QIcon(ICONOK));
        line3->setPlaceholderText("Line 3 (Left icon test: one icon set)");

        Utils::QButtonLineEdit *line4 = new Utils::QButtonLineEdit;
        line4->setLeftIcon(QIcon(ICONOK));
        line4->setLeftIcon(QIcon(ICONWARNING));
        line4->setPlaceholderText("Line 4 (Left icon test: replacing ok icon with warning icon)");

        Utils::QButtonLineEdit *line5 = new Utils::QButtonLineEdit;
        line5->setLeftIcon(QIcon(ICONOK));
        line5->setRightIcon(QIcon(ICONOK));
        line5->setPlaceholderText("Line 5 (both ok)");

        Utils::QButtonLineEdit *line6 = new Utils::QButtonLineEdit;
        line6->setLeftIcon(QIcon(ICONOK));
        line6->setLeftIcon(QIcon(ICONWARNING));
        line6->setRightIcon(QIcon(ICONOK));
        line6->setRightIcon(QIcon(ICONWARNING));
        line6->setPlaceholderText("Line 6 (both warning)");

        l->addWidget(line1);
        l->addWidget(line2);
        l->addWidget(line3);
        l->addWidget(line4);
        l->addWidget(line5);
        l->addWidget(line6);
        w->setFocus();
        w->show();
    }
};

QTEST_MAIN(tst_QButtonLineEdit)
#include "tst_qbuttonlineedit.moc"


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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

#include <utils/global.h>
#include <QDebug>
#include <QTest>

class tst_Html : public QObject
{
    Q_OBJECT
public:
    QString css, in, out;

private slots:
    void initTestCase()
    {
        css = "<style type=\"text/css\">"
              ".__ident__formContent {"
              "  font-size: 12pt;"
              "  color: black;"
              "  border: 2px solid gray;"
              "}"
              " "
              ".__ident__formContent .formHeader {"
              "}"
              "</style>";
    }

//    void cleanupTestCase()
//    {
//    }

//    /** Test body content extraction */
//    void htmlBodyExtractions()
//    {
//
//    }

    /** Test body content extraction */
    void extractCssFromBody()
    {
        in = QString("<html>"
                     "<body>\n"
                     "%1\n"
                     "<div class=\"__ident__formContent\">"
                     "<div class=\"formHeader\">"
                     "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                     "</div> <!-- formHeader -->"
                     "</body>"
                     "</html>").arg(css);

        out = QString("<html>"
                      "<body>\n"
                      "\n"
                      "<div class=\"__ident__formContent\">"
                      "<div class=\"formHeader\">"
                      "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                      "</div> <!-- formHeader -->"
                      "</body>"
                      "</html>");

        QCOMPARE(Utils::htmlTakeAllCssContent(in), css);
        QVERIFY(in == out);
    }

    void extractCssFromHeader()
    {
        in = QString("<html>"
                     "<header>"
                     "%1\n"
                     "</header>"
                     "<body>\n"
                     "<div class=\"__ident__formContent\">"
                     "<div class=\"formHeader\">"
                     "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                     "</div> <!-- formHeader -->"
                     "</body>"
                     "</html>")
                .arg(css);

        out = QString("<html>"
                      "<header>"
                      "\n"
                      "</header>"
                      "<body>\n"
                      "<div class=\"__ident__formContent\">"
                      "<div class=\"formHeader\">"
                      "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                      "</div> <!-- formHeader -->"
                      "</body>"
                      "</html>");

        QCOMPARE(Utils::htmlTakeAllCssContent(in), css);
        QVERIFY(in == out);
    }

    void extractDoubleCss()
    {
        QString doubleCss = css + css;
        in = QString("<html>"
                     "<header>"
                     "%1\n"
                     "</header>"
                     "<body>\n"
                     "%1\n"
                     "<div class=\"__ident__formContent\">"
                     "<div class=\"formHeader\">"
                     "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                     "</div> <!-- formHeader -->"
                     "</body>"
                     "</html>")
                .arg(css);

        out = QString("<html>"
                      "<header>"
                      "\n"
                      "</header>"
                      "<body>\n"
                      "\n"
                      "<div class=\"__ident__formContent\">"
                      "<div class=\"formHeader\">"
                      "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                      "</div> <!-- formHeader -->"
                      "</body>"
                      "</html>");

        QCOMPARE(Utils::htmlTakeAllCssContent(in), doubleCss);
        QVERIFY(in == out);
    }
};

QTEST_MAIN(tst_Html)
#include "tst_html.moc"

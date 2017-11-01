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

#include "../../autotest.h"

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

    /** Test body content extraction */
    void extractHtmlBody()
    {
        // simple extract
        in = "<body>Whoooa!</body>";
        QCOMPARE(Utils::htmlBodyContent(in), QString("<p>Whoooa!</p>"));


        // mode complicated
        out = "<div class=\"__ident__formContent\"> <!-- comment -->"
                "<div class=\"formHeader\">"
                "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                "</div> <!-- blah -->";

        const QString body =
                "<html><!-- comment -->"
                "<body%1>%2</body>"
                "<!-- another comment --></html>";

        in = body.arg("", out);

        QCOMPARE(Utils::htmlBodyContent(in), QString("<p>%1</p>").arg(out));


        // with style attribute
        in = body.arg(" style=\"background: white;\"", out);
        QCOMPARE(Utils::htmlBodyContent(in), QString("<p style=\"background: white;\">%1</p>").arg(out));
    }

    void extractMisformedBody()
    {
        in = "<body>Whoooa!";
        QCOMPARE(Utils::htmlBodyContent(in), QString("<p>Whoooa!</p>"));
    }

    /** Test body content extraction */
    void extractCssFromBody()
    {
        const QString html =
                "<html>"
                "<body>\n"
                "%1\n"
                "<div class=\"__ident__formContent\">"
                "<div class=\"formHeader\">"
                "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                "</div> <!-- formHeader -->"
                "</body>"
                "</html>";

        in = html.arg(css);
        out = html.arg("");

        QCOMPARE(Utils::htmlTakeAllCssContent(in), css);
        QVERIFY(in == out);
    }

    void extractCssFromHeader()
    {
        const QString html =
                "<html>"
                "<header>"
                "%1\n"
                "</header>"
                "<body>\n"
                "<div class=\"__ident__formContent\">"
                "<div class=\"formHeader\">"
                "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                "</div> <!-- formHeader -->"
                "</body>"
                "</html>";

        in = html.arg(css);
        out = html.arg("");

        QCOMPARE(Utils::htmlTakeAllCssContent(in), css);
        QVERIFY(in == out);
    }

    void extractDoubleCss()
    {
        QString doubleCss = css + css;
        const QString html =
                "<html>"
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
                "</html>";

        in = html.arg(css);
        out = html.arg("");

        QCOMPARE(Utils::htmlTakeAllCssContent(in), doubleCss);
        QVERIFY(in == out);
    }

    void removeLinkTags()
    {
        const QString html =
                "<html>"
                "<head>"
                "</head>"
                "<body>\n"
                "%1\n"
                "<div class=\"__ident__formContent\">"
                "<div class=\"formHeader\">"
                "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>"
                "</div> <!-- formHeader -->"
                "%2\n"
                "</body>"
                "</html>";

        in = html.arg("<a href=\"kjlkdjflqkjdflqksdfqdsfq\">").arg("</a>");
        out = html.arg("").arg("");
        QVERIFY(Utils::htmlRemoveLinkTags(in) == out);
    }

    void extractCssFileLinks()
    {
        const QString html =
                "<html>\n"
                "<head>\n"
                "<meta>\n"
                "<link rel=\"stylesheet\" href=\"../style/test1.css\" type=\"text/css\">\n"
                "<link rel=\"stylesheet\" href=\"../style/test2.css\" type=\"text/css\">\n"
                "</meta>\n"
                "</head>\n"
                "<body>\n"
                "<div class=\"__ident__formContent\">\n"
                "<div class=\"formHeader\">\n"
                "<div class=\"formLabel\">[[EpisodeFormLabel]]</div>\n"
                "</div> <!-- formHeader -->\n"
                "</div>\n"
                "</body>\n"
                "</html>\n";
        QStringList correct;
        correct << "../style/test1.css"
                << "../style/test2.css";
        QStringList result = Utils::htmlGetLinksToCssContent(html);
        QVERIFY(correct == result);
    }

    // TODO: code this tests
//    UTILS_EXPORT QString htmlReplaceAccents(const QString &html);
//    UTILS_EXPORT QString fontToHtml( const QFont &font, const QColor &color = QColor("black") );
//    UTILS_EXPORT QString textAlignmentToHtml( const Qt::Alignment &align );
//    UTILS_EXPORT QString firstLetterUpperCase(const QString &s);
//    UTILS_EXPORT QString removeAccents(const QString &text);
//    UTILS_EXPORT QString lineWrapString(const QString &in, int lineLength);
//    UTILS_EXPORT QString indentString(const QString &in, int lineIndent);

};

DECLARE_TEST(tst_Html)
#include "tst_html.moc"

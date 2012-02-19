/***************************************************************************
 *   FreeMedForms Project                                                  *
 *   (C) 2008-2010 by Eric MAEKER, MD                            *
 *   eric.maeker@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QApplication>

#include <QtGui>
#include <QDir>
#include <QDebug>

#include <QButtonLineEdit.h>
#include <tkGlobal.h>
#include <tkPrinter.h>
#include <tkSettings.h>
#include <tkTranslators.h>
#include <tkConstantTranslations.h>
#include <tkListView.h>
#include <tkRichTextEditor.h>
#include <tkActionManager.h>
#include <tkContextManager.h>

#include <tkPrinterTester.h>

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

QMainWindow *win;

void createMainWindow()
{
    win = new QMainWindow();
}

void test_QButtonLineEdit()
{
    createMainWindow();
    QWidget wgt;
    QVBoxLayout layout;
    wgt.setLayout( &layout );

    QButtonLineEdit l;

    // prepare button
    QToolButton but;
    QPixmap pixmap("../../pixmap/16x16/search.png");
    but.setIcon( pixmap );
    but.setIconSize( pixmap.size() );
    but.setToolTip( "Tool tip Button" );

    QAction act( qApp );
    act.setIcon( QPixmap( "../../pixmap/16x16/molecule.png" ) );
    act.setText( "Essai Text" );
    act.setToolTip( "Tool tip molecule action" );
    but.addAction( &act );

    QAction act2( qApp );
    act2.setIcon( QPixmap( "../../pixmap/16x16/pill.png" ) );
    act2.setText( "Commercial" );
    act2.setToolTip( "Tool tip Pills action" );
    but.addAction( &act2 );

    QToolButton butR;
    butR.setIcon( pixmap );
    butR.setIconSize( pixmap.size() );

    l.setLeftButton( &but );
    l.setRightButton( &butR );

    // debug visual show QPixmap
    layout.addWidget( &l );
    // seconf lineEdit for focus testing
    QLineEdit line( &wgt );
    layout.addWidget( &line );
    //      layout.addWidget( &but );

    // prepare central widget
    win->setCentralWidget( &wgt );
    win->show();
}

void text_Html()
{
    qWarning() << "align center" << tkGlobal::textAlignmentToHtml( Qt::AlignCenter );
    qWarning() << "align right" << tkGlobal::textAlignmentToHtml( Qt::AlignRight );
    qWarning() << "align left" << tkGlobal::textAlignmentToHtml( Qt::AlignLeft );
    qWarning() << "align just" << tkGlobal::textAlignmentToHtml( Qt::AlignJustify );
    qWarning() << "align hcenter" << tkGlobal::textAlignmentToHtml( Qt::AlignHCenter );
    qWarning() << "system font" << tkGlobal::fontToHtml( QFont(), "black" );
}

void test_Xml()
{
    QHash<QString,QString> d;
    d.insert( "K_1", "Value1");
    d.insert( "K_2", "Value2");
    d.insert( "K_3", "Value3");
    d.insert( "K_4", "Value4");
    QString xml = tkGlobal::createXml( "DIPRESCRIPTION", d , 4 , true );
    d.clear();
    qWarning() << xml << "correctly read XML ?" << tkGlobal::readXml( xml ,"DIPRESCRIPTION", d , true );
    qWarning() << d;

    xml = tkGlobal::createXml( "DIPRESCRIPTION", d , 4 , false );
    d.clear();
    qWarning() << xml << tkGlobal::readXml( xml ,"DIPRESCRIPTION", d , false );
    qWarning() << d;
}

void test_tkPrinter()
{
    tkPrinterTester t(win);
}

void testTokenReplacement()
{
    QString text = "This is [before text [TOKEN] after text] replacement.";
    qWarning() << tkGlobal::replaceToken( text, "TOKEN", "");
    qWarning() << text;
}

void test_Translations()
{
    tkTranslators *t = tkTranslators::instance();
    t->setPathToTranslations( tkSettings::instance()->path( tkSettings::TranslationsPath ) );
    t->addNewTranslator( "toolkit" );
    t->addNewTranslator( "qt" );
    qWarning() << tkTr(INTAKES, 2);
    qWarning() << tkTr(INTAKES, 1);
    qWarning() << tkTr(INTAKES, 0);
}

void test_dialogs()
{
    qWarning() << tkGlobal::defaultLicenceAgreementDialog("Smouky touky", tkAboutDialog::BSD);
}

void test_listView()
{
    win = new QMainWindow();
    QWidget *w = new QWidget(win);
    tkActionManager::instance(win);
    tkContextManager::instance(win);
    QStringListModel *m1 = new QStringListModel(win);
    QStringListModel *m2 = new QStringListModel(win);
    QStringListModel *m3 = new QStringListModel(win);
    tkListView *v1 = new tkListView(w);
    tkListView *v2 = new tkListView(w);
    tkListView *v3 = new tkListView(w);
    tkRichTextEditor *te = new tkRichTextEditor(w);
    tkRichTextEditor *te2 = new tkRichTextEditor(w);

    v1->setModel(m1);
    v2->setModel(m2);
    v3->setModel(m3);

    QGridLayout *l = new QGridLayout(w);

    l->addWidget(v1);
    l->addWidget(v2);
    l->addWidget(v3);
    l->addWidget(te);
    l->addWidget(te2);

    win->setCentralWidget(w);
    win->show();

}

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(toolkittester);
    QApplication app(argc, argv);
    app.setApplicationName("toolkittester");
    win=0;
    tkSettings *s = tkSettings::instance();
    s->setPath(tkSettings::BundleResourcesPath, qApp->applicationDirPath()+ "/../../../../global_resources/");

//    test_QButtonLineEdit();
//    text_Html();
//    test_Xml();
//    test_Translations();
//    test_tkPrinter();
    test_listView();
//    test_dialogs();
    app.exec();
    return 0;
}


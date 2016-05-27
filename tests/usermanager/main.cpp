/***************************************************************************
 *   FreeMedForms Project                                                  *
 *   (C) 2008-2016 by Eric MAEKER, MD                            *
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

#include <tkUserBase.h>
#include <tkUserModel.h>
#include <tkUser.h>
#include <tkUserIdentifier.h>
#include <tkUserViewer.h>
#include <tkUserManager.h>
#include <tkUserGlobal.h>
#include <tkUserWizard.h>

#include <tkGlobal.h>
#include <tkSettings.h>
#include <tkLog.h>
#include <tkTranslators.h>
#include <tkDebugDialog.h>
#include <tkDatabase.h>
#include <tkRichTextEditor.h>
#include <tkStringListModel.h>
#include <tkStringListView.h>
#include <tkListView.h>
#include <tkActionManager.h>
#include <tkTheme.h>
#include <tkTextDocumentExtra.h>

#include <QtGui>
#include <QMultiHash>
//#include <QtGlobal>

#include "testClass.h"

/**
  \brief This project is only a tester of the FreeMedForms' usertoolkit lib
  \author Eric MAEKER, MD <eric.maeker@gmail.com>
*/

enum
{
    F1 = 0,
    F2,F3,F4,F5
};

enum
{
    T1=0, T2
};


tkUser *user = 0;


void testDatabaseScheme()
{
    tkDatabase s;
    s.addTable( T1, "poillus" );
    s.addTable( T2, "feuillus" );
    s.addField( T1, F1, "poils1", tkDatabase::FieldIsUUID );
    s.addField( T1, F2, "poils2", tkDatabase::FieldIsDate );
    s.addField( T1, F3, "poils3", tkDatabase::FieldIsShortText );
    s.addField( T1, F4, "poils4", tkDatabase::FieldIsBoolean );
    s.addField( T1, F5, "poils5", tkDatabase::FieldIsBlob );
    s.addField( T2, 1, "feuille2", tkDatabase::FieldIsInteger );
    s.addField( T2, 2, "feuilleid2", tkDatabase::FieldIsInteger );


    qWarning() << s.tables() << s.fields(T1) << s.fields(T2);
    QHash<int, QString> where;
    where.insert( F1, "='poils1'");
    where.insert( F2, "='poils2'");
    where.insert( F4, "='poils4'");
    qWarning() << s.getWhereClause( T1, where);
    qWarning() << s.select( T1, F2, where);
    qWarning() << s.select( T2, 1, where);

    qWarning() << s.createTable( T1 );
    qWarning() << s.createTables();
    s.warn();
}

void testUserIdentification( tkSettings & settings )
{
    // identification
    tkUserIdentifier tki;
    if ( tki.exec() == QDialog::Rejected )
        return;
}

void testUserBase( tkSettings *settings)
{
//    tkUserBase * tkub = tkUserBase::instance();
//    tkub->initialize( settings );
//    tkub->recordLastLogin( DEFAULT_USER_LOGIN, DEFAULT_USER_PASSWORD );
//    user = tkub->getUserByLoginPassword( DEFAULT_USER_LOGIN, DEFAULT_USER_PASSWORD );
//    if ( ! user )
//        return 0;
}

void testUserWizard()
{
    tkUserWizard wiz;
    wiz.createUser();
    wiz.show();
}

void testListViewWithUsers()
{
    // test listview
    QMainWindow mainWin;
    tkUserModel * m = tkUserModel::instance();
    m->setCurrentUser( DEFAULT_USER_LOGIN, DEFAULT_USER_PASSWORD );
    QStringListModel *model = new QStringListModel(qApp);
    model->setStringList( QStringList() << "1" << "2" << "3" << "4" );

    tkStringListView tklv(&mainWin );
    tklv.setButtons( tkListView::AllButtons );
    tklv.setActions( tkListView::AllActions );
    tklv.setModel( model );
    tklv.setEditTriggers(QAbstractItemView::AllEditTriggers);
    mainWin.setCentralWidget( &tklv );
    mainWin.show();
}

void testTextDocExtra()
{
    const char* TT = "blablablablablablba";
    tkTextDocumentExtra tk(TT);
    QVariant q;
    q.setValue(tk);
    qWarning() << tk.toHtml();
    qWarning() << tk.toXml();
    qWarning() << q.value<tkTextDocumentExtra>().toHtml();
}

void testUserDynamicDatas()
{
    QVariant q(QDateTime::currentDateTime());
    qWarning() << q.toString() << q.type();
    q = "2009-06-22T06:29:299";
    qWarning() << q.toString() << q.type() << q.toDateTime();

    QHash<int, QVariant> fakeDb;
    const char* TT = "<html><body><p>blablablablablablba</p></body></html>";
    tkTextDocumentExtra tk(TT);
    fakeDb.insert(DATAS_ID,         1 );
    fakeDb.insert(DATAS_USER_UUID,  QUuid::createUuid().toString() );
    fakeDb.insert(DATAS_DATANAME,   "papers.generic.header" );
    fakeDb.insert(DATAS_STRING,     "" );
    fakeDb.insert(DATAS_LONGSTRING, tk.toXml() );
    fakeDb.insert(DATAS_FILE,       QVariant() );
    fakeDb.insert(DATAS_NUMERIC,    QVariant() );
    fakeDb.insert(DATAS_DATE,       QVariant() );
    fakeDb.insert(DATAS_LANGUAGE,   "fr" );
    fakeDb.insert(DATAS_LASTCHANGE, QVariant() );
    fakeDb.insert(DATAS_TRACE_ID,   QVariant() );
    UserDynamicData udd;
    foreach( int k, fakeDb.keys())
        udd.feedFromSql(k, fakeDb.value(k));
    udd.warn();
    qWarning() << "WARNING UDD.VALUE" << udd.value();

//    fakeDb.insert(DATAS_ID,          );
//    fakeDb.insert(DATAS_USER_UUID,   );
//    fakeDb.insert(DATAS_DATANAME,    );
//    fakeDb.insert(DATAS_STRING,      );
//    fakeDb.insert(DATAS_LONGSTRING,  );
//    fakeDb.insert(DATAS_FILE,        );
//    fakeDb.insert(DATAS_NUMERIC,     );
//    fakeDb.insert(DATAS_DATE,        );
//    fakeDb.insert(DATAS_LANGUAGE,    );
//    fakeDb.insert(DATAS_LASTCHANGE,  );
//    fakeDb.insert(DATAS_TRACE_ID,    );
//
//    fakeDb.insert(DATAS_ID,          );
//    fakeDb.insert(DATAS_USER_UUID,   );
//    fakeDb.insert(DATAS_DATANAME,    );
//    fakeDb.insert(DATAS_STRING,      );
//    fakeDb.insert(DATAS_LONGSTRING,  );
//    fakeDb.insert(DATAS_FILE,        );
//    fakeDb.insert(DATAS_NUMERIC,     );
//    fakeDb.insert(DATAS_DATE,        );
//    fakeDb.insert(DATAS_LANGUAGE,    );
//    fakeDb.insert(DATAS_LASTCHANGE,  );
//    fakeDb.insert(DATAS_TRACE_ID,    );
}

int main(int argc, char *argv[])
{
//    testTextDocExtra();
//    return 0;

//    testUserDynamicDatas();
//    return 0;

    QApplication app(argc, argv);
    app.setApplicationName( "tkUserTester" );
    app.setApplicationVersion( "0.0.1" );
//    tkLog::muteConsoleWarnings();

    // init settings
    tkSettings settings;
    // add translators (if not there will be a segfault)
    tkTranslators *trans = tkTranslators::instance(qApp);
    trans->setPathToTranslations( settings.path( tkSettings::TranslationsPath ) );
    tkUserGlobal::initLib();
    tkGlobal::initLib();
    trans->addNewTranslator( "qt" );

    tkUserModel * m = tkUserModel::instance();
    m->setCurrentUser( DEFAULT_USER_LOGIN, DEFAULT_USER_PASSWORD );

    // user manager
    tkUserManager *tkm = new tkUserManager();
    tkm->show();

    return app.exec();
}





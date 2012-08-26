/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "mythread.h"
#include "ledgerIO.h"
#include <QDebug>


enum { WarnDebugMessage = false };
ProduceDoc::ProduceDoc()
{
    m_fieldsToJoin     = 0;
    m_tableFormatParameters = "200,200,150,250,170";//the last is not used in fact
    m_tablesRecapParameters = "400,400";
}
    
ProduceDoc::~ProduceDoc(){}

void ProduceDoc::run(){
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " RUN !!! "  ;
    m_mutex.lock();
    m_textDocument = new QTextDocument;  
    organize();
    m_mutex.unlock();
}

void ProduceDoc::organize(){
    QString parametersForTableFormat = m_tableFormatParameters;
    QTextTableFormat tableFormat;
    myFormat(tableFormat,parametersForTableFormat);
    int nombrejoursannee              = m_date.daysInYear();
    QTextCursor   *cursortrie         = new QTextCursor(m_textDocument);
    
//-----------------------insertion du titre du document-----------------------------------

    QString titreDoc                   = "<font size = 10 color = #3300FF><bold>"+trUtf8("Ledger")+"<bold></font>"
                                         "<br/><br/><br/>";
    //QFont   titreFont                  ("Times", 40, QFont::Bold);
    //QTextCharFormat titreFormat ;
    //titreFormat                       . setFont(titreFont);
    QTextBlockFormat centrer ;
       //centrer                       .setBackground(Qt::yellow) ;
       centrer                        .setForeground(Qt::red) ;
       centrer                        .setAlignment(Qt::AlignCenter) ;
       centrer                        .setLeftMargin(24.0) ;
       centrer                        .setRightMargin(32.0) ;
       centrer                        .setTopMargin(16.0) ;
       centrer                        .setBottomMargin(24.0) ;
   
    cursortrie                        ->insertBlock(centrer);
    //cursortrie                        ->insertFragment(titredoc);
    cursortrie                        ->insertHtml(titreDoc);
    cursortrie                        ->movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,1);
//janvier
    QString unjanv                    = m_date.toString("yyyy")+"-01-01";
    QString trenteetunjanv            = m_date.toString("yyyy")+"-01-31";
    QString janvier                   = trUtf8("january");
    QString janvList;
    janvList                          = unjanv +","+ trenteetunjanv;
    

//fevrier------------mois particulier !
    QString unfev                     = m_date.toString("yyyy")+"-02-01";
    QString vingthuitfev              = m_date.toString("yyyy")+"-02-28";
        if(nombrejoursannee == 366){
            vingthuitfev  = m_date.toString("yyyy")+"-02-29";
            }
    QString fevrier                   =  trUtf8("february");
    QString fevList;
    fevList                           = unfev +","+ vingthuitfev;
//mars
    	QString unmars                = m_date.toString("yyyy")+"-03-01";
	QString trenteetunmars        = m_date.toString("yyyy")+"-03-31";
	QString mars = trUtf8("march");
	QString marsList;
	marsList                      = unmars +","+ trenteetunmars;
 //avril
    	QString unavril               = m_date.toString("yyyy")+"-04-01";
	QString trenteavril           = m_date.toString("yyyy")+"-04-30";
	QString avril                 = trUtf8("april");
	QString avrList;
	avrList                       = unavril +","+ trenteavril;
 //mai
    	QString unmai                 = m_date.toString("yyyy")+"-05-01";
	QString trenteetunmai         = m_date.toString("yyyy")+"-05-31";
	QString mai                   = trUtf8("may");
	QString maiList;
	maiList                       = unmai +","+ trenteetunmai;
 //juin
    	QString unjuin                = m_date.toString("yyyy")+"-06-01";
	QString trentejuin            = m_date.toString("yyyy")+"-06-30";
	QString juin                  = trUtf8("june");
	QString juinList;
	juinList                     = unjuin +","+ trentejuin;
 //juillet
     	QString unjuillet            = m_date.toString("yyyy")+"-07-01";
	QString trenteetunjuillet    = m_date.toString("yyyy")+"-07-31";
	QString juillet              = trUtf8("july");
	QString juillList;
	juillList                    = unjuillet +","+ trenteetunjuillet;
 //aout
    	QString unaout               = m_date.toString("yyyy")+"-08-01";
	QString trenteetunaout       = m_date.toString("yyyy")+"-08-31";
	QString aout                 = trUtf8("august");
	QString aouList;
	aouList                      = unaout +","+ trenteetunaout;
 //septembre
    	QString unsept               = m_date.toString("yyyy")+"-09-01";
	QString trentesept           = m_date.toString("yyyy")+"-09-30";
	QString sept                 = trUtf8("september");
	QString sepList;
	sepList                      = unsept +","+ trentesept;
 //octobre
    	QString unoct                = m_date.toString("yyyy")+"-10-01";
	QString trenteetunoct        = m_date.toString("yyyy")+"-10-31";
	QString oct                  = trUtf8("october");
	QString octList;
	octList                      = unoct +","+ trenteetunoct;
 //novembre
    	QString unnov                = m_date.toString("yyyy")+"-11-01";
	QString trentenov            = m_date.toString("yyyy")+"-11-30";
	QString nov                  = trUtf8("november");
	QString novList;
	novList                      = unnov +","+ trentenov;
 //decembre
    	QString undec                = m_date.toString("yyyy")+"-12-01";
	QString trenteetundec        = m_date.toString("yyyy")+"-12-31";
	QString dec                  = trUtf8("december");
	//modele(dec, undec , trenteetundec ,tableFormat,cursortrie,m_listforquery);
	//cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
	QString decList;
	decList                      = undec +","+ trenteetundec;
	
//annee
        QString annee                = trUtf8("complete year");
	QString anneeList;	             
	anneeList                    = unjanv+","+ trenteetundec;
//--------------construction de listes des types----------------------------------------------------------
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " thread 1 "   ;
    LedgerIO lio(0);
    m_typesReceipts = lio.listOfReceiptsTypes();
    m_typesReceipts.removeDuplicates();
    m_typesMovements = lio.listOfMovementsTypes();
    m_typesMovements.removeDuplicates();
    if (WarnDebugMessage)
      	      qDebug() << __FILE__ << QString::number(__LINE__) << " thread 2 "   ;
//------------boucles mois ---------------------------------------------------------------------
    QStringList listeMois;
        listeMois << janvier << fevrier << mars << avril << mai << juin 
                  << juillet << aout << sept << oct << nov << dec << annee ;
    QStringList datesMois;
        datesMois << janvList << fevList << marsList << avrList << maiList << juinList << juillList
                  << aouList << sepList << octList << novList << decList << anneeList;
    for (int i = 0 ; i < listeMois.size() ; i++)
    {
        QString mois      = listeMois[i];
        QString dates     = datesMois[i];
        QStringList list  = dates.split(",");
        QString debutMois = list[0];
        QString finMois   = list[1];
        
        modele(listeMois[i], debutMois , finMois ,tableFormat,cursortrie);
        cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    }
}//end of organize()
//----------fonction de construction du tableau type -------------------------------------------
QTextTableFormat ProduceDoc::myFormat(QTextTableFormat & tableFormat,QString & parametersForTableFormat){
     QTextTableFormat table         = tableFormat;
     QStringList parametersList     = parametersForTableFormat.split(",");
     if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " parametersList.size =" 
              << QString::number(parametersList.size()) ;
     tableFormat                     .setBackground(QColor("#C0C0C0"));
     tableFormat                     .setAlignment(Qt::AlignCenter);
     tableFormat                     .setCellPadding(2);
     tableFormat                     .setCellSpacing(2);
     QVector<QTextLength> constraints;
         for(int i = 0;i < parametersList.size() ; i++){
             if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " parametersForTableFormat =" << parametersList[i] ;
             constraints << QTextLength(QTextLength::FixedLength, parametersList[i].toInt());
             }

     tableFormat                     .setColumnWidthConstraints(constraints);
     return table;
}


//-------------fonction de calcul des colonnes recettes---------------------------
QStringList ProduceDoc::calculateReceipts( QString & dateBegin, QString & dateEnd){
    QString date1                    = dateBegin;
    QString date2                    = dateEnd;
    LedgerIO lio(0);
    QStringList listSums;
    listSums = lio.getListOfSumsMonthlyReceiptsIO(0,date1,date2);
    return listSums;
}
//----------------fonction de calcul des depenses--------------------------------------------------
QStringList ProduceDoc::calculateMovements(QString & dateBegin, QString & dateEnd){
    QString beginningOfMonth    = dateBegin;
    QString endOfMonth      = dateEnd;
    LedgerIO lio(0);
    QStringList typeAndSumsList;
    typeAndSumsList = lio.getListOfSumsMonthlyMovementsIO(0,beginningOfMonth , endOfMonth);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " typeAndSumsList.size() =" << QString::number(typeAndSumsList.size()) ;
    for (int i = 0; i < typeAndSumsList.size(); i += 1)
    {
    	if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "typeAndSumsList[i]  =" << typeAndSumsList[i] ;
        }
    
    return typeAndSumsList;
}
//--------------------fonction de remplissage de tableau par mois-------------------------------------------
bool ProduceDoc::modele(QString & month,
                        QString & day1 , //"yyyy-MM-dd"
                        QString & lastDay ,//"yyyy-MM-dd"
                        QTextTableFormat & tableFormatOrganized,
                        QTextCursor *cursorForFunction ){
    QString thisMonth                    = month;
    QString un                           = day1;
    QString trenteetquelque              = lastDay;
    QTextTableFormat tableFormatModele   = tableFormatOrganized;
    QTextCursor *cursorForFillFunction   = cursorForFunction;
    
    LedgerIO lio(0);
    QList<QVector<QString> > tableLedgerMonth;
    if (WarnDebugMessage)
    
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " thread 3 "   ;
    tableLedgerMonth = lio.getDatasReceiptsInVector(un,trenteetquelque);  
    if (WarnDebugMessage)
    
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " thread 4 "   ;
//---------------itération recettes par type-----------------------------------------------------------------
    QList<QVector<QString> > tableLedgerTypeReceipts;
        for(int i = 0; i <  m_typesReceipts.size(); i++){
            QString typeRecettes = m_typesReceipts[i];
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " m_typesReceipts[i] =" << m_typesReceipts[i] ;
            QStringList listeLibelle;
            listeLibelle << trUtf8("Cash") << trUtf8("Cheques") << trUtf8("Credit cards") << trUtf8("Bankings");
            for (int j = 0 ; j < listeLibelle.size() ; j++){
               QVector<QString> vector;
               foreach(vector,tableLedgerMonth){
                  if(vector[ACTS]== typeRecettes && ((vector[j+PLACE_OF_CASH] == QString::number(0)) == false))
                  {
                      QVector<QString> vectorLibelle;
                      emit recupSlot(thisMonth+": "+vector[DATE]);//renseigne le QLabel label_5
                      vectorLibelle           << vector[DATE] 
                                              << vector[PATIENT_NAME] 
                                              << vector[j+PLACE_OF_CASH] 
                                              << vector[ACTS] ;
                      tableLedgerTypeReceipts << vectorLibelle;
                  }
                }
            }
       }

    QStringList sumsReceipts;
                sumsReceipts   = calculateReceipts( un, trenteetquelque);
    if (WarnDebugMessage)    
    	      qDebug() << "tableLedgerTypeReceipts.size()= "+ QString::number(tableLedgerTypeReceipts.size()) << __LINE__;
    QString emptyStringForReceipts;
    fillTable(tableLedgerTypeReceipts,tableFormatModele,cursorForFillFunction,thisMonth,sumsReceipts,RECEIPTS_TYPE,emptyStringForReceipts);
//----------------tableau depenses---------------------------------------------------------
    QList<QVector<QString> > tableLedgerMonthDepenses;
    tableLedgerMonthDepenses = lio.getDatasMovementsInVector(un,trenteetquelque);

//----------------iteration des depenses par type------------------------------------------------------
    QList<QVector<QString> > tableauLivreTypeDepense;
    QStringList listOfTypeMovements;
    listOfTypeMovements = m_typesMovements;
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "m_typesMovements.size()  =" 
    	      <<  QString::number(m_typesMovements.size());
    for(int i = 0 ; i < listOfTypeMovements.size() ; i++){
            QVector<QString> vector;
            foreach(vector,tableLedgerMonthDepenses){
                 if(vector[VM_LABEL] == listOfTypeMovements[i]){
                     if (WarnDebugMessage)    
    	                 qDebug() << __FILE__ << QString::number(__LINE__) 
    	                 << "vector[VM_LABEL]  =" << vector[VM_LABEL] ;
                     tableauLivreTypeDepense << vector;
                     }
            }
        }
    QStringList sommedepenses;
                sommedepenses = calculateMovements( un, trenteetquelque);
    QString totalOfMovements;
    if (sommedepenses.size()>0)
    {
    	  totalOfMovements = sommedepenses.last().split("=")[1];
    	  
    	  //sommedepenses.removeAt(0);
        }
    fillTable(tableauLivreTypeDepense,tableFormatModele,cursorForFillFunction,thisMonth,sommedepenses,MOVEMENTS_TYPE,totalOfMovements);
    return true;
}
///////////////////////////////////////////////////////////////////////////////////
//fin modele-----------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////
void ProduceDoc::fillTable(QList<QVector<QString> > & tableau,
                           QTextTableFormat & tableFormatOrganized,
                           QTextCursor * cursorForFillFunction,
                           QString & thisMonth, 
                           QStringList & listSums, 
                           int choice,
                           const QString & totalMovementString){
    QList<QVector<QString> > tableauInFonction;
                         tableauInFonction         = tableau;
        int              nbreLignesTableau         = tableauInFonction.size();
        int              nbreColonnesTableau       = TABLE_NAME_OF_ACTS;
        int              sizeOfTable               = nbreLignesTableau*nbreColonnesTableau;
        QTextTableFormat tableFormat               = tableFormatOrganized;
        QTextCursor     *cursortrieinfunction      = cursorForFillFunction;
        QString          thisMonthfonction         = thisMonth;
        QString          type                      = "";
        QStringList      totalSumsList             = listSums;
        QString total;
        /*for (int i = 0; i < totalSumsList.size(); i += 1)
        {
        	if (WarnDebugMessage)
    
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " totalSumsList =" << totalSumsList[i] ;
            }*/
        if(choice == RECEIPTS_TYPE){
            type = trUtf8("Receipts");
            total = totalSumsList[SUMS_SUM];
                        }
        if(choice == MOVEMENTS_TYPE){
            type = trUtf8("Movements");
            total = totalMovementString;
            }
        QTextBlockFormat centerHead ;
       //centrer                       .setBackground(Qt::yellow) ;
       if (WarnDebugMessage)    
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " thisMonthfonction =" << thisMonthfonction ;
           QString heads = trUtf8("Month of ")+thisMonthfonction+" = "+type;
       if (thisMonthfonction == trUtf8("complete year"))
       {
       	   heads = trUtf8("Total of ")+thisMonthfonction+" = "+type;
           }
           centerHead                    .setAlignment(Qt::AlignCenter) ;
           cursortrieinfunction         -> insertBlock(centerHead);
           cursortrieinfunction         -> insertHtml("<font size = 6 color = #3300FF><bold><br/>"
                                                      "<br/>"+heads+"<bold>"
                                                      "</font><br/><br/>");
        QTextTableFormat tableFormatDone;   
        myFormat(tableFormatDone,m_tablesRecapParameters);
        if (WarnDebugMessage)
    
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " thread 10 "   ;
        if(sizeOfTable!= 0){
            if((thisMonthfonction != trUtf8("complete year")) ){
                QTextTable * table = cursortrieinfunction->insertTable(nbreLignesTableau,
                                                                       nbreColonnesTableau,
                                                                       tableFormat);
            for(int i=0 ; i< nbreLignesTableau ; i++){
                QVector<QString> vectorString;
                    vectorString = tableauInFonction[i];
                /*if (WarnDebugMessage)
    
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "vectorString size  =" 
                                     <<  QString::number(vectorString.size());*/
                QStringList list; // liste des données de la ligne
                /*if (WarnDebugMessage)
    
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "nbreColonnesTableau  = " 
                         << QString::number(nbreColonnesTableau)  ;*/
    	        for (int a = 0 ;a < nbreColonnesTableau ; a++){
                    QString str = vectorString[a];
                         list << str;
                         if (WarnDebugMessage)
    	                 qDebug() << __FILE__ << QString::number(__LINE__) << " str =" << str ;
                   }
                   double s = list[2].toDouble();
                   if(s > 0){
                      for(int j= 0 ; j < nbreColonnesTableau ; j++){
                          QTextTableCell cell    = table->cellAt(i,j);
                          QTextCursor cellCursor = cell.firstCursorPosition();
                          cellCursor             . insertText(list[j]);
                      }
                   }
            }
    QTextBlockFormat centrer ;
       //centrer                       .setBackground(Qt::yellow) ;
       centrer                        .setAlignment(Qt::AlignCenter) ;
       cursortrieinfunction         -> insertBlock(centrer);
       cursortrieinfunction         -> insertText("\n\n");
//----------------insertion fin de table-------------------------------------------
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "total  =" << total ;
    table                           -> insertRows(table->rows(),1);
    table                           -> mergeCells(table->rows()-1,0,1,2);//-1 car part de zero
    QTextTableCell cell              = table->cellAt(table->rows()-1,0);
     QTextCursor cellCursor          = cell.firstCursorPosition();
     QString totalMonth = QString("<html><font size = 4 color = #FF0000><bold>%1 %2 <bold></font></html>")
                          .arg(trUtf8("Total of "),thisMonthfonction);
     cellCursor                       .insertHtml(totalMonth);
    QTextTableCell cell2             = table->cellAt(table->rows()-1,2);
     QTextCursor cellCursor2         = cell2.firstCursorPosition();
     table                          -> mergeCells(table->rows()-1,2,1,3);
     cellCursor2                      .insertText(total);
    cursortrieinfunction            -> movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    
    QTextBlockFormat centrer1 ;
       //centrer1                       .setBackground(Qt::yellow) ;
       centrer1                        .setForeground(Qt::red) ;
       centrer1                        .setAlignment(Qt::AlignCenter);
       QString headAccumulation = trUtf8("Accumulation of ")+type+" "+trUtf8("of")+" "+thisMonthfonction;
       cursortrieinfunction          -> insertBlock(centrer1);
       cursortrieinfunction          -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                                    "<br/>"+headAccumulation+"<bold></font>"
                                                    "<br/><br/>");       
    }
    if (WarnDebugMessage)
    
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " thread 12 "   ;
//---------------insertion table recapitulative----------------------------------
//---------------complete year---------------------------------------------------
    QTextTable *tableRecap;
    if(choice == RECEIPTS_TYPE){
        QString esp               = totalSumsList[SUMS_CASH];
        QString chq               = totalSumsList[SUMS_CHECKS];
        QString cb                = totalSumsList[SUMS_CREDITCARDS];
        QString banking           = totalSumsList[SUMS_BANKING];
        QString totalReceipts     = totalSumsList[SUMS_SUM];
        nbreLignesTableau         = int(SUMS_MaxParam) ;
        tableRecap                = cursortrieinfunction->insertTable(nbreLignesTableau,2,tableFormatDone);
        QTextTableCell cell00     = tableRecap->cellAt(0,0);//verify all table
         QTextCursor cellCursor00 = cell00.firstCursorPosition();
         cellCursor00             . insertText(trUtf8("Total Cash"));
        QTextTableCell cell01     = tableRecap->cellAt(0,1);
         QTextCursor cellCursor01 = cell01.firstCursorPosition();
         cellCursor01             . insertText(esp);
        QTextTableCell cell10     = tableRecap->cellAt(1,0);
         QTextCursor cellCursor10 = cell10.firstCursorPosition();
         cellCursor10             . insertText(trUtf8("Total checks"));
        QTextTableCell cell11     = tableRecap->cellAt(1,1);
         QTextCursor cellCursor11 = cell11.firstCursorPosition();
         cellCursor11             . insertText(chq);
        QTextTableCell cell20     = tableRecap->cellAt(2,0);
         QTextCursor cellCursor20 = cell20.firstCursorPosition();
         cellCursor20             . insertText(trUtf8("Total credit cards"));
        QTextTableCell cell21     = tableRecap->cellAt(2,1);
         QTextCursor cellCursor21 = cell21.firstCursorPosition();
         cellCursor21             . insertText(cb);
        QTextTableCell cell30     = tableRecap->cellAt(3,0);
         QTextCursor cellCursor30 = cell30.firstCursorPosition();
         cellCursor30             . insertText(trUtf8("Total bankings"));
        QTextTableCell cell31     = tableRecap->cellAt(3,1);
         QTextCursor cellCursor31 = cell31.firstCursorPosition();
         cellCursor31             . insertText(banking);
            QTextTableCell cell40     = tableRecap->cellAt(4,0);
            QTextCursor cellCursor40  = cell40.firstCursorPosition();
            QString totalReceiptsHtml = QString("<html><font size = 4 color = #FF0000><bold>%1 %2 <bold></font></html>")
                                        .arg(trUtf8("Total of "),trUtf8("receipts"));
            cellCursor40              . insertHtml(totalReceiptsHtml);
            QTextTableCell cell41     = tableRecap->cellAt(4,1);
            QTextCursor cellCursor41  = cell41.firstCursorPosition();
            cellCursor41              . insertText(totalReceipts);  
            }
    if(choice == MOVEMENTS_TYPE){
        //nbreLignesTableau = m_typesMovements.size();
        if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_typesMovements.size() =" << QString::number(m_typesMovements.size())  ;
        //int nberLines = nbreLignesTableau +1 ;
        int nberLines = totalSumsList.size();
        if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "nberLines  =" <<  QString::number(nberLines);
        tableRecap                = cursortrieinfunction->insertTable(nberLines,2,tableFormatDone);
        if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "totalSumsList.size = " << QString::number(totalSumsList.size());
        for(int i = 0 ; i < nberLines ; i++){
            if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "i = " << QString::number(i);
    	    //if(!i < totalSumsList.size()){break;}
            if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "totalSumsList[i] = " << totalSumsList[i];
            QStringList paireDepenseMontant = totalSumsList[i].split("=");
            if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "paireDepenseMontant[1]  =" << paireDepenseMontant[1] ;
            if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "paireDepenseMontant[0]  =" << paireDepenseMontant[0] ;
            QTextTableCell cellDep          = tableRecap->cellAt(i,0);
             QTextCursor cellCursorDep      = cellDep.firstCursorPosition();
             QString paireDepenseMontantLeft = paireDepenseMontant[0];
             if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "paireDepenseMontantLeft  =" << paireDepenseMontantLeft ;
             if (paireDepenseMontantLeft == trUtf8("Total"))
             {
             	  
             	  if (WarnDebugMessage)
    	          qDebug() << __FILE__ << QString::number(__LINE__) << "in total";
             	  QString totalInHtml = QString("<html><font size = 4 color = #FF0000><bold>%1<bold></font></html>")
                                       .arg(paireDepenseMontantLeft);
             	  cellCursorDep.insertHtml(totalInHtml);
                 }
             else{
                  if (WarnDebugMessage)
    	          qDebug() << __FILE__ << QString::number(__LINE__) << "in else";
                  cellCursorDep.insertText(paireDepenseMontantLeft);
                 }
            QTextTableCell cellDep1         = tableRecap->cellAt(i,1);
             QTextCursor cellCursorDep1     = cellDep1.firstCursorPosition();
             cellCursorDep1                 . insertText(paireDepenseMontant[1]);
             if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "end of for";
        }
    }
    //calculparmois(listforquery,table, un,trenteetquelque);//calcul par type recette et mois
    cursortrieinfunction  ->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
   }
}//end of fillTable

void ProduceDoc::recupSlot(const QString & text){
    emit outThread(text);
}

QTextDocument *ProduceDoc::getTextDocument(){
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_textDocument =" << m_textDocument->toPlainText() ;
    return m_textDocument;
}

void ProduceDoc::dateChosen(QDate & date){
    m_mutex.lock();
    m_date = date;
    QString s = m_date.toString("yyyy");
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " date SSS =" << s ;
    m_mutex.unlock();
}

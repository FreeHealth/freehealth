#include <QtGui>
#include <iostream>
#include "saxhandler.h"
#include "patient.h"
#include "xmlwriterblock.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QtSql/QODBCDriver>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>

SaxHandler::SaxHandler(Patient* patienttest, QFile* filetest)
{
    patientptr = patienttest;
    fileptr = filetest;
}


bool SaxHandler::readFile()
{


    QSqlDatabase DB = QSqlDatabase::addDatabase("QODBC");
    DB.setDatabaseName("AXISANTE.DF1");
        if (!DB.open()){
            QMessageBox::critical(0, QObject::tr("Problem to open the Database !"),
                                  DB.lastError().text());
            return false;
        } else {

            // select from the table FPatient
            QSqlQuery query;
            query.prepare(QString("select * from FPatient WHERE (NCEPatient>10 AND NCEPatient<20)"));
            query.exec();
            if (query.isActive()) {

                while ( query.next() ) {
                    patientptr->setindex(query.value(0).toString());
                    patientptr->setnom(query.value(4).toString());
                    patientptr->setprenom(query.value(57).toString());
                    patientptr->setdatedenaissance(query.value(12).toString());
                    patientptr->setadresse(query.value(5).toString());
                    patientptr->setcodepostal(query.value(7).toString());
                    patientptr->setville(query.value(8).toString());
                    patientptr->settextelibre(query.value(29).toString());
                    patientptr->settelephone1(query.value(9).toString());
                    patientptr->settelephone2(query.value(10).toString());
               xmlwriterblock patientblock(fileptr, patientptr);
                }
            }return true;
       }
}



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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Jerome Pinguet <jerome@jerome.cc                       *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "processpatientimport_p.h"


using namespace PatientImport;

ProcessPatientImport::ProcessPatientImport(QString filename, QString software) : d_ptr(new Internal::ProcessPatientImportPrivate(this))
{
    d_ptr->m_fileName = filename;
    d_ptr->m_software = software;
    qWarning() << "ProcessPatientImport::ProcessPatientImport()";
    qWarning() << filename << "filename " << software << "software ";
    QStringList softwareList = PatientImport::Internal::PatientimportPlugin::softwareList();
    switch (softwareList.indexOf(software)) {
            case 0:
                break;
            case 1:
                ProcessPatientImport::parseGestcab();
                break;
            default:
                break;
    }
}

ProcessPatientImport::~ProcessPatientImport()
{
    if (d_ptr)
        delete d_ptr;
    d_ptr = 0;
}

void ProcessPatientImport::parseGestcab()
{
    if(d_ptr->m_fileName == NULL)
        return;
    QFile *file = new QFile(d_ptr->m_fileName);
    qWarning() << "ProcessPatientImport::parseGestcab()";
    if (!file->open(QIODevice::ReadOnly)) {
        std::cerr << "Cannot open file for reading: "
        << qPrintable(file->errorString()) << std::endl;
        return;
    }
    qWarning() << "file reading ok";
    QTextStream in(file);
    qWarning() << "QTextStream reading ok";
    qWarning() << "set codec ok";
    QVector<QVector<QString>> *test = new QVector<QVector<QString>>;
    //d_ptr->m_import = new QVector<QVector<QString>>;
    while (!in.atEnd()) {
        QString line = in.readLine();
        qWarning() << line;
        QStringList fields = line.split('@');
        QVector<QString> vect = QVector<QString>::fromList(fields);
        test->append(vect);
    }
    qDebug() << (test == NULL);
    qWarning() << "while loop over";
    d_ptr->m_import = test;

    for (int i = 0; i < 100; ++i) {
        qWarning()  << "***** Vector " << i << " *****";
        for (int j = 0; j < 43; ++j){
            qWarning() << j << d_ptr->m_import->at(i).at(j);
        }
    }
    qWarning() << d_ptr->m_import->size();
}

void ProcessPatientImport::importGestcab()
{
    for (int i = 0; i < d_ptr->m_import->size(); ++i) {
        qWarning()  << "***** Importing patient " << i << " *****";
        d_ptr->m_patientModel = new Patients::PatientModel(this);
        d_ptr->m_patientModel->setObjectName("PatientModelPatientImport");
        d_ptr->m_patientModel->setFilter("", "", QUuid::createUuid().toString() + "__FAKE", Patients::PatientModel::FilterOnUuid);


    }
}


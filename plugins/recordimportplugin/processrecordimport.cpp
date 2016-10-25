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
#include <memory>
#include "processrecordimport_p.h"

static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
static inline Patients::Internal::PatientBase *patientBase() { return Patients::Internal::PatientBase::instance(); }

using namespace RecordImport;

ProcessRecordImport::ProcessRecordImport(QString filename,
                                         QChar fieldSeparator,
                                         QChar recordSeparator)
    : d_ptr(new Internal::ProcessRecordImportPrivate(this))
{
    m_fileName = filename;
    m_fieldSeparator = fieldSeparator;
    m_recordSeparator = recordSeparator;
    qDebug() << Q_FUNC_INFO;
    qDebug() << filename << "filename ";
    ProcessRecordImport::modify();
    ProcessRecordImport::import();
}

ProcessRecordImport::~ProcessRecordImport()
{
    if (d_ptr)
        delete d_ptr;
    d_ptr = 0;
}

std::unique_ptr<QVector<QStringList>> ProcessRecordImport::parse(QString fileName,
                                                                 QChar fieldSeparator,
                                                                 QChar recordSeparator)
{
    if(fileName.isEmpty()) {
        return nullptr;
    }
    QFile file(fileName);
    qDebug() << Q_FUNC_INFO;
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Cannot open file for reading: "
                  << qPrintable(file.errorString()) << std::endl;
        return nullptr;
    }
    qWarning() << "file reading ok";
    QTextStream in(&file);
    qWarning() << "QTextStream reading ok";
    qWarning() << "set codec ok";
    qDebug() << "record " << recordSeparator;
    qDebug() << "field" << fieldSeparator;
    //QVector<QVector<QString>> *import = new QVector<QVector<QString>>;
    std::unique_ptr<QVector<QStringList>> data(new QVector<QStringList>);
    QChar character = 0;
    QStringList r;
    QString string;
    int i;
    int j;

    while (!in.atEnd()) {
        qDebug() << "*********** INSIDE main while loop";
        i++;
        if (i>2000)
            break;
        in >> character;
        //string+=character;
        qDebug() << "character " << character;
        qDebug() << "string " << string;

        while (!(character==recordSeparator) && !in.atEnd()) {
            qDebug() << "*********** INSIDE recordSeparator while loop";
            j++;
            if (j>2000)
                break;
            while (!(character==fieldSeparator)
                   && !(character==recordSeparator)
                   && !in.atEnd()) {
                qDebug() << "*********** INSIDE fieldSeparator while loop";
                //k++;
                //if (k>1000)
                //        break;

                qDebug() << "before >> " << character;
                //character = 0;
                in >> character;
                qDebug() << "after >> " << character;
                if (character=='\x0') {
                    string+="0";
                } else {
                    string+=character;
                }
                qDebug() << "string" << string;
            }
            qDebug() << character;
            if (!(character==recordSeparator)) {
                character=0;
            }
            if (string == fieldSeparator) {
                string.clear();
            }
            qDebug() << "string" << string;
            if (string.endsWith(fieldSeparator) || string.endsWith(recordSeparator)) {
                    string.chop(1);
            }
            qDebug() << "string" << string;
            r.append(string);
            string.clear();
        }
        if (character==recordSeparator)
        character = 0;
        qDebug() << r;
        data->append(r);
        r.clear();
    }
    qWarning() << "while loop over";
    // remove last empty QStringList
    if (!(data.get()->isEmpty())) {
        data->removeLast();
    }
    qWarning() << data->size();
    qWarning() << data.get()->size();

    QVector<int> error;
    for (int i = 0; i < data.get()->size(); ++i) {
        qWarning()  << "***** Vector " << i << " *****";
        for (int j = 0; j < data->at(i).size(); ++j){
            if (!(data->at(i).size()==data->at(0).size())) {
                qWarning() << "Number of QStringList items ("
                           << data->at(i).size()
                           << ") is different from number of header items ("
                           << data->at(0).size();
                error.append(i);
            }
            qWarning() << j << data->at(i).at(j);
        }
    }
    qWarning() << "Theses lines contained an incorrect number of items: "
               << error;

    return data;
}

void ProcessRecordImport::modify()
{
    // 1. Gender
    // convert "M","F","" to gender index value
    // "M" -> 0
    // "F" -> 1
    // ""  -> 3
    int gender3 = 0;
    qDebug() << Q_FUNC_INFO;
    QVector<QStringList>::iterator i;
    for (i = d_ptr->m_import->begin(); i != d_ptr->m_import->end(); i++) {
        if (i->value(8) == QString("M")) {
            qWarning() << i->value(8) << QString("M");
            i->removeAt(8);
            i->insert(8, QString("0"));
            qWarning() << i->value(8);
        }
        else if (i->value(8) == QString("F")) {
            qWarning() << i->value(8) << "F";
            i->removeAt(8);
            i->insert(8, QString("1"));
            qWarning() << i->value(8);
        }
        else if (i->value(8) == QString("")) {
            qWarning() << i->value(8) << "Unknown";
            // If Social security number exists
            // and if this number belongs to the patient (same DoB)
            // -> use it to determine gender
            // else gender is unknown
            if ((!i->value(24).isEmpty()) && (i->value(3) == i->value(34))) {
                qWarning() << "determining gender from SSN";
                if (i->value(24).startsWith("1")) {
                    i->removeAt(8);
                    qWarning() << "starts with 1";
                    i->insert(8, QString("1"));
                } else if (i->value(24).startsWith("2")) {
                    qWarning() << "starts with 2";
                    i->removeAt(8);
                    i->insert(8, QString("2"));
                } else {
                    i->removeAt(8);
                    i->insert(8, QString("3"));
                    qWarning() << i->value(8);
                    ++gender3;
                }
            }
        }
    }
    qWarning() << gender3 << "number of patients with unknown gender";
}

void ProcessRecordImport::import()
{
    qWarning() << d_ptr->m_import->size() << "d_ptr->m_import->size()";
    for (int j = 0; j < d_ptr->m_import->size(); ++j) {
        QStringList patientdata;
        foreach (QString s, d_ptr->m_import->at(j)) {
            patientdata << s;
        }
        if (patientdata.size()!=43)
            qWarning() << "QStringList size is different from 43";

        // Modify dob from dd/MM/yyyy to ISO yyyy-MM-dd
        QString stringdob = patientdata.at(3);
        stringdob = stringdob.remove(QChar('"')).remove(QChar('/'));
        QDate datedob = QDate::fromString(stringdob, "ddMMyyyy");
        stringdob = datedob.QDate::toString(Qt::ISODate);

        QString UsualName   = patientdata.at(1);
        QString Firstname   = patientdata.at(2);
        QString DateOfBirth = stringdob;
        QString OtherNames  = patientdata.at(5);
        QString GenderIndex = patientdata.at(8);
        QString Street      = patientdata.at(18);
        QString ZipCode     = patientdata.at(19);
        QString City        = patientdata.at(20);
        QString Tels        = patientdata.at(21);

        QString Gender;
        switch (GenderIndex.toInt()) {
        case 0:
            Gender = "M";
            break;
        case 1:
            Gender = "F";
            break;
        case 2:
            Gender = "H";
            break;
        case 3:
            Gender = "K"; // unKnown
            break;
        default:
            Gender = "K"; // unKnown
            break;
        }

        // Check for duplicate
        if (patientBase()->isPatientExists(UsualName,
                                           QString(),
                                           Firstname,
                                           QString(),
                                           datedob)) {
            qWarning() << UsualName << OtherNames << Firstname << GenderIndex
                       << DateOfBirth << "Patient" << j << "already exists !!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            QString duplicate = QString("The patient number %1 %2 %3 %4 %5 %6 already exists: skipping!")
                    .arg(j)
                    .arg(UsualName)
                    .arg(OtherNames)
                    .arg(Firstname)
                    .arg(GenderIndex)
                    .arg(stringdob);
            d_ptr->m_duplicate << duplicate;
            continue;
        }


        qWarning()  << "***** Importing patient " << j << " *****";
        d_ptr->m_patientModel = new Patients::PatientModel(this);
        connect(d_ptr->m_patientModel, SIGNAL(patientCreated(QString)), patient(), SIGNAL(patientCreated(QString)), Qt::UniqueConnection);
        d_ptr->m_patientModel->setObjectName("PatientModelPatientImport");
        d_ptr->m_patientModel->emitPatientCreationOnSubmit(true);
        d_ptr->m_patientModel->setFilter("", "", QUuid::createUuid().toString() + "__FAKE", Patients::PatientModel::FilterOnUuid);
        d_ptr->m_patientModel->insertRow(0);
        d_ptr->m_uuid = d_ptr->m_patientModel->index(0, Core::IPatient::Uid).data().toString();
        // Map each QString inside QVector->at(i) to corresponding d_ptr->m_patientModel column
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::UsualName),
                                       UsualName);
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::Firstname),
                                       Firstname);
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::DateOfBirth),
                                       DateOfBirth);
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::OtherNames),
                                       OtherNames);
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::GenderIndex),
                                       GenderIndex);
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::Street),
                                       Street);
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::ZipCode),
                                       ZipCode);
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::City),
                                       City);
        d_ptr->m_patientModel->setData(d_ptr->m_patientModel->index(0, Core::IPatient::Tels),
                                       Tels);
        d_ptr->m_patientModel->submit();
        if (!(d_ptr->m_patientModel==NULL)) {
            delete d_ptr->m_patientModel;
            d_ptr->m_patientModel=NULL;
        }
        //if (j == 50) break;
    }
    qWarning() << "Duplicates";
    foreach (QString s, d_ptr->m_duplicate) {
        qWarning() << s;
    }
}

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

static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
static inline Patients::Internal::PatientBase *patientBase() { return Patients::Internal::PatientBase::instance(); }

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
        ProcessPatientImport::modifyGestcab();
        ProcessPatientImport::importGestcab();
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
    //QVector<QVector<QString>> *import = new QVector<QVector<QString>>;
    d_ptr->m_import = new QVector<QStringList>;
    while (!in.atEnd()) {
        QString line = in.readLine();
        qWarning() << line;
        QStringList fields = line.split('@');
        d_ptr->m_import->append(fields);
    }
    qWarning() << "while loop over";

    for (int i = 0; i < 100; ++i) {
        qWarning()  << "***** Vector " << i << " *****";
        for (int j = 0; j < 43; ++j){
            qWarning() << j << d_ptr->m_import->at(i).at(j);
        }
    }
    qWarning() << d_ptr->m_import->size();
}

void ProcessPatientImport::modifyGestcab()
{
    // 1. Gender
    // convert "M","F","" to gender index value
    // "M" -> 0
    // "F" -> 1
    // ""  -> 3
    int gender3 = 0;
    qWarning() << "Modify gestcab";
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

void ProcessPatientImport::importGestcab()
{
    qWarning() << d_ptr->m_import->size() << "d_ptr->m_import->size()";

    // partial duplicate lists
    QStringList usualname_firstname;
    QStringList usualname_dob;
    QStringList firstname_dob;

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

        // Check for duplicates
        // exact match (Usual name, First name, DoB):
        // warn and skip patient creation
        if (patientBase()->isPatientExists(UsualName,
                                           QString(),
                                           Firstname,
                                           QString(),
                                           datedob)) {

            QString duplicate = QString("The patient number %1 %2 %3 %4 %5 %6 already exists: skipping!")
                                .arg(j)
                                .arg(UsualName)
                                .arg(OtherNames)
                                .arg(Firstname)
                                .arg(GenderIndex)
                                .arg(stringdob);
            d_ptr->m_duplicate << duplicate;
            qWarning() << duplicate;
            continue;
        }

        // warn about partial match
        // Usual name, First name
        if (patientBase()->isPatientExists(UsualName,
                                           QString(),
                                           Firstname,
                                           QString(),
                                           QDate())) {
            QString duplicate = QString("The patient number %1 %2 %3 %4 %5 %6"
                                        " has a partial id match: same usual name"
                                        " and first name")
                                .arg(j)
                                .arg(UsualName)
                                .arg(OtherNames)
                                .arg(Firstname)
                                .arg(GenderIndex)
                                .arg(stringdob);
            usualname_firstname << duplicate;
            qWarning() << duplicate;
        }
        // Usual name, DoB
        if (patientBase()->isPatientExists(UsualName,
                                           QString(),
                                           QString(),
                                           QString(),
                                           datedob)) {
            QString duplicate = QString("The patient number %1 %2 %3 %4 %5 %6"
                                        " has a partial id match: same usual name"
                                        " and DoB.")
                                .arg(j)
                                .arg(UsualName)
                                .arg(OtherNames)
                                .arg(Firstname)
                                .arg(GenderIndex)
                                .arg(stringdob);
            usualname_dob << duplicate;
            qWarning() << duplicate;
        }
        // Usual first name, DoB
        if (patientBase()->isPatientExists(QString(),
                                           QString(),
                                           Firstname,
                                           QString(),
                                           datedob)) {
            QString duplicate = QString("The patient number %1 %2 %3 %4 %5 %6"
                                        " has a partial id match: same first name"
                                        " and DoB.")
                                .arg(j)
                                .arg(UsualName)
                                .arg(OtherNames)
                                .arg(Firstname)
                                .arg(GenderIndex)
                                .arg(stringdob);
            firstname_dob << duplicate;
            qWarning() << duplicate;
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
    }

    qWarning() << "Exact match duplicates:";
    foreach (QString s, d_ptr->m_duplicate) {
        qWarning() << s;
    }
    qWarning() << "Exact match duplicates: usual name, first name";
    foreach (QString s, usualname_firstname) {
        qWarning() << s;
    }
    qWarning() << "Exact match duplicates: usual name, dob";
    foreach (QString s, usualname_dob) {
        qWarning() << s;
    }
    qWarning() << "Exact match duplicates: first name, dob";
    foreach (QString s, firstname_dob) {
        qWarning() << s;
    }
}


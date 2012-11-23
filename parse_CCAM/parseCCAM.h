/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.   *
 *   pm.desombre@medsyn.fr  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PARSECCAM_H
#define PARSECCAM_H
#include "ui_parse.h"
#include "findFile.h"
#include <QStandardItemModel>

//"Code";"c";"Texte";"Activité";"Phase";"Tarif(en euro)";"Rembt. ss Cdtions";"Accord Préalable";"Exo. TM";"Regroupement"
//"AEQM001";;"Échographie transcutanée de la moelle épinière";1;0;37,80;;;2;"ADE"
class ParseCcam : public QWidget, Ui::ParseWidget {
    Q_OBJECT
    enum itemsEnum{
        CODE = 0,
        C_CODE,
        TEXT,
        ACTIVITY,
        PHASE,
        PRICE,
        REIMBURSMENT,
        AGREEMENT,
        EXEMPTION,
        REGROUPMENT,
        ITEMS_ENUM_MaxParam
        };
    enum MPItems{
        MP_ID = 0,
        MP_UID,
        MP_USER_UID,
        MP_INSURANCE_UID,
        MP_NAME,
        MP_ABSTRACT,
        MP_TYPE,
        MP_AMOUNT,
        MP_DATE,
        MP_OTHERS,
        MP_COUNTRY,
        MP_MaxParam
        };
    enum XmlItems
    {
        XML_ACTIVITY = 0,
        XML_PHASE,
        XML_REIMBURSMENT,
        XML_AGREEMENT,
        XML_EXEMPTION,
        XML_REGROUPMENT,
        XmlItems_MaxParam
        };
    enum ModelItems
    {
        MODEL_CODE = 0,
        MODEL_TEXT,
        MODEL_PRICE,
        MODEL_BLOB,
        ModelItems_MaxParam 
        };
    enum NGAPitems
    {
        NGAP_CODE = 0,
        NGAP_TEXT,
        NGAP_TYPE,
        NGAP_AMOUNT,
        NGAP_OTHERS,
        NGAPitems_MaxParam
        };
    public :
        ParseCcam();
        ~ParseCcam();
        bool parseAndGetCsv();
    private Q_SLOTS:
        void searchCsvFile();
        void writeCsv();
        void writeMedicalProcedureCsv();
        void createMPDatapackDatabase();
        //void MPdatabaseUpdate();
    private :
        QString getXmlForBlobItem(   QString& activity,
                                     QString& phase,
                                     QString& reimbursment,
                                     QString& agreement,
                                     QString& exemption,
                                     QString& regroupment);
        QStandardItemModel * m_model;
        FindFile *m_f;

};

#endif

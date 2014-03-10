/***************************************************************************
 *   Copyright (C) 2008 by Eric MAEKER,,,   *
 *   eric.maeker@free.fr   *
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
#include "DRC_Static_Data.h"

/**
 * \class DRC_Static_Data
 * \brief Contient les données statiques de l'application.
 * Les arguments traduits de la ligne de commande
 * Les paramètres des fichiers ini (DrTux, eDRC)
 * Les différents répertoire à utiliser (multi-OS)
 * Les raccourcis claviers
 */

// RÉPERTOIRES ET FICHIERS DIVERSES
QString DRC_Static_Data::m_PathAppli               = "";
QString DRC_Static_Data::m_PathIni                 = ":/edrc.ini";
QString DRC_Static_Data::m_PathDrTux               = "";
QString DRC_Static_Data::m_PathDrTuxIni            = "/home/eric/Bureau/MedinTux-V-1.99/drtux/bin/drtux.ini";
QString DRC_Static_Data::m_PathExchg               = "";
QString DRC_Static_Data::m_MaskFile                = ":/masque.html";


// USER PAR DÉFAUT POUR PERMETTRE LES TESTS
QString DRC_Static_Data::m_MedinTux_User           = "MAEKER Eric";
QString DRC_Static_Data::m_MedinTux_GUIDPatient    = "335dc821-a7db-496d-a256-d1bc66ac9d28";
QString DRC_Static_Data::m_MedinTux_Id_User        = "4";
QString DRC_Static_Data::m_MedinTux_Observ_PrimKey = "5556";
QString DRC_Static_Data::m_Medintux_BaseName       = "DrTuxTest";


// BASE DE DONNÉES EDRC
QString DRC_Static_Data::m_BaseName             = "eDRC";
QString DRC_Static_Data::m_BaseLabel            = "eDRC";
QString DRC_Static_Data::m_BaseDriver           = "QMYSQL";
QString DRC_Static_Data::m_BaseUser             = "root";
QString DRC_Static_Data::m_BasePass             = "";
QString DRC_Static_Data::m_BaseHostName         = "localhost";

QString DRC_Static_Data::m_DebugMessage         = "";

// DONNÉES PATIENT
QString   DRC_Static_Data::m_PatientName        = "";
QDateTime DRC_Static_Data::m_PatientDDN         = QDateTime();
QString   DRC_Static_Data::m_PatientSexe        = "";
int       DRC_Static_Data::m_PatientIdFromBD    = -1;
QString   DRC_Static_Data::m_UserName           = "";
QDateTime DRC_Static_Data::m_ObservDate         = QDateTime();


// RACCOURCIS CLAVIERS INTERFACE DE CODAGE
QString DRC_Static_Data::m_DiagA_Keys           = "Alt+A";
QString DRC_Static_Data::m_DiagB_Keys           = "Alt+B";
QString DRC_Static_Data::m_DiagC_Keys           = "Alt+C";
QString DRC_Static_Data::m_DiagD_Keys           = "Alt+D";
QString DRC_Static_Data::m_DiagZ_Keys           = "Alt+Z";
QString DRC_Static_Data::m_SuiviN_Keys          = "Alt+N";
QString DRC_Static_Data::m_SuiviP_Keys          = "Alt+P";
QString DRC_Static_Data::m_SuiviR_Keys          = "Alt+R";
QString DRC_Static_Data::m_ALD_Yes_Keys         = "Alt+L";
QString DRC_Static_Data::m_ALD_No_Keys          = "Alt+X";
QString DRC_Static_Data::m_Sympto_Yes_Keys      = "Alt+S";
QString DRC_Static_Data::m_Sympto_No_Keys       = "Alt+Q";
QString DRC_Static_Data::m_ConsultOrder         = "Asc";
QString DRC_Static_Data::m_DateFormat_CsHist    = "ddd dd MMM yyyy hh:mm";
QString DRC_Static_Data::m_DateFormat_Export    = "ddd dd MMM yyyy hh:mm";
QString DRC_Static_Data::m_DateFormat_Comment   = "ddd dd MMM yyyy hh:mm";


bool    DRC_Static_Data::m_eDRCBase_Exists      = FALSE;
bool    DRC_Static_Data::m_PatientBase_Exists   = FALSE;

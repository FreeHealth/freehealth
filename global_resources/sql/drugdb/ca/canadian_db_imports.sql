-- /***************************************************************************
-- *   FreeMedicalForms                                                      *
-- *   (C) 2008-2010 by Eric MAEKER, MD                                      *
-- *   eric.maeker@free.fr                                                   *
-- *   All rights reserved.                                                  *
-- *                                                                         *
-- *   This program is a free and open source software.                      *
-- *   It is released under the terms of the new BSD License.                *
-- *                                                                         *
-- *   Redistribution and use in source and binary forms, with or without    *
-- *   modification, are permitted provided that the following conditions    *
-- *   are met:                                                              *
-- *   - Redistributions of source code must retain the above copyright      *
-- *   notice, this list of conditions and the following disclaimer.         *
-- *   - Redistributions in binary form must reproduce the above copyright   *
-- *   notice, this list of conditions and the following disclaimer in the   *
-- *   documentation and/or other materials provided with the distribution.  *
-- *   - Neither the name of the FreeMedForms' organization nor the names of *
-- *   its contributors may be used to endorse or promote products derived   *
-- *   from this software without specific prior written permission.         *
-- *                                                                         *
-- *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
-- *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
-- *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
-- *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
-- *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
-- *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
-- *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
-- *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
-- *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
-- *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
-- *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
-- *   POSSIBILITY OF SUCH DAMAGE.                                           *
-- ***************************************************************************/
-- /**************************************************************************
-- *   Main Developper : Jim Busser, MD <jbusser@interchange.ubc.ca>         *
-- *   Contributors :                                                        *
-- *       Eric MAEKER, MD <eric.maeker@free.fr>                             *
-- ***************************************************************************/

-- /**
--  * \file canadian_db_imports.sql
--  * \author Jim Busser, MD <jbusser@interchange.ubc.ca>, Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.4.4
--  * \date 27 July 2010
--  */

-- /**
--  * \brief This script imports the datas from the Canadian downloading
--  *
--  * \warning SQL commands MUST end with \e ; . There MUST BE nothing on the line after the ending \e ; .
--  *
--  * July 14, 2010 initial release
--  *
--  */

-- NB must first run download-and-reformat-ca.sh to obtain the .csv files below

.mode csv
.separator "|"

-- variants of unzip may output the files into a subdirectory allfiles/
-- for such variants, uncomment-enable instead the following :
-- .import allfiles/drug.csv drug
-- .import allfiles/form.csv form
-- .import allfiles/route.csv route
-- .import allfiles/status.csv status
-- .import allfiles/ingred.csv ingred
-- .import allfiles/ther.csv ther
-- .import allfiles/package.csv package

.import drug.csv drug
.import formshort.csv form
.import route.csv route
.import status.csv status
.import ingred.csv ingred
.import ther.csv ther
.import package.csv package

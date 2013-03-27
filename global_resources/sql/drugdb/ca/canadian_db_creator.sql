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
-- *   Main Developer:  Jim Busser, MD <jbusser@interchange.ubc.ca>         *
-- *  Contributors:                                                          *
-- *       Eric MAEKER, MD <eric.maeker@free.fr>                             *
-- ***************************************************************************/

-- /**
--  * \file canadian_db_creator.sql
--  * \author Jim Busser, MD <jbusser@interchange.ubc.ca>, Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.4.5
--  * \date 14 Dec 2010
--  */

-- /**
--  * \brief This script creates a sqlite db from Health Canada Drug Product Data
--  *
--  * Dec 14, 2010 script is renamed and splitted into multiple files:
--  *   - as before, canadian_db_creator.sql
--  *     (which mainly reads other sql files)
--  *   - others… see .read statements below)
--  * July 14, 2010 script is renamed and splitted into multiple files:
--  * July 14, 2010 doxygen comments, licence header and file descriptor are added
--  * July 9, 2010 supports new (April 2010) HC drug field DESCRIPTOR VARCHAR2(150)
--  * July 9, 2010 bugfix: made the INSERT into DRUGS to be SELECT DISTINCT
--  *
--  * NOTE: INSERT INTO "INFORMATIONS" (needs manual updating as to
--  *  i)  the version of the drug db that it is to be known as
--  *  ii) the compatible version of FreeDiams
--  */

-- Prepare Canadian temporary database
.read canadian_db_tables.sql

-- Imports Canadian raw datas
.read canadian_db_imports.sql

-- Create FreeDiams tables
.read drugs_schema.sql

-- Compute the temporary database and create the final FreeDiams database
.read canadian_db_processing.sql

-- Compute the temporary database and create the final FreeDiams database
.read canadian_db_finalize.sql

.exit

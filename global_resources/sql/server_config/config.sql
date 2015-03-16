-- /***************************************************************************
--  *  The FreeMedForms project is a set of free, open source medical         *
-- *  applications.                                                           *
--  *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
--  *  All rights reserved.                                                   *
--  *                                                                         *
--  *  This program is free software: you can redistribute it and/or modify   *
--  *  it under the terms of the GNU General Public License as published by   *
--  *  the Free Software Foundation, either version 3 of the License, or      *
--  *  (at your option) any later version.                                    *
--  *                                                                         *
--  *  This program is distributed in the hope that it will be useful,        *
--  *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
--  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
--  *  GNU General Public License for more details.                           *
--  *                                                                         *
--  *  You should have received a copy of the GNU General Public License      *
--  *  along with this program (COPYING.FREEMEDFORMS file).                   *
--  *  If not, see <http://www.gnu.org/licenses/>.                            *
--  ***************************************************************************/
-- /**************************************************************************
-- *   Main Developer:  Eric MAEKER, MD <eric.maeker@free.fr>               *
-- *  Contributors:                                                          *
-- ***************************************************************************/

-- /**
--  * \file global_resources/sql/server_config/config.sql
--  * \author Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.10.0
--  * \date 22 Jul 2014
--  */

-- /**
--  * \brief Use this script to configure you MySQL server.
--  *
--  * You must be connected with a MySQL server administrator user.
--  *
--  * \warning SQL commands MUST end with \e ;
--  *
--  */

-- Create the main FreeMedForms administrator on MySQL user
-- An error will occur if the user already exists and the script will fail
CREATE USER 'fmf_admin' IDENTIFIED BY 'fmf_admin';

GRANT CREATE USER, GRANT OPTION, RELOAD, SHOW DATABASES ON *.* TO 'fmf_admin'@'%' IDENTIFIED BY 'fmf_admin';
GRANT SELECT, UPDATE, INSERT, DELETE, CREATE, DROP, ALTER, INDEX, GRANT OPTION ON `%fmf\_%`.* TO 'fmf_admin'@'%' IDENTIFIED BY 'fmf_admin';

GRANT CREATE USER, GRANT OPTION, RELOAD, SHOW DATABASES ON *.* TO 'fmf_admin'@'localhost' IDENTIFIED BY 'fmf_admin';
GRANT SELECT, UPDATE, INSERT, DELETE, CREATE, DROP, ALTER, INDEX, GRANT OPTION ON `%fmf\_%`.* TO 'fmf_admin'@'localhost' IDENTIFIED BY 'fmf_admin';

GRANT CREATE USER, GRANT OPTION, RELOAD, SHOW DATABASES ON *.* TO 'fmf_admin'@'127.0.0.1' IDENTIFIED BY 'fmf_admin';
GRANT SELECT, UPDATE, INSERT, DELETE, CREATE, DROP, ALTER, INDEX, GRANT OPTION ON `%fmf\_%`.* TO 'fmf_admin'@'127.0.0.1' IDENTIFIED BY 'fmf_admin';

GRANT CREATE USER, GRANT OPTION, RELOAD, SHOW DATABASES ON *.* TO 'fmf_admin'@'::1' IDENTIFIED BY 'fmf_admin';
GRANT SELECT, UPDATE, INSERT, DELETE, CREATE, DROP, ALTER, INDEX, GRANT OPTION ON `%fmf\_%`.* TO 'fmf_admin'@'::1' IDENTIFIED BY 'fmf_admin';

GRANT SELECT, UPDATE, INSERT, DELETE, CREATE ON `mysql`.`user` TO 'fmf_admin'@'localhost' IDENTIFIED BY 'fmf_admin';
GRANT SELECT, UPDATE, INSERT, DELETE, CREATE ON `mysql`.`user` TO 'fmf_admin'@'%' IDENTIFIED BY 'fmf_admin';
GRANT SELECT, UPDATE, INSERT, DELETE, CREATE ON `mysql`.`user` TO 'fmf_admin'@'127.0.0.1' IDENTIFIED BY 'fmf_admin';
GRANT SELECT, UPDATE, INSERT, DELETE, CREATE ON `mysql`.`user` TO 'fmf_admin'@'::1' IDENTIFIED BY 'fmf_admin';

FLUSH PRIVILEGES;

-- Create the main FreeMedForms administrator on FreeMedForms user table

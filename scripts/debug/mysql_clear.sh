#!/bin/bash
#  The FreeMedForms project is a set of free, open source medical
#  applications.
#  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>
#  License: BSD 3 clauses

VERSION="0.1"

MYSQL=/usr/local/mysql/bin/mysql
MYSQL_ROOT_PASS=""

if [[ -e /usr/bin/mysql ]]; then
  MYSQL=/usr/bin/mysql
elif [[ -e /usr/sbin/mysql ]]; then
  MYSQL=/usr/sbin/mysql
elif [[ -e /usr/local/bin/mysql ]]; then
  MYSQL=/usr/local/bin/mysql
fi

SCRIPT_NAME=`basename $0`
if [[ "`echo $0 | cut -c1`" = "/" ]]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

showHelp()
{
  echo $SCRIPT_NAME" clear your MySQL server from all freemedforms data."
  echo "This script is part of the FreeMedForms project."
  echo "Version: $VERSION"
  echo "Usage : $SCRIPT_NAME -p rootpassword"
  echo "Options :"
  echo " -p  define the mysql root password"
  echo " -h  show this help"
  echo
}

# Parse options
while getopts "p:h" option
do
        case $option in
                p) MYSQL_ROOT_PASS=`echo "-p$OPTARG" | tr -d " "`;
                ;;
                h) showHelp
                    exit 0
                ;;
        esac
done

MYSQL="$MYSQL -uroot $MYSQL_ROOT_PASS"

if [[  "$MYSQL_ROOT_PASS" == "" ]]; then
    echo "*** No password"
else
    echo "*** Using password: "$MYSQL_ROOT_PASS
fi
echo "    Default MySQL command: "$MYSQL

echo
echo "*** Using MySQL from $MYSQL ***"
# DROP FMF USERS
echo "SELECT DISTINCT mysql.user.User, \"'@'\", mysql.user.Host FROM mysql.db" > ./select.sql
echo "JOIN mysql.user on mysql.user.User=mysql.db.User" >> ./select.sql
echo "WHERE mysql.db.Db='fmf\_%';" >> ./select.sql

echo
echo "*** Drop FreeMedForms users ***"
$MYSQL < ./select.sql | sed '1d' | tr -d "\t" | sed "s/^/DROP USER '/" | sed "s/$/';/" > ./drop.sql

if [[ -z ./drop.sql ]]; then
    echo
    echo "*** No recorded users ***"
else
    more drop.sql
    read -n1 -p "Execute these commands? [y/n]"
    if [[ $REPLY = [yY] ]]; then
        echo
        echo "*** Executing commands ***"
        $MYSQL < drop.sql
    else
        echo
        echo "*** Commands ignored ***";
    fi
fi
rm ./select.sql
rm ./drop.sql

echo
echo "*** Drop FreeMedForms databases ***"
echo "show databases LIKE 'fmf_%';" > ./select.sql
$MYSQL < ./select.sql | sed '1d' | tr -d "\t"  | sed "s/^/DROP DATABASE /" | sed "s/$/;/" > ./drop.sql

if [[ -z ./drop.sql ]]; then
    echo
    echo "*** No freemedforms database ***"
else
    more drop.sql
    read -n1 -p "Execute these commands? [y/n]"
    if [[ $REPLY = [yY] ]]; then
        echo
        echo "*** Executing commands ***"
        $MYSQL < drop.sql
    else
        echo
        echo "*** Commands ignored";
    fi
fi

rm ./select.sql
rm ./drop.sql

echo
echo "*** Finished ***"
exit 0

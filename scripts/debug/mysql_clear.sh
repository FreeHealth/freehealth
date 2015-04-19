#!/bin/bash
#  The FreeMedForms project is a set of free, open source medical
#  applications.
#  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>
#  License: BSD 3 clauses

SCRIPT_VERSION=1.0-1341675001  # see date "+%s"

MYSQL=/usr/local/mysql/bin/mysql
MYSQL_USER="root"
MYSQL_PASS=""
MYSQL_HOST=""
MYSQL_PORT=""
MYSQL_DB_PREFIX=""

SQL_USERS=""
SQL_DATABASES=""

ZENITY="/usr/bin/zenity"
SHOW_ZENITY_PROGRESS="y"  # y / n
LOG_FILE=""
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

# get version number of FreeDiams from the project file
if [[ -f "$SCRIPT_PATH/../../buildspecs/projectversion.pri" ]]; then
    PROJECT_VERSION=`cat $SCRIPT_PATH/../../buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`
else
    echo "* WARNING: no project file. Are you inside the freemedforms sources ?"
fi

checkMySQLPath()
{
    if [[ -e /usr/bin/mysql ]]; then
        MYSQL=/usr/bin/mysql
    elif [[ -e /usr/sbin/mysql ]]; then
        MYSQL=/usr/sbin/mysql
    elif [[ -e /usr/local/bin/mysql ]]; then
        MYSQL=/usr/local/bin/mysql
    elif [[ -e /usr/local/mysql/bin/mysql ]]; then
        MYSQL=/usr/local/mysql/bin/mysql
    else
        echo "* WARNING: unable to find mysql"
        exit 123
    fi
}

showHelp()
{
  echo $SCRIPT_NAME" clear your MySQL server from all freemedforms data."
  echo "This script is part of the FreeMedForms project."
  echo "Version: $SCRIPT_VERSION"
  echo "Usage : $SCRIPT_NAME -p rootpassword"
  echo "Options :"
  echo " -u  define the mysql root user login"
  echo " -p  define the mysql root user password"
  echo " -s  define the mysql hostname"
  echo " -t  define the mysql port"
  echo " -x  define the mysql database prefix"
  echo " -h  show this help"
  echo
}

checkZenity()
{
    if [[ -f $ZENITY ]]; then
        echo "* Using zenity"
    else
        echo "* zenity not found"
    fi
}

logConfig()
{
    if [[ "$MYSQL_PASS" == "" ]]; then
        echo "*** No password"
    else
        echo "*** Using password: "$MYSQL_PASS
    fi
    echo "*** Using MySQL command: $MYSQL ***"
    echo "*** Using MySQL database prefix: $MYSQL_DB_PREFIX ***"
    # checkZenity
}

getMySQLUser()
{
    if [[ -z $MYSQL_USER && -f $ZENITY ]]; then
        ENTRY=`$ZENITY --title 'Please enter your MySQL user' --password --username`
        if [[ "$ENTRY" == "0" ]] ; then
            MYSQL_USER=`echo $ENTRY | cut -d'|' -f1`
            MYSQL_PASSWORD=`echo $ENTRY | cut -d'|' -f2`
        else
            MYSQL_USER=""
            MYSQL_PASSWORD=""
            return 1;
        fi
    fi
    if [[ -z $MYSQL_USER ]]; then
        echo "* ERROR: no user specified. Exiting."
        exit 123;
    fi
}

showDropFile()
{
    cat ./drop.sql
}

showSqlFile()
{
    cat ./select.sql
}

dropUsers()
{
    echo "SELECT DISTINCT mysql.user.User, \"'@'\", mysql.user.Host FROM mysql.db" > ./select.sql
    echo "JOIN mysql.user on mysql.user.User=mysql.db.User" >> ./select.sql
    echo "WHERE mysql.db.Db='"$MYSQL_DB_PREFIX"%';" >> ./select.sql
    echo "*** Drop FreeMedForms users ***"
    $MYSQL < ./select.sql | sed '1d' | tr -d "\t" | sed "s/^/DROP USER '/" | sed "s/$/';/" > ./drop.sql

    if [[ -e ./drop.sql ]]; then
        echo
        echo "*** No recorded users ***"
    else
        showDropFile
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
}

dropDatabases()
{
    echo "*** Drop FreeMedForms databases ***"
    echo "show databases LIKE '"$MYSQL_DB_PREFIX"%';" > ./select.sql
    more ./select.sql
    $MYSQL < ./select.sql | sed '1d' | tr -d "\t"  | sed "s/^/DROP DATABASE /" | sed "s/$/;/" > ./drop.sql
    more ./drop.sql

    if [[ -z ./drop.sql ]]; then
        echo "*** No freemedforms database ***"
    else
        showDropFile
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
}

checkMySQLPath
# Parse options
while getopts "u:p:s:t:x:h" option
do
        case $option in
                u) MYSQL_USER=$OPTARG;
                ;;
                p) MYSQL_PASS=`echo "-p$OPTARG" | tr -d " "`;
                ;;
                s) MYSQL_HOST="--host=\"$OPTARG\"";
                ;;
                t) MYSQL_PORT="--port=\"$OPTARG\"";
                ;;
                x) MYSQL_DB_PREFIX=$OPTARG;
                ;;
                h) showHelp
                    exit 0
                ;;
        esac
done

MYSQL="$MYSQL -u $MYSQL_USER $MYSQL_PASS $MYSQL_HOST $MYSQL_PORT"
# Escape any _ with a \ for the database prefix
MYSQL_DB_PREFIX=$MYSQL_DB_PREFIX"fmf_"
MYSQL_DB_PREFIX=`echo $MYSQL_DB_PREFIX | sed "s/_/\\\\\\\\_/g"`
echo $MYSQL_DB_PREFIX

logConfig
getMySQLUser
dropUsers
dropDatabases

echo
echo "*** Finished ***"
exit 0

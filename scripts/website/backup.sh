#!/bin/sh

# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2011  Eric MAEKER, MD
#

# paths
SERVER_PATH="/home/ericmaek/www/freemedforms"
DOKU_ROOT_PATH=$SERVER_PATH"/dokuwiki"

#SERVER_PATH="/Library/WebServer/Documents/www"
#DOKU_ROOT_PATH=$SERVER_PATH"/dokuwiki"

DOKU_PLUGINS="lib/plugins"
DOKU_PAGES="data/pages"
DOKU_MEDIAS="data/media"
DOKU_CONF="conf/local.php conf/users.auth.php"
DOKU_ATTIC="data/attic"
DOKU_TEMPLATES="lib/tpl/arctic lib/tpl/typo"

BACKUP_PATH=$SERVER_PATH"/doku_bkup"
#BACKUP_PATH="/Users/eric/doku_bkup"


MYDATE="`date '+%Y%m%d.%H%M'`"
LOG_FILE="${BACKUP_PATH}/backup.log"

BACK_FILENAME="backup"
BACK_ABSFILENAME=${BACKUP_PATH}"/"${BACK_FILENAME}

NB_BACKUPS="7"    # keep this amount of old backup

#-- creates $1, if not existant
checkDir() {
    if [ ! -d "${BACKUP_PATH}/$1" ] ; then
        mkdir -p "${BACKUP_PATH}/$1"
    fi
}

# 1 -> path
# 2 -> name
# 3 -> number of backups
rotateDir() {
    for i in `seq $(($3 - 1)) -1 0`
      do
      if [ -d "$1/$2-$i" ] ; then
          /bin/rm -f "$1/$2-$((i + 1))"
          mv "$1/$2-$i" "$1/$2-$((i + 1))"
      fi
    done
}

#-- make sure everything exists
checkDir
checkDir "yearly"

#-- first step: rotate yearly.
rotateDir "${BACKUP_PATH}/yearly" "$BACK_FILENAME" "$NB_BACKUPS"

# then create our backup
cd $DOKU_ROOT_PATH
sdf=`pwd`
echo $sdf

tar --exclude=".*" -cjf "${BACK_ABSFILENAME}.tar.bz2" \
$DOKU_PLUGINS \
$DOKU_PAGES \
$DOKU_MEDIAS \
$DOKU_CONF \
$DOKU_ATTIC \
$DOKU_TEMPLATES

exit 0

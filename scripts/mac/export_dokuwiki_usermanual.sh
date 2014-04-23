#!/bin/bash
# author: samlt
# 20110221
# https://raw.github.com/vonloxley/offline-dokuwiki/master/offline-dokuwiki.sh
# Adaptation to the FreeMedForms project: Eric Maeker, MD <eric.maeker@gmail.com>

# default values
DEF_HOSTNAME=mydoku.wiki.lan
#DEF_LOCATION=path/to/start
DEF_LOCATION=fullindex
USERNAME=
PASSWORD=
PROTO=http
DEF_DEPTH=2
ADDITIONNAL_WGET_OPTS=${AWO}
PROGNAME=${0##*/}
WGET="./wget"

show_help() {
   cat<<EOT

NAME
   $PROGNAME: make an offline export of a dokuwiki documentation

SYNOPSIS
   $PROGNAME options

OPTIONS
   --login      username
   --passwd     password
   --ms-filenames
   --https
   --depth      number
   --hostname   doku.host.tld
   --location   path/to/start

NOTES
   if not specified on the command line
      * username and password are empty
      * hostname defaults to '$DEF_HOSTNAME'
      * location defaults to '$DEF_LOCATION'

EOT
}

while [ $# -gt 0 ]; do
   case "$1" in
      --login)
         shift
         USERNAME=$1
         ;;
      --passwd)
         shift
         PASSWORD=$1
         ;;
      --hostname)
         shift
         HOST=$1
         ;;
      --depth)
         shift
         DEPTH=$1
         ;;
      --location)
         shift
         LOCATION=$1
         ;;
      --https)
         PROTO=https
         ;;
      --ms-filenames)
         ADDITIONNAL_WGET_OPTS="$ADDITIONNAL_WGET_OPTS --restrict-file-names=windows"
         ;;
      --help)
         show_help
         exit
         ;;
   esac
   shift
done

: ${DEPTH:=$DEF_DEPTH}
: ${HOST:=$DEF_HOSTNAME}
: ${LOCATION:=$DEF_LOCATION}

PREFIX="$(date +'%Y%m%d')-$HOST"

echo "[WGET] downloading: start: http://$HOSTNAME/$LOCATION (login/passwd=${USERNAME:-empty}/${PASSWORD:-empty})"
$WGET  --no-verbose \
      --recursive \
      --level="$DEPTH" \
      --execute robots=off \
      --no-parent \
      --page-requisites \
      --convert-links \
      --http-user="$USERNAME" \
      --http-password="$PASSWORD" \
      --auth-no-challenge \
      --adjust-extension \
      --exclude-directories=_detail,_export \
      --reject="feed.php*,*do=backlink.html,*do=edit*,*do=index.html,*indexer.php?id=*,*do=admin.html,*do=revisions.html,*do=media*" \
      --directory-prefix="$PREFIX" \
      --no-host-directories \
      $ADDITIONNAL_WGET_OPTS \
      "$PROTO://$HOST/$LOCATION"


echo
echo "[SED] fixing links(href...) in the HTML sources"
sed -i -e 's#href="\([^:]\+:\)#href="./\1#g' \
       -e "s#\(indexmenu_\S\+\.config\.urlbase='\)[^']\+'#\1./'#" \
       -e "s#\(indexmenu_\S\+\.add('[^']\+\)#\1.html#" \
       -e "s#\(indexmenu_\S\+\.add([^,]\+,[^,]\+,[^,]\+,[^,]\+,'\)\([^']\+\)'#\1./\2.html'#" \
       ${PREFIX}/${LOCATION%/*}/*.html

#!/bin/bash

MYSQL=/usr/local/mysql/bin/mysql

if [[ -e /usr/bin/mysql ]]; then
  MYSQL=/usr/bin/mysql
elif [[ -e /usr/sbin/mysql ]]; then
  MYSQL=/usr/sbin/mysql
elif [[ -e /usr/local/bin/mysql ]]; then
  MYSQL=/usr/local/bin/mysql
fi

echo
echo "*** Using MySQL from $MYSQL ***"
# DROP FMF USERS
echo "SELECT DISTINCT mysql.user.User, \"'@'\", mysql.user.Host FROM mysql.db" > ./select.sql
echo "JOIN mysql.user on mysql.user.User=mysql.db.User" >> ./select.sql
echo "WHERE mysql.db.Db='fmf\_%';" >> ./select.sql

echo
echo "*** Drop FreeMedForms users ***"
$MYSQL -uroot -p < ./select.sql | sed '1d' | tr -d "\t" | sed "s/^/DROP USER '/" | sed "s/$/';/" > ./drop.sql

more drop.sql
read -n1 -p "Execute these commands? [y/n]"
if [[ $REPLY = [yY] ]]; then 
   echo
   echo "*** Executing commands ***"
   $MYSQL -uroot < drop.sql
else
   echo
   echo "*** Commands ignored ***";
fi

rm ./select.sql
rm ./drop.sql

echo
echo "*** Drop FreeMedForms databases ***"
echo "show databases LIKE 'fmf_%';" > ./select.sql
$MYSQL -uroot -p < ./select.sql | sed '1d' | tr -d "\t"  | sed "s/^/DROP DATABASE /" | sed "s/$/;/" > ./drop.sql

more drop.sql
read -n1 -p "Execute these commands? [y/n]"
if [[ $REPLY = [yY] ]]; then 
   echo
   echo "*** Executing commands ***"
   $MYSQL -uroot < drop.sql
else
   echo
   echo "*** Commands ignored";
fi

rm ./select.sql
rm ./drop.sql

echo
echo "*** Finished ***"
exit 0

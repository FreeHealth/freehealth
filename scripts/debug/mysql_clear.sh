#!/bin/bash

MYSQL=/usr/local/mysql/bin/mysql

if [[ -e /usr/bin/mysql ]]; then
  MYSQL=/usr/bin/mysql
elif [[ -e /usr/sbin/mysql ]]; then
  MYSQL=/usr/sbin/mysql
elif [[ -e /usr/local/bin/mysql ]]; then
  MYSQL=/usr/local/bin/mysql
fi

# DROP FMF USERS
echo "SELECT DISTINCT mysql.user.User, \"'@'\", mysql.user.Host FROM mysql.db" > ./select.sql
echo "JOIN mysql.user on mysql.user.User=mysql.db.User" >> ./select.sql
echo "WHERE mysql.db.Db='fmf\_%';" >> ./select.sql

$MYSQL -uroot -p < ./select.sql | sed '1d' | tr -d "\t" | sed "s/^/DROP USER '/" | sed "s/$/';/" > ./drop.sql

echo "*** Drop FreeMedForms users ***"
more drop.sql
$MYSQL -uroot < drop.sql

rm ./select.sql
rm ./drop.sql

echo "show databases LIKE 'fmf_%';" > ./select.sql
$MYSQL -uroot -p < ./select.sql | sed '1d' | tr -d "\t"  | sed "s/^/DROP DATABASE /" | sed "s/$/;/" > ./drop.sql

echo "*** Drop FreeMedForms databases ***"
more drop.sql
$MYSQL -uroot < drop.sql

echo "*** Finished ***"
exit 0

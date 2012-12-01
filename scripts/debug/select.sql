SELECT DISTINCT mysql.user.User, "'@'", mysql.user.Host FROM mysql.db
JOIN mysql.user on mysql.user.User=mysql.db.User
WHERE mysql.db.Db='fmf\_%';

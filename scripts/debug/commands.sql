SELECT '### ROOT GRANTS';\n
SHOW GRANTS FOR 'root'@'localhost';\n
SELECT ' ';\n
SELECT '### FMF_ADMIN GRANTS';\n
SHOW GRANTS FOR 'fmf_admin'@'localhost';\n
SELECT ' ';\n
SELECT '### LOCALHOST ANONYMOUS USER';\n
SELECT mysql.user.Host, mysql.user.User, mysql.user.Password FROM mysql.user WHERE (mysql.user.User is null and mysql.user.Host='localhost');\n
SELECT ' ';\n
SELECT '### FMF_ DATABASES';\n
SHOW DATABASES LIKE 'fmf_%';\n
SELECT ' ';\n
SELECT '### ALL DATABASES';\n
SHOW DATABASES;\n
SELECT ' ';\n
SELECT '### FMF_USERS';\n
SELECT fmf_users.users.NAME, fmf_users.users.SURNAME, fmf_users.users.USER_UUID, fmf_users.users.ISVIRTUAL FROM fmf_users.users WHERE fmf_users.users.VALIDITY=1;\n
SELECT ' ';\n
SELECT '### MYSQL USERS FOR FMF';\n
SELECT DISTINCT mysql.user.User, mysql.user.Host FROM mysql.db
JOIN mysql.user on mysql.user.User=mysql.db.User
WHERE mysql.db.Db='fmf\_%';

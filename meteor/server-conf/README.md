# Configure OVH VPS to deploy/run Meteor multiple applications

Find here some notes I wrote when configuring a Debian Wheezy VPS OVH hosted to run [Meteor.js] application(s).
The main objective is to configure the server with:
- Latest Debian stable release
- non-root users (sudoers)
- upstart instead of sysinit
- node.js & meteor
- nginx
- meteor-up (aka [mup])
- git (for private code sharing)

# Create sudoers

```bash
# Install required apps
sudo apt-get install sudo
sudo apt-get install sshpass bzip2 python python3 build-essential
# Create a new sudoer
addgroup USERNAME sudo
# nano /etc/sudoers
```

# Install upstart

Debian system is running a sysinit daemon. Upstart was created by canonical to replace sysinit, but meteor-up requires upstart. So we need to install it. 
During installation, you will be prompted for a confirmation. Don't be worried, just confirm and go on. After intallation is done, we will need to reboot the VPS.

```bash
# Install upstart
apt-get install upstart
# Answer 'yes' to all questions
/sbin/reboot
# ctrl+D
```

# Get nodejs & npm

### Download compiled binaries
```bash
wget http://nodejs.org/dist/v0.10.33/node-v0.10.33-linux-x64.tar.gz
cd /usr/local && sudo tar zxvf ~/node*.tar.gz --strip-components=1
```

### Build from sources
```bash
wget http://nodejs.org/dist/node-latest.tar.gz
tar zxvf node-latest.tar.gz
cd node*
./configure
make
make install
```

### Recommanded way
```bash
wget --no-check-certificate https://npmjs.org/install.sh | sudo sh
sudo sh install.sh
```

# Meteor-Up

We will use Meteor-Up aka [mup] for the deployment process of our applications. Please read the documentation on the [mup] github site.

### Download and install using 'npm'

```bash
sudo npm install -g mup
sudo npm update mup -g
```

Server initialisation will be presented below.

# Install mongodb on the server

We want to host multiple applications. Each application will connect to a global mongodb instance instead of multiples instances of mongodb.

```bash
# Add mongodb repository
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 7F0CEB10
echo 'deb http://downloads-distro.mongodb.org/repo/debian-sysvinit dist 10gen' | sudo tee /etc/apt/sources.list.d/mongodb.list
# Update apt
sudo apt-get update
# Install the updated mongodb
sudo apt-get install mongodb-10gen
# Not required: apt-get install mongodb-server
# Check if mongodb is running
netstat -ln | grep -E '27017|28017'
```

### Databases automatic backup

If you want to add auto-database-backup using cron, use this trick.

```bash
/etc/cron.d/mongodb-backup: @daily root mkdir -p /var/backups/mongodb; mongodump --db MONGO_DATABASE_APPNAME --out /var/backups/mongodb/$(date +'\%Y-\%m-\%d')
```

### Create mongodb users with correct rights/roles for our app management

We need an *admin* user and a specific user that has correct rights/roles for the application database.

```bash
# Start mongodb
su - mongodb
mongod --fork --logpath /var/log/mongodb.log
# Open mongo shell
mongo
# In the mongo shell
> db = db.getSiblingDB('admin')
> db.createUser({ user: 'admin', pwd: 'YOUR_PASSWORD_TO_REMEMBER', roles: [ "userAdminAnyDatabase" ] })
# Quit mongo shell: Ctrl+C
# Ensure that we use authentication in mongodb.conf
sudo nano /etc/mongodb.conf
  auth = true

# Restart mongodb daemon
sudo initctl restart mongodb
# sysinit: sudo service mongodb restart
# if mongo do not want to start (permission issue)
# sudo chown -R mongodb:mongodb /var/lib/mongodb

# Create the mongo user for the application
mongo -u admin -p --authenticationDatabase admin
> use ssrDB
> db.createUser({ user: 'YOUR_MONGO_APPUSER_LOGIN', pwd: 'YOUR_MONGO_APPUSER_PASSWORD', roles: [ 'readWrite' ] })
```

PS: you will need *YOUR_MONGO_APPUSER_LOGIN* and *YOUR_MONGO_APPUSER_PASSWORD* inside the environment variable "MONGO_URL" in the mup.json on your local machine in order to correctly deploy.

### Create the Meteor.js application on the server

```bash
# Create the path on the server where the application will be stored when deployed
mkdir /var/meteor/APPNAME # or any path
cd /var/meteor/APPNAME
mup init
# Edit the mup configuration file for the server part. Adapt to your needs.
nano mup.json
    ...
    "setupMongo": false,
    "setupNode": false,
    ...
  "env": {
    "PORT": 80,
    "ROOT_URL": "http://myapp.com",
    "MONGO_URL": "mongodb://MONGO_USERNAME:YOUR_MONGO_APPUSER_PASSWORD@127.0.0.1:27017/MONGO_DATABASE_APPNAME"
  },
```

# Remove user password authentication for mup setup

```bash
nano /etc/sudoers
   # replace this line
   %admin  ALL=(ALL) ALL
   # by this line
   %admin ALL=(ALL) NOPASSWD:ALL
sudo service sudo restart
```

# Some tricks
```bash
cd /usr/local/lib/node_modules/wait-for-mongo/node_modules/mongodb/node_modules/bson
sudo node-gyp rebuild
```

# Setting up nginx as HTTP(S) provider/proxy

```bash
# Install nginx
sudo apt-get install nginx
# Create nginx upstart commands
sudo nano /etc/init/nginx.conf
# Write the following into this file
description "nginx http daemon"
author "George Shammas <georgyo@gmail.com>"
 
start on (filesystem and net-device-up IFACE=lo)
stop on runlevel [!2345]
env DAEMON=/usr/sbin/nginx
env PID=/var/run/nginx.pid
expect fork
respawn
respawn limit 10 5
#oom never
pre-start script
        $DAEMON -t
        if [ $? -ne 0 ]
                then exit $?
        fi
end script
exec $DAEMON
############ EOF
```

### Configuring nginx for one unique application

```bash
# Create the configuration file for your server name
sudo nano /etc/nginx/sites-enabled/SERVER_NAME
server {
  listen                *:80;
  server_name           SERVER_NAME;
  access_log            /var/log/nginx/app.dev.access.log;
  error_log             /var/log/nginx/app.dev.error.log;

  location / {
    proxy_pass http://127.0.0.1:8000;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection 'upgrade';
    proxy_set_header X-Forwarded-For $remote_addr;
  }
}
sudo ln -s /etc/nginx/sites-available/SERVER_NAME /etc/nginx/sites-enabled/SERVER_NAME
```

### Check if nginx is ready

```bash
sudo initctl list | grep nginx
# Start nginx
sudo initctl start nginx
# Restart nginx
sudo initctl restart nginx
```

# Deploy multiple Meteor apps in subfolders

Our application is called APPNAME. It is running on our server SERVER_NAME (eg: freemedforms.com).
Usually, node.js application are running on subdomains like http://myMeteorApplication.meteor.com. 
But, we want our application running inside a subfolder/subpath of our server.
Eg: https://freemedforms.com/myMeteorApplication
As an extension of this process, we want multiple applications running on the same server inside different subfolder/subpath.
Eg: https://freemedforms.com/app1, https://freemedforms.com/app2
Or even in complex subpath, like: https://freemedforms.com/appPath/appSubPath/appName

### nginx configuration
Proxy all request to the Meteor application using a location based proxy.
```yaml
server {
  listen                *:80;
  server_name           SERVER_NAME;
  access_log            /var/log/nginx/app.dev.access.log;
  error_log             /var/log/nginx/app.dev.error.log;

  location /APPNAME {
    proxy_pass http://127.0.0.1:8000;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection 'upgrade';
    proxy_set_header X-Forwarded-For $remote_addr;
  }
}
```

### mup configuration
We have to tell Meteor application that app is running in a folder using the environment variable: ROOT_URL.
Please note that ROOT_URL must **not** end with a '/'.
```javascript
  "env": {
    "PORT": 8000,
    "ROOT_URL": "http://localhost/APPNAME",
    "MONGO_URL": "..."
  },
```

### iron-router

We need to correctly define routes used in [iron:router]: we need to prefix all route path.

Code snippet using coffeescript
```coffeescript
# Get application name (supposed that URL is written like http[s]://[www.]myserver.com/appName/path1/path2/file"
appName = Meteor.absoluteUrl().split("/")[3]
# Add appName to all declared routes
Router.route "/#{appName}/",  name: "welcome"
```

Code snippet using javascript
```javascript
// Get application name (supposed that URL is written like http[s]://[www.]myserver.com/appName/path1/path2/file"
var appName = Meteor.absoluteUrl().split("/")[3];
// Add appName to all declared routes
Router.route("/"+appName+"/",  name: "welcome");
```

### TODO: add SEO


# Install and configure git on the server_name

Follow the [official tutorial](http://git-scm.com/book/en/v2/Git-on-the-Server-Setting-Up-the-Server)


# Some links: 
- http://docs.mongodb.org/manual/tutorial/install-mongodb-on-debian/
- https://gentlenode.com/journal/meteor-1-deploy-and-manage-a-meteor-application-on-ubuntu-with-nginx/1
- http://pascoual.github.io/meteor-app-on-debian
- http://alexbachuk.com/deploying-meteor-application/
- http://websquared.ca/deploying-meteor-js-to-production-server-redhat-centos-debian-ubuntu/
- http://wiki.nginx.org/Configuration
- http://timita.org/wordpress/2013/10/21/multiple-meteor-js-apps-on-the-same-domain-under-virtual-directories/
- http://superuser.com/questions/386564/how-to-set-up-two-node-js-apps-in-one-nginx-config

[mup]:https://github.com/arunoda/meteor-up
[iron:router]:https://github.com/EventedMind/iron-router
[Meteor.js]:http://meteor.com



# Infrastructure

## Recap of all passwords

Connection to OVH server (ip is 5.196.28.226, hostname is vps377318.ovh.net) : there are 2 users (we give user:password), root:PBhPNiX4 and bio:pxFa4xALQFx. The command is :

~~~
ssh bio@vps377318.ovh.net
~~~

For mysql, we have 2 users : root:r$Qet7e3*wrE and website:B7p3ed!FuBr! (use website).

For phpmyadmin, the application password is root.

## Add a sudo user

To add a user on the server and give it sudo privileges :

~~~
ssh root@vps377318.ovh.net
adduser bio
usermod -aG sudo bio
~~~

## Git

Install git and tig :

~~~
sudo apt-get install git
sudo apt-get install tig
~~~

Generate RSA private and public keys on the server with :

~~~
ssh-keygen
~~~

Add the public key (in .ssh/id_rsa.pub) in the keys able to read/write this repo in bitbucket (avatar --> bitbucket settings --> add key). Finally, configure the identity of the server (the name appearing when pushes are made from the server) as requested the first time you push.

Clone the repository :

~~~
git clone git@bitbucket.org:biosimul/bio_simulation.git
~~~

## Database

First, install mysql-server on the server with :

~~~
sudo apt-get install mysql-server
~~~

The password set in the installation for user 'root' is 'root'. Then, secure the installation by executing :

~~~
/usr/bin/mysql_secure_installation
~~~

You can then connect in command line to the database with :

~~~
mysql -uroot -p
~~~

The command 'status' enables to get a summary of the tables existing in the database. To be able to connect remotely to the database (django needs it), we need to activate remote access for MySQL database the following way :

* open file /etc/mysql/my.cnf
* find the line bind-address : 127.0.0.1 and replace the ip by the server's ip
* restart mysql server : sudo service mysql restart

## PhpMyAdmin

Install with :

~~~
sudo apt-get install phpmyadmin
~~~

During the installation, choose apache2 http server. The password set during the installation is 'root'. In the file /etc/apache2/apache2.conf, add in the end of the file the following line :

~~~
Include /etc/phpmyadmin/apache.conf
~~~

Restart apache2 server with :

~~~
sudo service apache2 restart
~~~

We need also to configure a sql user which can connect from all clients. We have created the user 'website' with password 'website'.

## Website

Note that an admin account exists on the website : user is admin and password is bio_simulation.

### Requirements

Install django :

~~~
sudo apt-get install python-pip python-dev libmysqlclient-dev
sudo pip install MySQL-python
sudo pip install django
~~~

We use mod_wsgi to run the website on an apache server (see http://djangobook.com/deploying-django/). Install wsgi with the following doc : http://modwsgi.readthedocs.io/en/develop/user-guides/quick-installation-guide.html.

~~~
wget https://github.com/GrahamDumpleton/mod_wsgi/archive/4.5.14.tar.gz
sudo apt-get install apache2-dev
sudo make
sudo make install (do not forget the sudo otherwise there is a weird error!)
~~~

### Database setup

Create the database from phpmyadmin, and make the migrations :

~~~
python manage.py makemigrations
python manage.py migrate
~~~

### Django settings

Do the following changes in settings.py :

* Add the server's ip address (or host name) in ALLOWED_HOSTS parameter
* Configure the database in DATABASES parameter in the django file settings.py
* Make django sql migrations with : python manage.py makemigrations, and then python manage.py migrate.
* Set DEBUG = False in settings.py.

The following modifications are needed for a website serving in https :

In settings.py :

~~~
# secure proxy SSL header and secure cookies
SECURE_PROXY_SSL_HEADER = ('HTTP_X_FORWARDED_PROTO', 'https')
SESSION_COOKIE_SECURE = True
CSRF_COOKIE_SECURE = True

# session expire at browser close
SESSION_EXPIRE_AT_BROWSER_CLOSE = True

# wsgi scheme
os.environ['wsgi.url_scheme'] = 'https'
~~~

In wsgi.py :

~~~
os.environ['HTTPS'] = "on"
~~~

### Setup SSL certificate (https)

We have a SSL certificate bought on Namecheap. The Namecheap account is GregoireJauvion:TuringBiotech.

The following commands have been types in /etc/apache2/ssl/.

First, generate a private key with :

~~~
sudo openssl genrsa -des3 -out apache.key 2048
~~~

The pass phrase (needed when restarting apache2 server) is Turing.

Then, generate the csr file with :

~~~
sudo openssl req -new -key /etc/apache2/ssl/apache.key -out apache.csr
~~~

When the common name is asked for, we typed turingbio.tech (www.turingbio.tech would be better, but it works).

Finally :

~~~
sudo openssl x509 -req -days 365 -in apache.csr -signkey apache.key -out apache.crt
~~~

These files are needed by Namecheap to generate the certificate. Then, the certificate is received by e-mail and copied in /etc/apache2/ssl/.

Then enable SSL on apache :

~~~
sudo a2enmod ssl
~~~

### Apache server configuration

Below the configuration files of the Apache server.

The following lines have been added in /etc/apache2/apache2.conf :

~~~
WSGIPythonPath /home/bio/bio_simulation/website/bio_simulation

#WSGIDaemonProcess simulation python-path=/home/bio/bio_simulation/website/bio_simulation:/usr/local/lib/python2.7/site-packages
#WSGIProcessGroup simulation
~~~

Note : the 2 commented lines used to work but do not work anymore, I think they are not really needed.

Finally, define the wsgi module configuration in /etc/apache2/sites-available/default-ssl.conf. The first  :

~~~
<IfModule mod_ssl.c>
        <VirtualHost _default_:443>
                ServerName www.turingbio.tech
                ServerAdmin admin@turingbio.tech

                ErrorLog ${APACHE_LOG_DIR}/error.log
                CustomLog ${APACHE_LOG_DIR}/access.log combined

                # Django website configuration
                LoadModule wsgi_module     /usr/lib/apache2/modules/mod_wsgi.so

                WSGIScriptAlias / /home/bio/bio_simulation/website/bio_simulation/simulation/wsgi.py

                <Directory /home/bio/bio_simulation/website/bio_simulation/simulation>
                <Files wsgi.py>
                Require all granted
                </Files>
                </Directory>

                Alias /static/ /home/bio/bio_simulation/website/bio_simulation/simulation/static/

                <Directory /home/bio/bio_simulation/website/bio_simulation/simulation/static>
                Require all granted
                </Directory>

                SSLEngine on
                SSLCertificateFile /etc/apache2/ssl/turingbio_tech.crt
                SSLCertificateKeyFile /etc/apache2/ssl/apache.key
                SSLCertificateChainFile /etc/apache2/ssl/turingbio_tech.ca-bundle
                Header set Strict-Transport-Security "max-age=31536000; includeSubDomains"
        </VirtualHost>

        # This part is used to redirect turingbio.tech requests to www.turingbio.tech
        <VirtualHost _default_:443>
                ServerName turingbio.tech
                Redirect permanent / https://www.turingbio.tech/
        </VirtualHost>

        # The 2 following parts are needed to redirect http requests to https requests
        <VirtualHost *:80>
                ServerName www.turingbio.tech
                Redirect permanent / https://www.turingbio.tech
        </VirtualHost>

        <VirtualHost *:80>
                ServerName turingbio.tech
                Redirect permanent / https://www.turingbio.tech
        </VirtualHost>
</IfModule>
~~~

We need also to activate manually the SSL site :

~~~
sudo a2ensite default-ssl
sudo service apache2 reload
~~~

Finally, restart apache server and it should work :

~~~
sudo service apache2 restart
~~~

If you modify the website, you only need to pull your modifications and restart aache2 server.

### Magic step

There is also this step I did but I don't know if that's really necessary :

Execute :

~~~
sudo ln -s /etc/apache2/mods-available/headers.load /etc/apache2/mods-enabled/headers.load
~~~

And add the following line in default-ssl.conf :

~~~
Add Header set Strict-Transport-Security "max-age=31536000; includeSubDomains"
~~~

## Simulation server

First, compile the C++ library :

* Install qmake : sudo apt-get install qt5-default
* Create Makefile : "qmake sim-engine.pro -r -spec linux-g++ CONFIG+=debug"
* In src/Makefile, replace "g++" with "g++ -std=c++11"
* Compile library by executing make in src/
* Copy the library in /usr/lib/

The http server running the simulations is run with the following commands (the logs are written in bio_simulation/server/server.log) :

~~~
nohup python simulation_server.py --host {ip} --lib-path /usr/lib/libsim-engine.so
~~~

If you want to run simulations, use the python object SimulationClient({ip}) (defined in simulation_client.py).

Remarks : because of json library, we need the latest version of gcc (4.9). You can check the version used on the server with gcc --version. If an old version is installed, upgrade it using this link : http://stackoverflow.com/questions/25147363/how-to-install-g-4-9-on-debian-wheezy-armel.

## Python repository

We should set up a pypi repository to host our python modules (low priority).

For the moment, the best strategy is to put the python modules we want to import in /usr/local/lib/python2.7/site-packages/.

## Connect to the server with graphical interface

Pour info, pour se connecter en SSH avec interface graphique depuis un mac (utile pour voir la commande lancée par qtcreator notamment) :

* Install XQuartz : brew install Caskroom/cask/xquartz
* Run XQuartz application : /Applications/Utilities/XQuartz.app/Contents/MacOS/X11
* SSH connection enabling X forwarding : ssh -X pi@...
* Then, you can run any application with a graphical interface, for example : ./usr/bin/qtcreator

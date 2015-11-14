mod_vhost_ldapx
===============
This module enables dynamic configuring of Apache virtual hosts using LDAP backend. There are many other implementations already available for
this functionality. Nevertheless, while some of them did not have all the needed functionality, others were not conveniently packaged for rapid deployment.

This module is a stripped-down version of [mod_vhostx](https://github.com/porjo/mod_vhostx) and [mod_vhost_ldapng](https://github.com/Ardeek/mod-vhost-ldap-ng), distributed as a Debian *.deb installation package.

Features
--------
* Lightweight Apache configuration
* VirtualHosts will reside in LDAP directory tree, results are cached per Apache child.
* If using [ITK MPM](http://mpm-itk.sesse.net) Apache worker, uid/gid or username can be defined in LDAP under which the request will be served.

Requirements
------------
* Apache httpd server >= 2.4
* LDAP server (e.g. OpenLDAP)
* libldap libraries
* Apache mpm-itk (optional)

Install Manually
----------------
Download package from [Bintray repository](https://bintray.com/theranger/debian/mod_vhost_ldapx/_latestVersion) and install:
```
sudo dpkg -i libapache2-mod-vhost-ldapx_X.Y.Z_amd64.deb
```

Install using APT
-----------------
Add Bintray repo and keys:
```
echo "deb https://dl.bintray.com/theranger/debian jessie main" | sudo tee -a /etc/apt/sources.list
wget -qO - https://bintray.com/user/downloadSubjectPublicKey?username=bintray | sudo apt-key add -
```
Update and install:
```
sudo apt-get update
sudo apt-get install libapache2-mod-vhost-ldapx
```

Configuration
-------------
Following configuration directives are supported in `<VirtualHost />`

Key					| Values		| Default	| Description
:---				| :---:			| :---:		| :---
`VHXEnable`			| `on`,`off`	| `off`   	| Enable virtual host lookups
`VHXLdapUrl`		| LDAP URL		| - | LDAP server URL. Accepts LDAP-style URL-s with search scope and filters. `%v` will be substituted with a name of virtual host requested.
`VHXBindDN`			| 				|			| DN to use for binding
`VHXBindPW`			|				|			| Password to use for binding
`VHXDefaultUser`	| username		| 			| Default ITK user which to serve content under. Requires ITK MPM Apache module to be loaded.
`VHXDefaultTTL`		| sec			| 300 		| Lifetime of LDAP cache entries.

Typical LDAP URL looks something like this ldap://localhost/dc=nodomain??sub?(apacheServerName=%v)
Note, that attribute list in LDAP URL can be left empty and is ignored by the module.

```apache
<VirtualHost *:80>
	<IfModule vhost_ldapx_module>
		VHXEnable	On
		VHXLdapUrl	ldap://localhost/dc=nodomain??sub?(apacheServerName=%v)
		VHXBindDN	cn=admin,dc=nodomain
		VHXBindPW	************
	</IfModule>
</VirtualHost>
```

Credits
--------
* Initial codebase from [mod_vhostx](https://github.com/porjo/mod_vhostx) by Ian Bishop.
* Additional ideas regarding pools and caching from [mod_vhost_ldapng](https://github.com/Ardeek/mod-vhost-ldap-ng) by Ardeek.
* Debian package cmake build script from [cmake-debhelper](https://github.com/sebknzl/cmake-debhelper) by sebknzl.

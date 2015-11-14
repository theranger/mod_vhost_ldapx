LDAP Backend
============
This module gets configuration data from existing LDAP database. Read-only access is sufficient since no information is written back to directory.
Data is read from predefined attributes, no attribute mapping is currently possible. For a prebuilt version of this module to work, directory must be extended with the following schema.

This folder contains schema definitions both old in `*.schema` style as well as in newer OLC `*.ldif` format.

Sample LDAP Object
------------------

Using the schema something like this should be stored in LDAP:

```
dn: cn=Virtual User,dc=example,dc=net
objectClass: posixAccount
objectClass: apacheConfiguration
cn: Virtual User
homeDirectory: /srv/www
uid: web-user
uidNumber: 1000
gidNumber: 1000
apacheServerName: example.com
apacheDocumentRoot: /srv/www
apacheServerAdmin: web-user@example.com
```
Since `apacheConfiguration` is an `AUXILIARY` object class, it cannot form an object by its own, rather it exists to extend any existing objects in a directory. 

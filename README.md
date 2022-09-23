# kind

kind - **k**ind **i**s **n**ot **d**irvish

**kind** is a tool to organize backups on harddisks. 
It is inspired and follows the same principles as **dirvish**.

A backup server running kind creates images (snapshots) of 
directory trees on different hosts (clients) on a regular basis. 
Each image is a complete copy, but unchanged files are 
shared between different images as hardlink. 

Basic tool for this is **rsync**, which must be installed on the 
server and the clients. rsync may be used via ssh or as server.

For an automated backup via ssh the server must have ssh access 
to the clients without password (using public key authentication).

If the client runs rsync as server it must be configured to
allow the server to read the modules to backup. 

**Windows** clients can be backed up using **DeltaCopy** as rsync server on Windows.

##Installation

Make sure, that rsync, ssh and find are installed correctly on server and clients.

Make sure, that the backup user can login from server to the client host[s] 
without password (using public key authentication)

clone the repository and build kind using
```
- make dep
- make
```
in the directory src. Copy kind to a directory on server (e.g. /usr/sbin)

**or**

copy the statically linked binary (for x86-64) from directory bin to the server.

To use man as help for kind:
```
copy man/kind.8 to /usr/share/man/man8
copy man/kind.conf.5 to /usr/share/man/man5
```

##Usage
- Create a directory used as "bank" on a filesystem with enough space
- Create a master config file /etc/kind/master.conf, containing at 
least the bank:
```
    bank=/disk1/kind
```
- Create one or more subdirectories of the bank as "vault" for the backups
- Create a subdirectory kind in each vault directory
- Create a vault config file /[bank]/[vault]/kind/vault.conf, containing
```
    host=[client host]

    user=[backup user on client]
    path=[path to backup on client]

```
- Run kind (on server) first time:
```
    kind -f [vault]
```
- Run kind regularly (with cron):
```
    kind [vault]
```

See the [wiki](https://github.com/OWeh55/kind/wiki) for more information.

Use **man kind** and **man kind.conf** for more help

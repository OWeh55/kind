# kind
kind - kind is not dirvish

kind is a tool to organize backups on harddisks. 
It is inspired and follows the same principles as dirvish.

A backup server running kind creates images (snapshots) of 
directory trees on different hosts (clients) on a regular basis. 
Each image is a complete copy, but unchanged files are 
shared between different images as hardlink. 

Basic tool for this is rsync, which must be installed on the 
server and at the clients. rsync may be used via ssh or as server.

For an automated backup via ssh the server must have ssh access 
to the clients without password (using public key authentication).

If the client runs rsync as server it must be configured to allow the
server to read the modules to backup. 

Windows clients can backed up using DeltaCopy as rsync server.


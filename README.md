# Server Status Check:

A simple C++ script for keeping track of a server's uptime
By Kenneth Burchfiel
Released under the MIT license

(This script was written to help keep track of my personal NextCloud server, but it could likely work with other server types as well.)

This script will:
1. Calculate the current time
2. Update a local laptop uptime log
3. Attempt to download a 'latest_uptime.txt' file from a server
4. (If this download is successful) update a local server uptime
log; create a new copy of this latest_uptime.txt file (that will 
store the current time calculated earlier); and push the 
new latest_uptime.txt file back to the server

(Note: an earlier version of this script uploaded the server and 
laptop uptime logs to the server as well; however, as these files
grow in size, this would quickly use up a great deal of bandwidth.
The current approach, which only downloads and uploads a 25-byte
file, is far less resource intensive.)

Prerequisites:
1. Make sure that a 'server_uptime_folder' is present within
your server's file system at the desired path, and that 
a 'latest_uptime.txt' file is present within this folder. (The
file can be empty.)
2. Update paths/directories as needed.

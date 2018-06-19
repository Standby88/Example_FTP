# Example_FTP

A simple pair of programs to set up a FTP server and a client

The Server should be given a port number to setup the socket, after which it will turn itself into a daemon process
and once it has an incoming connection from a client it will create a child process to handle it.

The Client program needs to be given at least a host address (which will run it on a default port) to connect to the Server,
otherwise it should be given both a host address and a port number (in that order) to connect to the Server.

The Client has a number of commands it can carry out
1. pwd - print working directory for the SERVER
2. lpwd - print working directory for the CLIENT
3. cd <path> - change working directory for the SERVER to that in path
4. lcd <path> - change working directory for the CLIENT to that in path
5. dir - display all files in the current directory for the SERVER
6. ldir - display all files in the current directory for the CLIENT
7. get <filename> - creates a local copy of <filename> in the CLIENT's current working directory
8. put <filename> - copies a local file of <filename> in the SERVER's current working directory
9. quit - exits the program and closes the connection
10. help - displays these options

# makefile for Assignment 2
# the filename must be either Makefile or makefile

all: myftp myftpd clean

myftp: FTPClientFunctions.o client.o token.o
	gcc -o myftp FTPClientFunctions.o client.o token.o

myftpd: FTPServerFunctions.o server.o 
	gcc -o myftpd FTPServerFunctions.o server.o 

token.o: token.c token.h
	gcc -c token.c
	
FTPClientFunctions.o: FTPClientFunctions.c FTPClientFunctions.h
	gcc -c FTPClientFunctions.c

FTPServerFunctions.o: FTPServerFunctions.c FTPServerFunctions.h
	gcc -c FTPServerFunctions.c

client.o: client.c
	gcc -c client.c

server.o: server.c 
	gcc -c server.c

clean:
	rm *.o
cleanall:
	rm myftp myftpd *.o

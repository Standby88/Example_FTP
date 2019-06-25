#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>

#include "token.h"

//Client Functions
int ConnectToServer();
int ConnectToServerH(char host[]);
int ConnectToServerHP(char host[], int port);
void ProcessInput(char* buffer, int sd);
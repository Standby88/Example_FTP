#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>

//Server Functions
void WriteToLogFile(char* msg);
void CreateLogFile(char* dir);
int WaitForRequest(int sd);
int HandleRequest(int sd);
int StartServer();
int StartServerD();

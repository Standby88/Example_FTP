#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FTPServerFunctions.h"

int main(int argc, char *argv[])
{

	int sd;
	int request_sd;
	pid_t pid;

	switch(argc)
	{
		case 1:		//Default Directory Assumed
		
			sd = StartServer();
			break;
		
		case 2:		//Directory Provided

			sd = StartServerD(argv[1]);
			break;

		default:	//Incorrect Usage
		
			printf("Usage: %s [ <server port> ] \n", argv[0]);
			exit(1);
			break;
	}

	while(1)
	{
		request_sd = WaitForRequest(sd);

		if((pid = fork()) < 0)
		{
			printf("Failed to fork() process.\n");
			exit(1);
		}
		else if (pid > 0)
		{
			close(request_sd);
			continue;
		}

		close(sd);

		while(HandleRequest(request_sd) == 0);
			
		exit(0);

	}
}

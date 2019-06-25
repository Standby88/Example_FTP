#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FTPClientFunctions.h"


int main(int argc, char *argv[])
{
	char buffer[8192];
	int inputLength, sd;

	//Obtain Server address and Post number from command line arguments.
	switch(argc)
	{
		case 1:		//Local Host and Default Port Assumed
			sd = ConnectToServer();
			break;
		
		case 2:		//Host Provied and Default Port Assumed
			sd = ConnectToServerH(argv[1]);
			break;
		
		case 3:		//Host and Port Provided
			sd = ConnectToServerHP(argv[1], atoi(argv[2]));
			break;

		default:	//Incorrect Usage
			printf("Usage: %s [ <server host name> [ <server port> ] ] \n", argv[0]);
			exit(1);
			break;
	}

	while(1)
	{
		printf("> ");

		fgets(buffer, sizeof(buffer), stdin);
		inputLength = strlen(buffer);

		if(buffer[inputLength-1] == '\n')
		{
			buffer[inputLength-1] = '\0';
			inputLength = inputLength - 1;
		}

		ProcessInput(buffer, sd);
	}

}
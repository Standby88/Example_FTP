#include "FTPClientFunctions.h"

#define SERVER_TCP_PORT 40003

int GetDefaultServerPort()
{
	return SERVER_TCP_PORT;
}

int SetupSocket(int sd)
{
	if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Error: Failed to setup socket.\n");
		exit(1);
	}

	printf("Socket Setup.\n");

	return sd;
}

struct sockaddr_in BuildClientSocket(char host[], int port, struct sockaddr_in server_addr)
{
	struct hostent *hostp;

	memset((char*) &server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;						
	server_addr.sin_port = htons(port);

	if((hostp = gethostbyname(host)) == NULL)
	{
		printf("Error: Host %s is not found.\n", host);
		exit(1);
	}

	server_addr.sin_addr.s_addr = *(unsigned long*)hostp->h_addr;


	printf("Socket Built.\n");

	return server_addr;
}

int ConnectClientToServer(char host[], int port)
{
	int sd;
	struct sockaddr_in server_addr;
	server_addr = BuildClientSocket(host, port, server_addr);

	sd = SetupSocket(sd);

	if((connect(sd, (struct sockaddr*) &server_addr, sizeof(server_addr))) < 0)
	{
		printf("Error: Failed to connect to server.\n");
		exit(1);
	}

	printf("Client Connected.\n");

	return sd;

}

int ConnectToServer()
{
	char host[60];

	gethostname(host, sizeof(host));
	
	return ConnectClientToServer(host, GetDefaultServerPort());
}

int ConnectToServerH(char host[])
{
	return ConnectClientToServer(host, GetDefaultServerPort());
}

int ConnectToServerHP(char host[], int port)
{
	if(port < 1024 || port >= 65536)
	{	
		printf("Error: Port number provided must be between 1024 and 65536.\n");
		exit(1);
	}

	return ConnectClientToServer(host, port);
}

void ProcessInput(char* buffer, int sd)
{
	char *toklist[MAX_NUM_TOKENS];
	int y = tokenise(buffer, toklist);
	
	if(strcmp(toklist[0], "pwd") == 0)
	{
		char code[1] = "D";

		write(sd, code, sizeof(code));
		/*char remote_directory[1024];
		if(read(sd, remote_directory, 1024) <= 0)
			printf("Error: Failed to read pwd request.\n");
		else
			printf("%s\n", remote_directory);*/
		char * temp;
		short stsize;
		read(sd, &stsize, sizeof(stsize));
		//readMSG(sd, temp);
		stsize = ntohs(stsize);
		//printf("%d",stsize);
		temp = malloc(stsize * sizeof(char));
		read(sd, temp, stsize);
		printf("\n%s\n", temp);
	}
	else if(strcmp(toklist[0], "lpwd") == 0)	//WORKING
	{
		char local_directory[1024];
		getcwd(local_directory, 1024);
		printf("%s\n", local_directory);
	}
	else if(strcmp(toklist[0], "dir") == 0)
	{
		char code[1] = "F";

		write(sd, code, sizeof(code));

		char remote_dir[8196];
		if(read(sd, remote_dir, 8196) <= 0)
			printf("Error: Failed to read dir request.\n");
		else
			printf("%s\n", remote_dir);
	}
	else if(strcmp(toklist[0], "ldir") == 0)	//WORKING
	{
		struct dirent **namelist;
		int n, m;

		n = scandir(".", &namelist, NULL, alphasort);
		
		if (n < 0)
			perror("scandir");
		else {
			while (n > 2) {
				n--;
				printf("%s\n", namelist[n]->d_name);
				free(namelist[n]);
			}
			free(namelist);
		}
	}
	else if(strcmp(toklist[0], "cd") == 0)
	{	
		if(y > 1)
		{
			char code[1] = "C";
			char dir[1024];
			strcpy(dir, toklist[1]);
			write(sd, code, sizeof(code));
			write(sd, dir, sizeof(dir));
		}
		else
		{		
			printf("Incorrect Usage.\n");
			printf("Usage: [ command ] [ argument ] \n");
			printf("Type help to see the valid commands available.\n");
		}
	}
	else if(strcmp(toklist[0], "lcd ") == 0)	//WORKING
	{
		if(chdir(toklist[1])< 0) 
		{
			perror("change directory error\n");
		}
	}
	else if(strcmp(toklist[0], "get ") == 0)
	{
		printf("Download the named file from the server's current directory to the client's current directory.\n");
	}
	else if(strcmp(toklist[0], "put ") == 0)
	{
		printf("Upload the named file from the client's current directory to the server's current directory.\n");
	}
	else if(strcmp(toklist[0], "quit") == 0)	//WORKING
	{
		printf("FTP session now terminated.\n");

		char code[1] = "Q";

		write(sd, code, sizeof(code));		

		exit(0);
	}
	else if(strcmp(toklist[0], "help") == 0)	//WORKING
	{
		printf("\n\nHELP PAGE\n\n");
		printf("Possible Commands:\n\n");
		printf("pwd - Displays the current directory of the host server.\n");
		printf("lpwd - Displays the current directory of the client.\n");
		printf("dir - Displays the file names in the current directory of the host server.\n");
		printf("ldir - Displays the file names in the current directory of the client.\n");
		printf("cd [ directory_pathname ] - Change the current directory of the host server.\n");
		printf("lcd [ directory_pathname ] - Change the current directory of the client.\n");
		printf("get [ filename ] - Download the named file from the server's current directory to the client's current directory.\n");
		printf("put [ filename ] - Upload the named file from the client's current directory to the server's current directory.\n");
		printf("quit - Terminated current FTP session.\n");
		printf("help - Display this list.\n\n");

	}
	else	//WORKING
	{
		printf("Incorrect Usage.\n");
		printf("Usage: [ command ] [ argument ] \n");
		printf("Type help to see the valid commands available.\n");
	}
}
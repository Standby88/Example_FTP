#include "FTPServerFunctions.h"

#define SERVER_TCP_PORT 40003 

int GetDefaultServerPort()
{
	return SERVER_TCP_PORT;
}

ssize_t getFileSize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return (ssize_t)st.st_size;

    return -1; 
}

void WriteToLogFile(char* msg)
{	
	time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char dateString[24 + strlen(msg)];
    memset(dateString, '\0', sizeof(dateString));	
	strftime(dateString, sizeof(dateString), "%d/%m/%Y - %H:%M:%S - ", tm);
	
	write(STDOUT_FILENO, strcat(dateString, msg), sizeof(dateString));
}

void CreateLogFile(char* dir)
{	
	char dirString[256];
	memset(dirString, '\0', sizeof(dirString));	
	strcpy(dirString, dir);

	int fd = open (strcat(dirString, "server_log.log"), O_WRONLY | O_CREAT | O_APPEND, 0644);
	dup2 (fd, STDOUT_FILENO);
	close(fd);

	WriteToLogFile("Starting Log\n");
}

int SetupSocket(int sd)
{
	if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		WriteToLogFile("Error: Failed to setup socket.\n");
		exit(1);
	}
	
	WriteToLogFile("Socket Setup.\n");

	return sd;
}

struct sockaddr_in BuildServerSocket(int port, struct sockaddr_in server_addr)
{
	
	memset((char*) &server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;						
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	WriteToLogFile("Socket Built.\n");
	return server_addr;
}

void claim_children()
{
     pid_t pid=1;
     
     while (pid>0) { /* claim as many zombies as we can */
         pid = waitpid(0, (int *)0, WNOHANG); 
     } 
}

int WaitForRequest(int sd)
{
	int new_sd;
	struct sockaddr_in client_addr;
	socklen_t client_addrlen = sizeof(client_addr);

	if((new_sd = accept(sd, (struct sockaddr*) &client_addr, &client_addrlen)) < 0)
	{
		WriteToLogFile("Error: Failed to accept client request.\n");
		exit(1);
	}

	WriteToLogFile("Waiting For Client Request.\n");

	return new_sd;
}

int HandleRequest(int sd)
{
	char buffer[1];

	if(read(sd, buffer, sizeof(buffer)) <= 0)
		WriteToLogFile("Error: Failed to read request.\n");
	else
	{	

		if(buffer[0] == 'D')
		{
			char local_directory[1024];
			getcwd(local_directory, 1024);
			write(sd, local_directory, sizeof(local_directory));
		}
		else if(buffer[0] == 'F')
		{
			struct dirent **namelist;
			int n, m;

			n = scandir(".", &namelist, NULL, alphasort);
			
			if (n < 0)
				perror("scandir");
			else {
				char fileList[8196];
				while (n > 2) {
					n--;
					strcat(fileList, namelist[n]->d_name);
	
					if(n != 3)
						strcat(fileList, "\n");
	
					free(namelist[n]);
				}

				free(namelist);
				write(sd, fileList, 8196);
			}
		}
		else if(buffer[0] == 'C')
		{	
			char new_directory[1024];
			if(read(sd, new_directory, sizeof(new_directory)) <= 0)
				WriteToLogFile("Error: Failed to read cd request.\n");
			else
			{
				if(chdir(new_directory) < 0) 
					WriteToLogFile("Error: Failed directory change.\n");
			}

		}
		else if(buffer[0] == 'G')
		{
			char file_name[1024];
			if(read(sd, file_name, sizeof(file_name)) <= 0)
				WriteToLogFile("Error: Failed to read get request.\n");
			else
			{	
				int fileSize = htonl(getFileSize(file_name));
				write(sd, &fileSize, sizeof(fileSize));

				int sentBytes = 0;
				off_t offset = 0;
				int rData = ntohl(fileSize);
				int fd = open(file_name, O_RDONLY);
				if(fd < 0)
					WriteToLogFile("File Error");
				else
				{
					while (((sentBytes = sendfile(sd, fd, &offset, rData)) > 0) && (rData > 0))
					{
						rData -= sentBytes;
					}
					close(fd);
				}
			}	
		}
		else if(buffer[0] == 'P')
		{
			char file_name[1024];
			if(read(sd, file_name, sizeof(file_name)) <= 0)
				WriteToLogFile("Error: Failed to read get request.\n");
			else
			{	
				int fSize;
				read(sd, &fSize, sizeof(fSize));
				fSize = ntohl(fSize);

				FILE *rFile;
				int rData = 0;
				int rBytes = 0;
				char fileBuffer[fSize+1];
				if((rFile = fopen(file_name, "w")) == NULL)
					WriteToLogFile("Error: Put File.\n");
				else
				{
					rData = fSize;
					while (((rBytes = read(sd, fileBuffer, rData)) > 0) && (rData > 0))
					{	
						fwrite(fileBuffer, sizeof(char), rBytes, rFile);
						rData -= rBytes;
					}
					fclose(rFile);
				}
				
			}
		}
		else
		{
			WriteToLogFile("Client Disconnected.\n");
			return 1;
		}
	}

	return 0;

}

int SetupServer(int port)
{	
	
	int sd;
	struct sockaddr_in server_addr;
	server_addr = BuildServerSocket(port, server_addr);

	sd = SetupSocket(sd);

	if(bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
	{
		WriteToLogFile("Error: Failed to bind server socket.\n");
		exit(1);
	}

	WriteToLogFile("Socket Bound.\n");

	listen(sd, 5);
	WriteToLogFile("Socket now a listing socket..\n");
	return sd;

}

void daemon_init(char* dir)
{
	pid_t pid;
	struct sigaction act;

	pid = fork();

	if(pid < 0)
	{
		WriteToLogFile("Error: Failed to fork() process.\n");
		exit(1);
	} 
	else 
		if(pid > 0)
			exit(0);

	setsid();
	chdir(dir);
	umask(0);

	act.sa_handler = claim_children;
	sigemptyset(&act.sa_mask);
	act.sa_flags   = SA_NOCLDSTOP;
	sigaction(SIGCHLD,(struct sigaction *)&act,(struct sigaction *)0);
}

int StartServer()
{
	CreateLogFile("./Logs/");
	daemon_init("./");
	return SetupServer(GetDefaultServerPort());
}

int StartServerD(char* dir)
{
	CreateLogFile(dir);
	daemon_init(dir);
	return SetupServer(GetDefaultServerPort());
}

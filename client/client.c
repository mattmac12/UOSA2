#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <string.h>

// Create socket
// Create connection

// Functionality

// Close

const int BACK_LOG = 5;

int createTCPSocket()
{
	return socket(AF_INET, SOCKET_STREAM, 0);
}

int createConnection(int sd, struct sockaddr* RSA, int RSAlen)
{
	return connect(sd, RSA, RSAlen); // Backlog assumed to be 5
	// If 0 exit
}

int send(int tosend)
{
	if (sizeof(tosend) == 2)
	{
		return htons(tosend);
	}
	else
	{
		return htonl(tosend);
	}

	// followed by write() command
}

int receiver(int fromread)
{
	// read command first (toread)

	if (sizeof(int) == 2)
	{
		return ntoh(fromread);
	}
	else
	{
		return ntohl(fromread);
	}
}


// Client -> server communication requires protocol specification
// Put is done in lab 11
// address for get
void helpMenu()
{
	printf("*******************************************************************************************************");
	printf("---------------------------------------------- HELP MENU ----------------------------------------------");
	printf("pwd - Display the current directory of the server.");
	printf("lpdw - Display your current directory.");
	printf("dir - Display the file names under the current directory of the server.");
	printf("ldir - Display the file names under your current directory.");
	printf("cd <directory_pathname> - Change the current directory of the server to a specified directory.");
	printf("lcd <directory_pathname> - Change your current directory to the specified directory.");
	printf("get <filename> - Download the file with specified filename from the servers current directory.");
	printf("put <filename> - Upload the file with specified filename from your current directory to the server.");
	printf("quit - Terminate the session and close the program.");
	printf("*******************************************************************************************************");
}

int main(int argc, char* argv[])
{
	int socket;

	socket = createTCPSocket();
}


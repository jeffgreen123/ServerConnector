#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include<Mswsock.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "mswsock.lib")
enum command{
	MOUSERIGHT = 1,
	MOUSELEFT,
	MOUSELEFTDOUBLE,
	KEYPRESS,
	SCREENSHOT,
	CHANGEIMGQUAL,
	DISCONNECT,
	SHUTDOWNSERVER
};
class Server
{
public:
	Server(char * newport);
	bool SetupServer();
	bool WaitForClient();
	bool ShutdownSocket();
	char* RecieveData(int size) const;
	bool SendData(char *data, int size) const;
	~Server();
private:
	
	WSADATA wsaData;

	SOCKET ListenSocket;
	SOCKET ClientSocket;
	char * port;
	struct addrinfo *resultaddr;
	struct addrinfo addrHints;

};


#include "Server.h"


Server::Server(char * newport)
{
	port = newport;
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	resultaddr = NULL;
	addrHints.ai_family = AF_INET;
	addrHints.ai_socktype = SOCK_STREAM;
	addrHints.ai_protocol = IPPROTO_TCP;
	addrHints.ai_flags = AI_PASSIVE;

}
bool Server::SetupServer(){
	int iResult;

	//intialize winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return false;
	}
	//resolve address and port
	iResult = getaddrinfo(NULL, port, &addrHints, &resultaddr);
	if (iResult != 0) {
		WSACleanup();
		return false;
	}
	// setup socket to connect to
	ListenSocket = socket(resultaddr->ai_family, resultaddr->ai_socktype, resultaddr->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(resultaddr);
		WSACleanup();
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, resultaddr->ai_addr, (int)resultaddr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(resultaddr);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(resultaddr);
	// listen to socket
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

}
bool Server::WaitForClient(){

	ClientSocket = accept(ListenSocket, NULL, NULL);
	int iTimeout = 60000;
	int iRet = setsockopt(ClientSocket,
		SOL_SOCKET,
		SO_RCVTIMEO,
		(const char *)&iTimeout,
		sizeof(iTimeout));

	if (ClientSocket == INVALID_SOCKET) {
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}
}
bool Server::ShutdownSocket(){
	
	int iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		closesocket(ClientSocket);
		WSACleanup();
		return false;
	}

	// listen to socket
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}
	return true;
}

char* Server::RecieveData(int size) const{

	char * ret = new char[size];
	int iResult = recv(ClientSocket, ret, size, 0);
	return ret;
}

bool Server::SendData(char *data, int size) const{
	
	int iSendResult = 0;

	// if size is larger than 1000 -- use transmit file

	if (size > 1000){
		HANDLE hFile = CreateFile("D:/screen.png", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		TransmitFile(ClientSocket, hFile, size, size, NULL, NULL, TF_USE_KERNEL_APC | TF_WRITE_BEHIND);
		CloseHandle(hFile);
		return true;
	}

	iSendResult = send(ClientSocket, data, size, 0);
	if (iSendResult == SOCKET_ERROR) {
		closesocket(ClientSocket);
		WSACleanup();
		return false;
	}
	return true;

}

Server::~Server()
{
}

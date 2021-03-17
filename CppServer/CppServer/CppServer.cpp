#include "CppServer.h"

using namespace std;

//SERVER

TcpServer::TcpServer() {}

TcpServer::~TcpServer() { if (status == server_status::up) stop(); }

void TcpServer::setPort(unsigned short new_port) 
{ 
	port = new_port; 
	if (status == server_status::up)
		stop();
}

TcpServer::server_status TcpServer::start()
{
#ifdef WIN32
	WSAStartup(MAKEWORD(2, 2), &wData);
#endif
	WIN_AND_UNIX_SOCKADDR_IN address;
	address.SIN_ADDR = INADDR_ANY;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;
	serverS = socket(AF_INET, SOCK_STREAM, 0);
	if (serverS == -1) return status = server_status::socket_err;
	printf("Server configured successfully\n");
	
	if (bind(serverS, (struct sockaddr*)&address, sizeof(address)) < 0) return status = server_status::socket_err;
	printf("Socket binded successfully\n");
	
	if (listen(serverS, SOMAXCONN) < 0) return status = server_status::socket_err;
	printf("Start listenin at port%u\n", ntohs(address.sin_port));
	
	status = server_status::up;
	return status;
}

void TcpServer::handle()
{
	while (status == server_status::up)
	{
		WIN_AND_UNIX_SOCKET acceptS;
		WIN_AND_UNIX_SOCKADDR_IN clientAddr;
		ADDR addrlen = sizeof(clientAddr);
		if ((acceptS = accept(serverS, (struct sockaddr*)&clientAddr, &addrlen)) != 0)
		{
			unsigned int ip = clientAddr.SIN_ADDR;
			printf("Sended Client connected from 0  %u.%u.%u.%u:%u\n",
				(unsigned char)(&ip)[0],
				(unsigned char)(&ip)[1],
				(unsigned char)(&ip)[2],
				(unsigned char)(&ip)[3],
				ntohs(clientAddr.sin_port));
			TcpClient* client = new TcpClient(acceptS, clientAddr);
			client->~TcpClient();
		}
		SLEEP(50);
	}
}

void TcpServer::stop()
{
	status = server_status::off;
	CLOSESOCKET(serverS);
#ifdef WIN32
	WSACleanup();
#endif
	printf("Server was stoped\n");
}

//CLIENT

TcpClient::TcpClient(WIN_AND_UNIX_SOCKET socket, WIN_AND_UNIX_SOCKADDR_IN address)
{
	clientS = socket;
	clientAddress = address;
	receive();
}

TcpClient::~TcpClient()
{
	shutdown(clientS, 0);
	CLOSESOCKET(clientS);
	printf("Client disconnect\n");
}

int TcpClient::loadData() { return recv(clientS, buffer, sizeof(buffer), 0); }

char* TcpClient::getData() { return buffer; }

void TcpClient::receive()
{
	int size = 0;
	do {
		size = loadData();
		char* data = getData();
		data[size] = 0;
		if (check(size, data) == 0)
		{	
			cout
				<< "size: " << size << " bytes" << endl
				<< "received data: " << data << endl;
		}
		else { if (size > 0) printf("Error: The received data does not meet the requirements\n"); }
	} while (size > 0);
}

int TcpClient::check(int size, char* data)
{
	if (size > 2)
		if ((atoi(data) % 32) == 0)
			return 0;
	return 1;
}

#pragma once

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define WIN_AND_UNIX_SOCKET SOCKET
#define WIN_AND_UNIX_SOCKADDR_IN SOCKADDR_IN
#define SIN_ADDR sin_addr.S_un.S_addr
#define CLOSESOCKET closesocket
#define SLEEP Sleep
#define ADDR int
#elif __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define WIN_AND_UNIX_SOCKET int
#define WIN_AND_UNIX_SOCKADDR_IN struct sockaddr_in
#define SIN_ADDR sin_addr.s_addr
#define CLOSESOCKET close
#define SLEEP sleep
#define ADDR unsigned int
#endif


class TcpServer
{
public:
	TcpServer();
	~TcpServer();
	enum class server_status
	{
		socket_err = -1,
		off = 0,
		up = 1
	};
	server_status start();
	void stop();
	void handle();
	void setPort(unsigned short new_port);
private:
	server_status status = server_status::off;
	unsigned short port;
	WIN_AND_UNIX_SOCKET serverS;
#ifdef WIN32
	WSAData wData;
#endif
};

class TcpClient
{
public:
	TcpClient(WIN_AND_UNIX_SOCKET socket, WIN_AND_UNIX_SOCKADDR_IN address);
	int loadData();
	char* getData();
	~TcpClient();
	void receive();
	int check(int size, char* data);
private:
	WIN_AND_UNIX_SOCKET clientS;
	WIN_AND_UNIX_SOCKADDR_IN clientAddress;
	char buffer[1024];
};

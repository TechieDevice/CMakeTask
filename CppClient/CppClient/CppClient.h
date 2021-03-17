#pragma once

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#define MAX_LENGTH 64
#ifdef WIN32
#include <winsock2.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#define WIN_AND_UNIX_SOCKET SOCKET
#define WIN_AND_UNIX_SOCKADDR_IN SOCKADDR_IN
#define SIN_ADDR sin_addr.S_un.S_addr
#define CLOSESOCKET closesocket
#define HOST_ADDR inet_addr
#define BUF buf
#elif __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#define WIN_AND_UNIX_SOCKET int
#define WIN_AND_UNIX_SOCKADDR_IN struct sockaddr_in
#define SIN_ADDR sin_addr.s_addr
#define CLOSESOCKET close
#define HOST_ADDR inet_addr
#define BUF shmid
#endif


class TcpClient
{
public:
	TcpClient(); 
	enum class connection_status
	{
		socket_err = -1,
		off = 0,
		conn = 1
	};
	connection_status ClientSet(char* hostIP, unsigned short hostPort);
	~TcpClient();
	bool sendData(char* data_string, size_t size);
private:
	WIN_AND_UNIX_SOCKET _socket;
	WIN_AND_UNIX_SOCKADDR_IN address;
	connection_status status = connection_status::off;
	char buffer[1024];
#ifdef WIN32
	WSAData wData;
#endif
};

#ifdef __linux__
class shared{
    public:
    shared();
	~shared();
    void freeShm();
    void* getShmptr(int id);
    int createShm(int size);
    int getMemId();
    int getSize();
    
    private:
        int shmid;
        int size;
        void* shmptr;
};
#endif

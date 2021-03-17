#include "CppClient.h"

using namespace std;

TcpClient::TcpClient() {}

TcpClient::connection_status TcpClient::ClientSet(char* hostIP, unsigned short hostPort)
{
#ifdef WIN32
    WSAStartup(MAKEWORD(2, 2), &wData);
#endif
    WIN_AND_UNIX_SOCKADDR_IN address;
    address.SIN_ADDR = HOST_ADDR(hostIP);
    address.sin_port = htons(hostPort);
    address.sin_family = AF_INET;
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) return status = connection_status::socket_err;
    printf("Socket configured successfully\n");

    if (connect(_socket, (struct sockaddr*)&address, sizeof(address)) < 0) return status = connection_status::socket_err;

    status = connection_status::conn;
    return status;
}

TcpClient::~TcpClient()
{
    status = connection_status::off;
    CLOSESOCKET(_socket);
#ifdef WIN32
    WSACleanup();
#endif
}

bool TcpClient::sendData(char* data_string, size_t size)
{
    if(send(_socket, data_string, size, 0) < 0) return false;
    return true;
}

#ifdef __linux__
shared::shared()
{
    shmid = 0;
    size = 0;
    shmptr = NULL;
}

shared::~shared() {}
 
int shared::createShm(int size)
{
    shmid = shmget(IPC_PRIVATE, size, 0600 | IPC_CREAT | IPC_EXCL);
    if(!shmid)
        return -1;
    shmptr = shmat(shmid, 0, 0);
    if(!shmptr)
        return -1;
    this->size = size;
    return 1;
}
 
int shared::getMemId()
{
    return shmid;
}

int shared::getSize()
{
    return size;
}
 
void* shared::getShmptr(int id)
{
    shmid_ds buf;
    shmptr = shmat(id, 0, 0);
    size = shmctl(id, IPC_STAT, &buf);
    shmid = id;
    return shmptr;
}
 
void shared::freeShm()
{
    shmctl(shmid, IPC_RMID, NULL);
    size = 0;
    shmptr = NULL;
}
#endif
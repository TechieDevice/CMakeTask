#include "CppServer.cpp"

int main()
{
	TcpServer server;
    int port;
    printf("Enter port");
    scanf("%d", &port);
	server.setPort(port);
    if (server.start() == TcpServer::server_status::up) {
        printf("Server is up!\n");
        server.handle();
    }
    else {
        printf("Server start error!");
        return -1;
    }
	return 0;
}
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

int main(int argc, char** argv)
{
    // Initialize Winsock
    WSADATA wsaData;
    int iresult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iresult != 0) {
        std::cout << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return -1;
    }

    // Create TCP socket as client
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }
    // Create remote address from IP and port
    u_long iMode = 1;
    iresult = ioctlsocket(s, FIONBIO, &iMode);
    if (iresult != NO_ERROR)
    {
        std::cout << "ioctlsocket failed with error : % ld\n" << iresult << std::endl;
    }

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_port = htons(8888);
    inet_pton(AF_INET, "35.85.114.0", &clientService.sin_addr);

    // Connect to server.
    SOCKET result;
    do
    {
        result = connect(s, (sockaddr*)&clientService, sizeof(clientService));
        if (result == INVALID_SOCKET) {
            int error = WSAGetLastError();
            if(error == WSAEISCONN)
                break;
            if(error == WSAEWOULDBLOCK || error == WSAEINVAL || error == WSAEALREADY)
                continue;
        }
    } while (result != 0);

    if (iresult == SOCKET_ERROR) {
        wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
        iresult = closesocket(s);
        if (iresult == SOCKET_ERROR)
            wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //Send data over TCP
    iresult = send(s, static_cast<const char*>(argv[0]), (int)strlen(argv[0]), 0);
    if (iresult == SOCKET_ERROR) {
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(s);
        WSACleanup();
        return -1;
    }

    //If there is nothing to send, shutdown() the connection
    iresult = shutdown(s, SD_SEND);
    if (iresult == SOCKET_ERROR) {
        std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(s);
        WSACleanup();
        return -1;
    }
    std::string recvToString;
    char recvbuf[1000];
    //Listen for data with recv() until it returns 0
    do
    {
        Sleep(100);
        std::cout << ".";
        memset(recvbuf, 0, sizeof(recvbuf));
        iresult = recv(s, recvbuf, sizeof(recvbuf), 0);
        recvToString += recvbuf;
    } while (iresult != 0);
    std::cout << recvToString << std::endl;
    
    //Close socket
    closesocket(s);
    //Shutdown Winsock
    WSACleanup();

    return 0;
}
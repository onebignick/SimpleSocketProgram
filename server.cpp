#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(27015); // Change this to the port number you want to listen on

    iResult = bind(ListenSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (iResult == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 27015..." << std::endl;

    SOCKET ClientSocket;
    sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    while (true) {
        ClientSocket = accept(ListenSocket, (sockaddr*)&clientAddress, &clientAddressSize);
        if (ClientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        char buffer[1024];
        int iResult;

        std::cout << "Client connected!" << std::endl;

        do {
            iResult = recv(ClientSocket, buffer, sizeof(buffer), 0);
            if (iResult > 0) {
                buffer[iResult] = '\0';
                std::cout << "Received message from client: " << buffer << std::endl;

                iResult = send(ClientSocket, buffer, iResult, 0);
                if (iResult == SOCKET_ERROR) {
                    std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
                    closesocket(ClientSocket);
                    WSACleanup();
                    return 1;
                }
            } else if (iResult == 0) {
                std::cout << "Client disconnected." << std::endl;
            } else {
                std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        } while (iResult > 0);

        closesocket(ClientSocket);
    }

    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
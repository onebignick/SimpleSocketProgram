#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(27015);

    iResult = connect(ConnectSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (iResult == SOCKET_ERROR) {
        std::cerr << "connect failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    char buffer[1024];
    std::string userInput;
    while (true) {
        std::cout << "Enter a message to send to the server: ";
        std::getline(std::cin, userInput);

        iResult = send(ConnectSocket, userInput.c_str(), userInput.length(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "send failed withe error: " << WSAGetLastError() << std::endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Sent message to server: " << userInput << std::endl;

        if (userInput == "exit") {
            break;
        }

        iResult = recv(ConnectSocket, buffer, sizeof(buffer), 0);
        if (iResult > 0) {
            buffer[iResult] = '\0';
            std::cout << "Received message from server: "  << buffer << std::endl;
        } else if (iResult == 0)  {
            std::cout << "Server closed connection." << std::endl;
            break;
        } else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
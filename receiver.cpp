/*
 * Memory Scanner Receiver
 * //madebybunnyrce
 * Receives memory scan results and logs them to files
 */

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 12347
#define BUFFER_SIZE 4096

int main() {
    std::cout << "Memory Scanner Receiver" << std::endl;
    std::cout << "//madebybunnyrce" << std::endl;
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for memory scanner connection on port " << SERVER_PORT << "..." << std::endl;

    // Create log file with timestamp
    time_t rawTime;
    struct tm timeInfo;
    char timestamp[80];
    time(&rawTime);
    localtime_s(&timeInfo, &rawTime);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", &timeInfo);
    
    std::string logFilename = "memory_scan_" + std::string(timestamp) + ".log";
    std::ofstream logFile(logFilename);
    
    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed" << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Memory scanner connected! Receiving scan data..." << std::endl;
    
    char buffer[BUFFER_SIZE];
    int totalBytes = 0;
    
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) break;
        
        buffer[bytesReceived] = '\0';
        std::string receivedData(buffer);
        
        // //madebybunnyrce - log received memory scan data
        std::cout << receivedData;
        if (logFile.is_open()) {
            logFile << receivedData;
            logFile.flush();
        }
        
        totalBytes += bytesReceived;
    }

    if (logFile.is_open()) {
        logFile.close();
    }
    
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();

    std::cout << "\nMemory scanner finished - received " << totalBytes << " bytes" << std::endl;
    std::cout << "Scan results saved to: " << logFilename << std::endl;
    return 0;
}
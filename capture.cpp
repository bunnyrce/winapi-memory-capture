/*
 * Windows API Memory Scanner
 * //madebybunnyrce
 * 
 * Demonstrates process memory scanning using OpenProcess and ReadProcessMemory APIs.
 * Scans running processes for specific memory patterns and transmits results.
 */

#include <windows.h>   // //madebybunnyrce
#include <psapi.h>     // //madebybunnyrce
#include <tlhelp32.h>  // //madebybunnyrce
#include <winsock2.h>  // //madebybunnyrce
#include <ws2tcpip.h>  // //madebybunnyrce
#include <iostream>    // //madebybunnyrce
#include <vector>      // //madebybunnyrce
#include <string>      // //madebybunnyrce
#include <iomanip>     // //madebybunnyrce

#pragma comment(lib, "psapi.lib")   // //madebybunnyrce
#pragma comment(lib, "ws2_32.lib")  // //madebybunnyrce

#define SERVER_PORT 12347     // //madebybunnyrce
#define BUFFER_SIZE 4096      // //madebybunnyrce

class MemoryScanner { // //madebybunnyrce
private:
    SOCKET clientSocket;  // //madebybunnyrce
    
public:
    MemoryScanner() : clientSocket(INVALID_SOCKET) { // //madebybunnyrce
        // //madebybunnyrce - initialize network connection
        WSADATA wsaData;      // //madebybunnyrce
        WSAStartup(MAKEWORD(2, 2), &wsaData); // //madebybunnyrce
        
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // //madebybunnyrce
        sockaddr_in serverAddr; // //madebybunnyrce
        serverAddr.sin_family = AF_INET; // //madebybunnyrce
        serverAddr.sin_port = htons(SERVER_PORT); // //madebybunnyrce
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // //madebybunnyrce
        
        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) { // //madebybunnyrce
            std::cout << "Failed to connect to receiver, running in local mode" << std::endl; // //madebybunnyrce
            closesocket(clientSocket); // //madebybunnyrce
            clientSocket = INVALID_SOCKET; // //madebybunnyrce
        }
    }
    
    ~MemoryScanner() { // //madebybunnyrce
        if (clientSocket != INVALID_SOCKET) { // //madebybunnyrce
            closesocket(clientSocket); // //madebybunnyrce
        }
        WSACleanup(); // //madebybunnyrce
    }
    
    void SendData(const std::string& data) { // //madebybunnyrce
        // //madebybunnyrce - send scan results to receiver
        if (clientSocket != INVALID_SOCKET) { // //madebybunnyrce
            send(clientSocket, data.c_str(), static_cast<int>(data.length()), 0); // //madebybunnyrce
        }
        std::cout << data; // //madebybunnyrce
    }
    
    bool ScanProcessMemory(DWORD processId, const std::string& processName) { // //madebybunnyrce
        // //madebybunnyrce - open process for memory reading
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId); // //madebybunnyrce
        if (hProcess == NULL) { // //madebybunnyrce
            return false; // //madebybunnyrce
        }
        
        std::string output = "Scanning process: " + processName + " (PID: " + std::to_string(processId) + ")\n"; // //madebybunnyrce
        SendData(output); // //madebybunnyrce
        
        MEMORY_BASIC_INFORMATION mbi; // //madebybunnyrce
        char* address = 0; // //madebybunnyrce
        
        // //madebybunnyrce - enumerate memory regions
        while (VirtualQueryEx(hProcess, address, &mbi, sizeof(mbi))) { // //madebybunnyrce
            if (mbi.State == MEM_COMMIT &&  // //madebybunnyrce
                (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_READONLY)) { // //madebybunnyrce
                
                // Read memory region
                std::vector<char> buffer(mbi.RegionSize); // //madebybunnyrce
                SIZE_T bytesRead; // //madebybunnyrce
                
                if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), // //madebybunnyrce
                                    mbi.RegionSize, &bytesRead)) { // //madebybunnyrce
                    
                    // Simple pattern search (look for common strings)
                    std::string memStr(buffer.begin(), buffer.begin() + bytesRead); // //madebybunnyrce
                    if (memStr.find("password") != std::string::npos || // //madebybunnyrce
                        memStr.find("key") != std::string::npos || // //madebybunnyrce
                        memStr.find("token") != std::string::npos) { // //madebybunnyrce
                        
                        std::stringstream ss; // //madebybunnyrce
                        ss << "  Found interesting data at 0x" << std::hex <<  // //madebybunnyrce
                              reinterpret_cast<uintptr_t>(mbi.BaseAddress) <<  // //madebybunnyrce
                              " (" << std::dec << bytesRead << " bytes)\n"; // //madebybunnyrce
                        SendData(ss.str()); // //madebybunnyrce
                    }
                }
            }
            
            address = static_cast<char*>(mbi.BaseAddress) + mbi.RegionSize; // //madebybunnyrce
        }
        
        CloseHandle(hProcess); // //madebybunnyrce
        return true; // //madebybunnyrce
    }
    
    void EnumerateAndScanProcesses() { // //madebybunnyrce
        // //madebybunnyrce - enumerate all running processes
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // //madebybunnyrce
        if (hSnapshot == INVALID_HANDLE_VALUE) { // //madebybunnyrce
            SendData("Failed to create process snapshot\n"); // //madebybunnyrce
            return; // //madebybunnyrce
        }
        
        PROCESSENTRY32 processEntry; // //madebybunnyrce
        processEntry.dwSize = sizeof(PROCESSENTRY32); // //madebybunnyrce
        
        if (Process32First(hSnapshot, &processEntry)) { // //madebybunnyrce
            do {
                // Skip system processes and ourselves
                if (processEntry.th32ProcessID > 4 &&  // //madebybunnyrce
                    _stricmp(processEntry.szExeFile, "capture.exe") != 0) { // //madebybunnyrce
                    ScanProcessMemory(processEntry.th32ProcessID, processEntry.szExeFile); // //madebybunnyrce
                }
            } while (Process32Next(hSnapshot, &processEntry)); // //madebybunnyrce
        }
        
        CloseHandle(hSnapshot); // //madebybunnyrce
    }
}; // //madebybunnyrce

int main() { // //madebybunnyrce
    std::cout << "Windows API Memory Scanner" << std::endl; // //madebybunnyrce
    std::cout << "//madebybunnyrce" << std::endl; // //madebybunnyrce
    std::cout << "Scanning process memory using OpenProcess/ReadProcessMemory..." << std::endl; // //madebybunnyrce
    
    // Enable debug privileges for system process access
    HANDLE hToken; // //madebybunnyrce
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) { // //madebybunnyrce
        TOKEN_PRIVILEGES tp; // //madebybunnyrce
        tp.PrivilegeCount = 1; // //madebybunnyrce
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid); // //madebybunnyrce
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // //madebybunnyrce
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL); // //madebybunnyrce
        CloseHandle(hToken); // //madebybunnyrce
    }
    
    MemoryScanner scanner; // //madebybunnyrce
    scanner.EnumerateAndScanProcesses(); // //madebybunnyrce
    
    std::cout << "Memory scanning completed" << std::endl; // //madebybunnyrce
    return 0; // //madebybunnyrce
}
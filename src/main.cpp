#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#include <fstream>
#include <iostream>
#include <string>

MODULEENTRY32 GetModule(DWORD dwProcID, TCHAR *szModuleName) {
    MODULEENTRY32 foundModule;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry32;
        moduleEntry32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &moduleEntry32)) {
            do {
                std::cout << moduleEntry32.szModule << std::endl;
                if (_tcsicmp(moduleEntry32.szModule, szModuleName) == 0) {
                    foundModule = moduleEntry32;
                    break;
                }
            } while (Module32Next(hSnapshot, &moduleEntry32));
        }
        CloseHandle(hSnapshot);
    }
    return foundModule;
}

int main() {
    int pid;
    std::cout << "Enter PID: ";
    std::cin >> pid;
    std::cout << std::endl;

    std::string moduleName;
    std::cout << "Enter module name: ";
    std::cin >> moduleName;
    std::cout << std::endl;

    std::cout << "Dumping " << moduleName << "..." << std::endl;
    auto module = GetModule(pid, (TCHAR*)moduleName.c_str());
    std::cout << "Module address: " << std::to_string((DWORD_PTR)module.modBaseAddr) << std::endl;
    std::cout << "Module size: " << std::to_string(module.modBaseSize) << std::endl;

    auto buf = new char[module.modBaseSize];
    auto process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    std::cout << "Opened process, dumping memory..." << std::endl;
    SIZE_T bytesRead = 0;
    ReadProcessMemory(process, module.modBaseAddr, buf, module.modBaseSize, &bytesRead);

    std::cout << std::to_string(bytesRead) << " bytes read; saving..." << std::endl;
    std::ofstream outFile(moduleName);
    for (auto i = 0; i < bytesRead; i++) {
        outFile << buf[i];
    }

    std::cout << "Done!" << std::endl;

    delete[] buf;
    return 0;
}

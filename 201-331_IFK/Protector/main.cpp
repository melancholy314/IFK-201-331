#include <iostream>
#include <windows.h>

using namespace std;

int main()
{

    //Запуск приложения
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t cmdLine[] = L"C:/201-331_Ivanov/build-Lab_1-Desktop_Qt_6_4_3_MSVC2019_64bit-Debug/debug/Lab_1.exe";

    if(CreateProcess(cmdLine, NULL, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
        std::cout << "Process id created!" << std::endl;
        std::cout << "Pid = " << std::dec << pi.dwProcessId << std::endl;
    }
    else {
        std::cout << "Process is not created!"<< std::endl;
    }

    //Подключение к приложению как отладчик

    bool isAttached = DebugActiveProcess(pi.dwProcessId);
    if(!isAttached) {
        DWORD lastError = GetLastError();
        std::cout << std::hex << "DebugActiveProcess FAILED, LastError is " << lastError;
    }
    else {
        std::cout << "DebugActiveProcess SUCCESS" << std::endl;
    }

    // Пропускать поступающие сигналы отладки

    DEBUG_EVENT debugEvent;
    while(true) {
        bool result1 = WaitForDebugEvent(&debugEvent, INFINITE);
        bool result2 = ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_CONTINUE);
    }
    return 0;

}
